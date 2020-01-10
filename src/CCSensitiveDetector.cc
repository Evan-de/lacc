#include "CCSensitiveDetector.hh"
#include "CCHit.hh"

CCSensitiveDetector::CCSensitiveDetector(G4String detName)
: G4VSensitiveDetector(detName), fHitsMap(nullptr), fHCName("CCData")
{
    collectionName.insert(fHCName);
}

CCSensitiveDetector::~CCSensitiveDetector()
{}

void CCSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    fHitsMap = new CCHitsMap(GetName(), fHCName);
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHCName);
    hce->AddHitsCollection(hcID, fHitsMap);
}

G4bool CCSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double eDep = aStep->GetTotalEnergyDeposit();
    if(0. == eDep) return false;

    G4int cpNo = aStep->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(3);
    G4double time = aStep->GetTrack()->GetGlobalTime();
    G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();

    CCHit* thisHit;

    CCHitsMap::iterator itr;
    for(itr = fHitsMap->begin(); itr!=fHitsMap->end(); ++itr)
        if(cpNo==itr->first)
        {
            thisHit = itr->second;
            thisHit->AddDepEAndPosition(eDep, pos);
            break;
        }

    if(itr==fHitsMap->end())
    {
        thisHit = new CCHit(eDep, pos, time);
        fHitsMap->add(cpNo, thisHit);
    }

    return true;
}

