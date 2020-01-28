#include "DetectorConstruction.hh"
#include "LACCBuilder.hh"
#include "SpentFuelAssemblyBuilder.hh"
#include "CCSensitiveDetector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"

#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"

#include "G4PVPlacement.hh"

#include "G4SDManager.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // World
    G4double worldSize = 10.*m;
    auto worldSol = new G4Box("World", 0.5*worldSize, 0.5*worldSize, 0.5*worldSize);
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

    // Spent Fuel Assembly
    auto spentFuelAssembly = new SpentFuelAssembly("SpentFuelAssembly", nistAir);
    spentFuelAssembly->SetFuelRodStatus(1.);
    spentFuelAssembly->PrintFuelRodStatus(G4cout);
    G4double spentFuelAssemblySurfaceDistance = 20.*cm;
//    G4double spentFuelAssemblyLength =
//            2*static_cast<G4Box*>(spentFuelAssembly->GetLogicalVolume()->GetSolid())->GetYHalfLength();
//    new G4PVPlacement(G4Transform3D(G4RotationMatrix().rotateX(90.*deg),
//                                    G4ThreeVector(0., 0., spentFuelAssemblySurfaceDistance + spentFuelAssemblyLength/2.)),
//                      spentFuelAssembly->GetLogicalVolume(), "SpentFuelAssembly", worldLV, false, 0);
    G4double spentFuelAssemblyHeight =
            2*static_cast<G4Box*>(spentFuelAssembly->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    new G4PVPlacement(G4Transform3D(G4RotationMatrix(),
                                    G4ThreeVector(0., 0., spentFuelAssemblySurfaceDistance + spentFuelAssemblyHeight/2.)),
                      spentFuelAssembly->GetLogicalVolume(), "SpentFuelAssembly", worldLV, false, 0);

    return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
    auto sd_Det = new CCSensitiveDetector("LACC");
    G4SDManager::GetSDMpointer()->AddNewDetector(sd_Det);
    SetSensitiveDetector(fLACC->GetScatter()->GetCrystalLV(), sd_Det);
    SetSensitiveDetector(fLACC->GetAbsorber()->GetCrystalLV(), sd_Det);
}
