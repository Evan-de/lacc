#include "EventAction.hh"
#include "CCHit.hh"

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
        ofs.open("output/data.txt");
        ofs << std::fixed;
        ofs << "evtID\t"
            << "X1(mm)\tY1(mm)\tZ1(mm)\tE1(MeV)\tT1(ns)\t"
            << "X2(mm)\tY2(mm)\tZ2(mm)\tE2(MeV)\tT2(ns)\t\n";
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
    if(hitsMap->entries()<2) return; // coincidence check

    G4AutoLock lock(&aMutex);
    ofs << anEvent->GetEventID() << "\t";
    for(const auto& itr: *hitsMap)
    {
        ofs.precision(1);
        ofs << itr.second->GetPosition().x()/mm << "\t"
            << itr.second->GetPosition().y()/mm << "\t"
            << itr.second->GetPosition().z()/mm << "\t";
        ofs.precision(3);
        ofs << itr.second->GetDepE()/MeV << "\t"
            << itr.second->GetTime()/ns << "\t";
    }
    ofs << "\n";
}

