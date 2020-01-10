#include "SpentFuelAssemblyParameterisation.hh"

SpentFuelAssemblyParameterisation::SpentFuelAssemblyParameterisation(G4int nOfX, G4int nOfY, G4double intervalSize)
: G4VPVParameterisation(), fnOfX(nOfX), fnOfY(nOfY), fIntervalSize(intervalSize)
{
    fStartX = -1.*((static_cast<G4double>(fnOfX) - 1)/2.)*fIntervalSize;
    fStartY = -1.*((static_cast<G4double>(fnOfY) - 1)/2.)*fIntervalSize;
}

SpentFuelAssemblyParameterisation::~SpentFuelAssemblyParameterisation()
{}

void SpentFuelAssemblyParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
    physVol->SetTranslation(GetTranslation(copyNo));
}

G4ThreeVector SpentFuelAssemblyParameterisation::GetTranslation(const G4int copyNo) const
{
    G4int xIdx = copyNo%fnOfX;
    G4int yIdx = copyNo/fnOfX;
    G4double xPos = fStartX + xIdx*fIntervalSize;
    G4double yPos = fStartY + yIdx*fIntervalSize;

    return G4ThreeVector(xPos, yPos, 0.);
}


