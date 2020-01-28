#include "EventAction.hh"
#include "CCHit.hh"
#include "SpentFuelAssemblyBuilder.hh"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4AutoLock.hh"

namespace { G4Mutex aMutex = G4MUTEX_INITIALIZER; }
std::ofstream EventAction::ofs;

EventAction::EventAction()
: G4UserEventAction(), fCCHCID(-1)
{
    G4AutoLock lock(&aMutex);
    if(!ofs.is_open())
    {
        ofs.open("output/coindata.txt");
        SpentFuelAssemblyStore::GetInstance()->GetSpentFuelAssembly("SpentFuelAssembly")->PrintFuelRodStatus(ofs);
        ofs << "# evtID\tParticleWeight\t"
            << "DetID1\tX1(mm)\tY1(mm)\tZ1(mm)\tE1(MeV)\tT1(ns)\t"
            << "DetID2\tX2(mm)\tY2(mm)\tZ2(mm)\tE2(MeV)\tT2(ns)\t\n";
    }
}

EventAction::~EventAction()
{
    G4AutoLock lock(&aMutex);
    if(ofs.is_open()) ofs.close();
}

void EventAction::EndOfEventAction(const G4Event* anEvent)
{
    if(fCCHCID==-1)
        fCCHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LACC/CCData");

    auto HCE = anEvent->GetHCofThisEvent();
    if(!HCE) return;

    auto hitsMap = static_cast<CCHitsMap*>(HCE->GetHC(fCCHCID));
//    if(hitsMap->entries()==0) return; // empty HC
    if(hitsMap->entries()<2) return; // coincidence only

    G4AutoLock lock(&aMutex);
    ofs << anEvent->GetEventID() << "\t";
    ofs.precision(5);
    ofs << std::scientific
        << hitsMap->begin()->second->GetWeight() << "\t";
    for(const auto& itr: *hitsMap)
    {
        ofs.precision(1);
        ofs << itr.first << "\t";
        ofs << std::fixed
            << itr.second->GetPosition().x()/mm << "\t"
            << itr.second->GetPosition().y()/mm << "\t"
            << itr.second->GetPosition().z()/mm << "\t";
        ofs.precision(3);
        ofs << itr.second->GetDepE()/MeV << "\t"
            << itr.second->GetTime()/ns << "\t";
    }
    ofs << "\n";
}

