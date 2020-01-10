#include "PrimaryGeneratorAction_PG.hh"

PrimaryGeneratorAction_PG::PrimaryGeneratorAction_PG()
: G4VUserPrimaryGeneratorAction()
{
    fPrimary = std::make_unique<G4ParticleGun>();

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    fPrimary->SetParticleDefinition(particle);
    fPrimary->SetParticlePosition(G4ThreeVector());
    fPrimary->SetParticleEnergy(1.*MeV);
}

PrimaryGeneratorAction_PG::~PrimaryGeneratorAction_PG()
{}

void PrimaryGeneratorAction_PG::GeneratePrimaries(G4Event* anEvent)
{
    fPrimary->SetParticleMomentumDirection(G4RandomDirection());

    fPrimary->GeneratePrimaryVertex(anEvent);
}
