#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4NeutronTrackingCut.hh"

PhysicsList::PhysicsList()
: G4VModularPhysicsList()
{
    SetVerboseLevel(1);

    // Decay physics
    RegisterPhysics(new G4DecayPhysics());
    // EM physics
    RegisterPhysics(new G4EmStandardPhysics());
    // Radioactive decay
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Hadron physics
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC());
    RegisterPhysics(new G4StoppingPhysics());
    RegisterPhysics(new G4HadronElasticPhysics());
    RegisterPhysics(new G4EmExtraPhysics());
    RegisterPhysics(new G4IonBinaryCascadePhysics());
    RegisterPhysics(new G4NeutronTrackingCut());
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::SetCuts()
{
    G4VUserPhysicsList::SetCuts();
}
