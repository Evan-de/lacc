#ifndef SPENTFUELASSEMBLYPARAMETERISATION_HH
#define SPENTFUELASSEMBLYPARAMETERISATION_HH

#include "G4VPVParameterisation.hh"
#include "G4VPhysicalVolume.hh"

class SpentFuelAssemblyParameterisation: public G4VPVParameterisation
{
public:
    SpentFuelAssemblyParameterisation(G4int nOfX, G4int nOfY, G4double intervalSize);
    virtual ~SpentFuelAssemblyParameterisation();
    
    virtual void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;

    G4ThreeVector GetTranslation(const G4int copyNo) const;

private:
    G4int fnOfX, fnOfY;
    G4double fIntervalSize;
    G4double fStartX, fStartY;
};

#endif
