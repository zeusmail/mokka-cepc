//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//*   - the detailed geant4 simulation for Tesla -      *
//*                                                     *
//* For more information about Mokka, please, go to the *
//*                                                     *
//*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
//*                                                     *
//* Mokka home page.                                    *
//*                                                     *
//*******************************************************
//
// $Id: Proto01.hh,v 1.2 2004/07/02 11:07:09 musat Exp $
// $Name: mokka-07-00 $
//
#ifndef Proto01_h
#define Proto01_h 1

class G4LogicalVolume;
class Database;
class G4Material;
class ProtoSD;

#include <vector>
#include "Control.hh"
#include "VSubDetectorDriver.hh"

class WLAYERS {
public:
  WLAYERS(int an_layers,G4int aplate_multiplicity)
    : n_layers(an_layers),plate_multiplicity(aplate_multiplicity),
      AsDeadWLogical(0),AsSlabWLogical(0),AsAlveolusLogical(0),
      AsSlabWTotalHalfY(0),AsDeadTotalHalfY(0),
      AsAlveolusTotalHalfY(0) {}
  
  G4int n_layers,plate_multiplicity;
  G4double x_offset_start,x_offset_step;
  G4LogicalVolume *AsDeadWLogical;
  G4LogicalVolume *AsSlabWLogical;
  G4LogicalVolume *AsAlveolusLogical;
  G4double AsSlabWTotalHalfY;
  G4double AsDeadTotalHalfY;
  G4double AsAlveolusTotalHalfY;
};


class Proto01 : public VSubDetectorDriver
{
public:
  Proto01() : VSubDetectorDriver("proto01","proto"),
	      db(0),theProtoSD(0),Mix(0),
	      theMaxStepAllowed(DBL_MAX)
  { }
  
  ~Proto01();

  G4bool construct(const G4String &aSubDetectorDBName,
			    G4LogicalVolume *WorldLog);

private:
  
  void BuildElements();
  void BuildAlveolus(WLAYERS*);
  void BuildDeadPlate(WLAYERS*);
  
  G4int total_W_plates,n_towers,n_dead_w_plates;
  G4double fiber_thickness,inter_tower_fiber_thickness;
  G4double nominal_w_thickness;

  G4double HalfAlveolusX,HalfAlveolusZ,HalfWSlabX,HalfWSlabZ;
  G4double HalfSuppX,HalfSuppZ,HalfWafferX,HalfWafferY,HalfWafferZ;
  G4double HalfCuY,HalfMixY;

  Database* db;

  ProtoSD* theProtoSD;
  G4Material* Mix;

  G4double HalfDeadWX,HalfDeadWZ,
    tan_rate;

  G4LogicalVolume *WafferLogical,*CuLogical,*KaptonLogical;
  G4LogicalVolume *DetectorLogical,*WorldLog;

 std::vector<WLAYERS*> PlateGroups;
 std::vector<WLAYERS*> Plates;

  G4double theMaxStepAllowed;
};

#endif


