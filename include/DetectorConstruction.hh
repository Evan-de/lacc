#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class LACC;

class DetectorConstruction: public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction() override;

    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

private:
    LACC* fLACC;
};

#endif
