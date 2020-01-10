#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4ParticleGun.hh"
#include "G4PhysicalVolumeStore.hh"

class SpentFuelAssembly;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction() override;

    virtual void GeneratePrimaries(G4Event*) override;

private:
    std::unique_ptr<G4ParticleGun> fPrimary;

    std::shared_ptr<SpentFuelAssembly> fSpentFuelAssembly;
};

#endif
