#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction_PG.hh"
#include "PrimaryGeneratorAction_GPS.hh"
#include "RunAction.hh"
#include "EventAction.hh"

ActionInitialization::ActionInitialization()
:G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
//    SetUserAction(new PrimaryGeneratorAction_PG());
    SetUserAction(new PrimaryGeneratorAction_GPS());

    SetUserAction(new RunAction());
    SetUserAction(new EventAction);
}
