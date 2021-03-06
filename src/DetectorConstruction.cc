#include "DetectorConstruction.hh"
#include "LACCBuilder.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"

#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"

#include "G4PVPlacement.hh"

#include "G4SDManager.hh"
#include "CCSensitiveDetector.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // World
    G4double worldSize = 10.*m;
    G4VSolid* worldSol = new G4Box("World", 0.5*worldSize, 0.5*worldSize, 0.5*worldSize);
    auto nistAir = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto worldLV = new G4LogicalVolume(worldSol, nistAir, "World");
    auto worldVA = new G4VisAttributes(G4Colour::White());
    worldVA->SetForceWireframe(true);
    worldLV->SetVisAttributes(worldVA);
    auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0);

    // LACC
    fLACC = new LACC("LACC");
    G4double laccHeight = 2*static_cast<G4Box*>(fLACC->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -laccHeight/2),
                      fLACC->GetLogicalVolume(), "LACC", worldLV, false, 0);

    return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
    auto sd_Det = new CCSensitiveDetector("LACC");
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_Det);
    SetSensitiveDetector(fLACC->GetScatter()->GetCrystalLV(), sd_Det);
    SetSensitiveDetector(fLACC->GetAbsorber()->GetCrystalLV(), sd_Det);
}
