#include "PrimaryGeneratorAction.hh"
#include "SpentFuelAssemblyBuilder.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
{
    fPrimary = std::make_unique<G4ParticleGun>();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    if(!fSpentFuelAssembly)
        fSpentFuelAssembly = SpentFuelAssemblyStore::GetInstance()->GetSpentFuelAssembly("SpentFuelAssembly");

    // source position
    G4int randomFuelRodCopyNumber = fSpentFuelAssembly->SampleRandomFuelRodID();
    auto randomPointInFuelRod = fSpentFuelAssembly->GetFuelRod()->SampleRandomPointInFuelRod();
    auto fuelRodPosition = fSpentFuelAssembly->GetFuelRodLocation(randomFuelRodCopyNumber);
    auto spentFuelAssemblyRotation =
            G4PhysicalVolumeStore::GetInstance()->GetVolume("SpentFuelAssembly")->GetObjectRotationValue();
    auto spentFuelAssemblyTranslation =
            G4PhysicalVolumeStore::GetInstance()->GetVolume("SpentFuelAssembly")->GetObjectTranslation();
    auto srcPos =
            spentFuelAssemblyTranslation +
            (fuelRodPosition + randomPointInFuelRod).rotate(spentFuelAssemblyRotation.axisAngle());
    fPrimary->SetParticlePosition(srcPos);

    fPrimary->SetParticleMomentumDirection(G4RandomDirection());

    fPrimary->GeneratePrimaryVertex(anEvent);
}
