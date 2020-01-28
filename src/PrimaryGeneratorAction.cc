#include "PrimaryGeneratorAction.hh"
#include "SpentFuelAssemblyBuilder.hh"
#include "PrimarySamplingTools.hh"

#include "G4Tubs.hh"

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

    G4double particleWeight = 1.;
    G4double fuelRodHeight = 2*static_cast<G4Tubs*>(fSpentFuelAssembly->GetFuelRod()->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    particleWeight *= fuelRodHeight/(4.*m);

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

    // source direction
    auto srcDir = SampleDirectionFromTo(fPrimary->GetParticlePosition(), "LACC", particleWeight, 10.*cm);
//    auto srcDir = G4RandomDirection();
    fPrimary->SetParticleMomentumDirection(srcDir);

    // Generate primary
    fPrimary->GeneratePrimaryVertex(anEvent);
    anEvent->GetPrimaryVertex()->SetWeight(particleWeight);
}
