// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id: ParticleGunGenerator.cc,v 1.2 2007/06/22 14:42:50 musat Exp $
// $Name: mokka-07-00 $

#include "ParticleGunGenerator.hh"
#include "ParticleGunMessenger.hh"

#include "Control.hh"
#include "UserInit.hh"

#include "G4ParticleGun.hh"
#include "G4MuonPlus.hh"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4UIcommand.hh"
#include "Randomize.hh"

#include <fstream>

ParticleGunGenerator::ParticleGunGenerator(void): 
			VPrimaryGenerator("particleGun")
{
  fGunMessenger = new ParticleGunMessenger(this);

  fParticleGun  = new G4ParticleGun(G4MuonPlus::MuonPlus());
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 1, 0));
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));
  fParticleGun->SetParticleEnergy(100 * GeV);

  fGunPositionStep = G4ThreeVector(0, 0, 0);
  fGunPositionSmearing = G4ThreeVector(0, 0, 0);
  fGunThetaStep = 0;
  fGunThetaSmearing = 0;
  fGunPhiStep = 0;
  fGunPhiSmearing = 0;
  fGunMomentumStep = 0;
  fGunMomentumSmearing = 0;
  fGunDirectionSmearingMode = kGaussian;
  fGunMomentumSmearingMode = kGaussian;
}

ParticleGunGenerator::~ParticleGunGenerator(void)
{
  delete fParticleGun;
  delete fGunMessenger;
}

void ParticleGunGenerator::SetGunMomentum(G4double gunMomentum)
{
  // The momentum is not a primary property (i.e. data member) of the gun,
  // so it needs to be converted to the energy (using the given particle mass).
  // Note: "energy" means kinetic energy in this context.
  const G4double gunMass = fParticleGun->GetParticleDefinition()->GetPDGMass();
  const G4double gunEnergy = sqrt(sqr(gunMomentum) + sqr(gunMass)) - gunMass;
  fParticleGun->SetParticleEnergy(gunEnergy);
}

G4double ParticleGunGenerator::GetGunMomentum(void)
{
  // The momentum is not a primary property (i.e. data member) of the gun,
  // so it needs to be calculated from the energy (using the given particle mass).
  // Note: "energy" means kinetic energy in this context.
  const G4double gunMass = fParticleGun->GetParticleDefinition()->GetPDGMass();
  const G4double gunEnergy = fParticleGun->GetParticleEnergy() + gunMass;
  const G4double gunMomentum = sqrt(sqr(gunEnergy) - sqr(gunMass));
  return gunMomentum;
}

void ParticleGunGenerator::GeneratePrimaryVertex(G4Event *evt)
{
  // save old properties (to undo the smearing later)
  const G4ThreeVector savedPosition = fParticleGun->GetParticlePosition();
  const G4ThreeVector savedDirection = fParticleGun->GetParticleMomentumDirection();
  const G4double savedEnergy = fParticleGun->GetParticleEnergy();
  const G4double savedMomentum = this->GetGunMomentum(); // needs to be calculated

  // perform smearing of position, ...
  const G4double smearedPositionX = G4RandGauss::shoot(savedPosition.getX(), fGunPositionSmearing.getX());
  const G4double smearedPositionY = G4RandGauss::shoot(savedPosition.getY(), fGunPositionSmearing.getY());
  const G4double smearedPositionZ = G4RandGauss::shoot(savedPosition.getZ(), fGunPositionSmearing.getZ());
  const G4ThreeVector smearedPosition(smearedPositionX, smearedPositionY, smearedPositionZ);

  // ... direction, ...
  G4ThreeVector smearedDirection(1, 0, 0); // dummy unit vector
  switch (fGunDirectionSmearingMode) {
    case kGaussian:
      smearedDirection.setTheta(G4RandGauss::shoot(savedDirection.getTheta(), fGunThetaSmearing));
      smearedDirection.setPhi(G4RandGauss::shoot(savedDirection.getPhi(), fGunPhiSmearing));
      break;
    case kUniform:
      smearedDirection.setTheta(savedDirection.getTheta() + (2 * G4UniformRand() - 1) * fGunThetaSmearing);
      smearedDirection.setPhi(savedDirection.getPhi() + (2 * G4UniformRand() - 1) * fGunPhiSmearing);
      break;
  }

  // ... and momentum
  G4double smearedMomentum ;
  switch (fGunMomentumSmearingMode) {
    case kGaussian:
      smearedMomentum = G4RandGauss::shoot(savedMomentum, fGunMomentumSmearing);  
      break;
    case kUniform:
      smearedMomentum = savedMomentum + (2 * G4UniformRand() - 1) * fGunMomentumSmearing;
      break;
  }
        
  // apply smeared properties
  fParticleGun->SetParticlePosition(smearedPosition);
  fParticleGun->SetParticleMomentumDirection(smearedDirection);

  if (fGunMomentumSmearing) // avoid unnecessary back-and-forth calculations
    this->SetGunMomentum(smearedMomentum);

  // shoot one particle
  fParticleGun->GeneratePrimaryVertex(evt);

  // perform stepping of position, ...
  const G4ThreeVector steppedPosition = savedPosition + fGunPositionStep;

  // ... direction, ...
  G4ThreeVector steppedDirection(1, 0, 0);
  steppedDirection.setTheta(savedDirection.getTheta() + fGunThetaStep);
  steppedDirection.setPhi(savedDirection.getPhi() + fGunPhiStep);

  // ... and momentum
  const G4double steppedMomentum = savedMomentum + fGunMomentumStep;

  // restore saved properties (plus the steps) without the smearing
  fParticleGun->SetParticlePosition(steppedPosition);
  fParticleGun->SetParticleMomentumDirection(steppedDirection);
  if (fGunMomentumStep) // avoid unnecessary back-and-forth calculations
    this->SetGunMomentum(steppedMomentum);
  else // the old value is still valid
    fParticleGun->SetParticleEnergy(savedEnergy);
}

void ParticleGunGenerator::PrintGeneratorInfo(void)
{
  const G4ParticleDefinition *particleDef = fParticleGun->GetParticleDefinition();
  const G4ThreeVector direction = fParticleGun->GetParticleMomentumDirection();

  // borrow G4UIcommand::ConvertToString(...) for a minute because it does exactly what we need
  G4cout
    << "Particle:       " << particleDef->GetParticleName()
    << " (m = " << G4UIcommand::ConvertToString(particleDef->GetPDGMass(), "GeV") << ")"
    << G4endl;
  G4cout
    << "Kinetic Energy: " << G4UIcommand::ConvertToString(fParticleGun->GetParticleEnergy(), "GeV")
    << " (momentum = " << G4UIcommand::ConvertToString(this->GetGunMomentum(), "GeV") << ")"
    << G4endl;
  G4cout
    << "Position:       " << G4UIcommand::ConvertToString(fParticleGun->GetParticlePosition(), "cm")
    << G4endl;
  G4cout
    << "Direction:      " << G4UIcommand::ConvertToString(direction) // has no unit
    << " (theta = " << G4UIcommand::ConvertToString(direction.getTheta(), "deg")
    << ", phi = " << G4UIcommand::ConvertToString(direction.getPhi(), "deg") << ")"
    << G4endl;
  G4cout << "(Steps and smearing are not displayed.)" << G4endl;
}
