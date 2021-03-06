/*! \file SiLCSD.hh
    \brief A definition of Silc::SiLCSD class.

*******************************************************
*                                                     *
*                      Mokka                          *
*   - the detailed geant4 simulation for Tesla -      *
*                                                     *
* For more information about Mokka, please, go to the *
*                                                     *
*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
*                                                     *
* Mokka home page.                                    *
*                                                     *
*******************************************************

$Id: SiLCSC.hh, copy of TRKSD00.hh written by P.Mora de Freitas $

*/

#pragma once

#include "Silc_Globals.hh"
#include "TRKHit.hh"
#include "VSensitiveDetector.hh"
#include "G4Step.hh"

namespace Silc
{
    class SiLCSD : public VSensitiveDetector
    {

    public:
        SiLCSD(G4String TRKSD00name,
               G4double Threshold,
               G4double thePrimaryTPCCut=0);
        virtual ~SiLCSD() {}

        void Initialize(G4HCofThisEvent*HCE);
        G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
        void EndOfEvent(G4HCofThisEvent*HCE);

        // Basic reload hits from Ascii Files
        void LoadEvent(FILE* theSubDetectorEventHitsFileInput);

    private:
        void StartNewHit(G4int aLayerNumber,
                         G4ThreeVector theEntryPoint,
                         G4ThreeVector theEntryMomentum,
                         G4int thePDG,
                         G4int theSecondaryPID);

        void UpdateHit(G4Step *aStep);

        void DumpHit(G4Step *aStep);

        void Clear();

        G4double Threshold;
        G4double PrimaryTPCCut;
        G4int HCID;
        G4int currentCylinder;
        G4int currentPID;
        G4int currentPDG;
        G4int currentSecondaryPID;

        G4ThreeVector EntryPoint;
        G4ThreeVector ExitPoint;
        G4ThreeVector EntryMomentum;
        G4ThreeVector ExitMomentum;
        G4double DepositedEnergy;
        G4double HitTime ;
        G4double StepLength;

        TRKHitsCollection *CalCollection;
    };
}
