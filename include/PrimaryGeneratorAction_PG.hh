#ifndef PRIMARYGENERATORACTION_PG_HH
#define PRIMARYGENERATORACTION_PG_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4ParticleGun.hh"

class PrimaryGeneratorAction_PG: public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction_PG();
    virtual ~PrimaryGeneratorAction_PG() override;

    virtual void GeneratePrimaries(G4Event*) override;

private:
    std::unique_ptr<G4ParticleGun> fPrimary;
};

#endif
