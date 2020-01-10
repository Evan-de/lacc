#ifndef CCSENSITIVEDETECTOR_HH
#define CCSENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"

#include "G4SDManager.hh"
#include "G4THitsMap.hh"

class CCHit;

class CCSensitiveDetector: public G4VSensitiveDetector
{
public:
    CCSensitiveDetector(G4String name);
    virtual ~CCSensitiveDetector() override;

    virtual void Initialize(G4HCofThisEvent*) override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;

private:
    G4THitsMap<CCHit>* fHitsMap;
    G4String fHCName;
};

#endif // CCSENSITIVEDETECTOR_HH
