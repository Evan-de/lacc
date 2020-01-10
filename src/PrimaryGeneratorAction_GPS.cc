#include "PrimaryGeneratorAction_GPS.hh"

PrimaryGeneratorAction_GPS::PrimaryGeneratorAction_GPS()
: G4VUserPrimaryGeneratorAction()
{
    fPrimary = std::make_unique<G4GeneralParticleSource>();
}

PrimaryGeneratorAction_GPS::~PrimaryGeneratorAction_GPS()
{}

void PrimaryGeneratorAction_GPS::GeneratePrimaries(G4Event* anEvent)
{
    fPrimary->GeneratePrimaryVertex(anEvent);
}
