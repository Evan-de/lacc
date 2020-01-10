#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"

#include <fstream>

class EventAction: public G4UserEventAction
{
public:
	EventAction();
    virtual ~EventAction() override;

    virtual void EndOfEventAction(const G4Event*) override;

private:
    G4int fCCHCID;
    static std::ofstream ofs;
};

#endif

