#ifndef PRIMARYGENERATORACTION_GPS_HH
#define PRIMARYGENERATORACTION_GPS_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"

class PrimaryGeneratorAction_GPS: public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction_GPS();
    virtual ~PrimaryGeneratorAction_GPS() override;

    virtual void GeneratePrimaries(G4Event*) override;

private:
    std::unique_ptr<G4GeneralParticleSource> fPrimary;
};

#endif
