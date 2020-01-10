#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

class RunAction: public G4UserRunAction
{
public:
    RunAction();
    virtual ~RunAction() override;

    virtual void BeginOfRunAction(const G4Run*) override;
};

#endif
