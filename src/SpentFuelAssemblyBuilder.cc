#include "SpentFuelAssemblyBuilder.hh"
#include "SpentFuelAssemblyParameterisation.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4VisAttributes.hh"
#include "G4RandomTools.hh"

G4bool FuelRod::fDefineMaterialsFlag = false;

SpentFuelAssembly::SpentFuelAssembly(G4String name, G4Material* surrMat, G4int nx, G4int ny, G4double interval)
: fName(name), fNX(nx), fNY(ny), fInterval(interval)
{
    // Geometry tree view
    // - SpentFuelAssembly
    // | - FuelRod (fNXxfNY)

    fFuelRod = std::make_shared<FuelRod>();
    G4double fuelRodRadius = static_cast<G4Tubs*>(fFuelRod->GetLogicalVolume()->GetSolid())->GetRMax();
    G4double fuelRodHeight = 2*static_cast<G4Tubs*>(fFuelRod->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    auto spentFuelAssemblySol = new G4Box(fName,
                                          fInterval*(fNX - 1)/2. + fuelRodRadius,
                                          fInterval*(fNY - 1)/2. + fuelRodRadius,
                                          fuelRodHeight/2.);
    fSpentFuelAssemblyLV = new G4LogicalVolume(spentFuelAssemblySol, surrMat, fName);

    fSpentFuelAssemblyParam = new SpentFuelAssemblyParameterisation(fNX, fNY, fInterval);
    new G4PVParameterised("FuelRod", fFuelRod->GetLogicalVolume(), fSpentFuelAssemblyLV,
                                       kXAxis, fNX*fNY, fSpentFuelAssemblyParam);

    SpentFuelAssemblyStore::GetInstance()->Register(std::shared_ptr<SpentFuelAssembly>(this));
}

G4ThreeVector SpentFuelAssembly::GetFuelRodLocation(const G4int i) const
{
    if(i<0 || i>=fNX*fNY)
    {
        G4Exception("SpentFuelAssembly::GetFuelRodLocation()", "", JustWarning,
                    "    Input copy number is out of range.");
        return G4ThreeVector();
    }

    return fSpentFuelAssemblyParam->GetTranslation(i);
}

std::shared_ptr<SpentFuelAssembly> SpentFuelAssemblyStore::GetSpentFuelAssembly(const G4String& name) const
{
    auto pStore = GetInstance();
    for(const auto& pData: *pStore)
        if(pData->GetName()==name) return pData;

    G4Exception("SpentFuelAssemblyStore::GetSpentFuelAssembly()", "", JustWarning,
                G4String("    No spent fuel assembly exists named '" + name + "'.").c_str());
    return std::shared_ptr<SpentFuelAssembly>(nullptr);
}

FuelRod::FuelRod()
{
    if(!fDefineMaterialsFlag) DefineMaterials();

    // Geometry tree view
    // - Cladding
    // | - HeGap
    // | | - Fuel

    G4double claddingDiameter = 9.7*mm;
    G4double claddingHeight = 30.*mm;
    auto claddingSol = new G4Tubs("Cladding", 0., claddingDiameter/2., claddingHeight/2., 0., 360.*deg);
    auto zir4 = G4Material::GetMaterial("Zir4");
    fCladdingLV = new G4LogicalVolume(claddingSol, zir4, "Cladding");
    auto claddingVA = new G4VisAttributes(G4Colour::Cyan());
    fCladdingLV->SetVisAttributes(claddingVA);

    // He gas gap
    G4double HeGapOuterDiameter = 8.43*mm;
    G4double HeGapHeight = claddingHeight;
    auto HeGapSol = new G4Tubs("HeGap", 0., HeGapOuterDiameter/2., HeGapHeight/2., 0., 360.*deg);
    auto HeGas = G4Material::GetMaterial("HeGas");
    auto HeGapLV = new G4LogicalVolume(HeGapSol, HeGas, "HeGap");
    new G4PVPlacement(nullptr, G4ThreeVector(), HeGapLV, "HeGap", fCladdingLV, false, 0);

    // Fuel pellet of LEU
    G4double fuelPelletDiameter = 8.26*mm;
    G4double fuelPelletHeight = claddingHeight;
    auto fuelPelletSol = new G4Tubs("FuelPellet", 0., fuelPelletDiameter/2., fuelPelletHeight/2., 0., 360.*deg);
    auto LEU = G4Material::GetMaterial("mLEU");
    fFuelPelletLV = new G4LogicalVolume(fuelPelletSol, LEU, "FuelPellet");
    auto fuelPelletVA = new G4VisAttributes(G4Colour::Magenta());
    fFuelPelletLV->SetVisAttributes(fuelPelletVA);
    new G4PVPlacement(nullptr, G4ThreeVector(), fFuelPelletLV, "FuelPellet", HeGapLV, false, 0);
}

G4ThreeVector FuelRod::SampleRandomPointInFuelRod() const
{
    G4double fuelPelletDiameter = 2*static_cast<G4Tubs*>(fFuelPelletLV->GetSolid())->GetRMax();
    G4double fuelPelletHeight = 2*static_cast<G4Tubs*>(fFuelPelletLV->GetSolid())->GetZHalfLength();
    G4TwoVector randomXYPoint = G4RandomPointInEllipse(fuelPelletDiameter/2., fuelPelletDiameter/2.);
    G4double randomZPoint = G4UniformRand()*fuelPelletHeight - fuelPelletHeight/2.;

    return G4ThreeVector(randomXYPoint.x(), randomXYPoint.y(), randomZPoint);
}

void FuelRod::DefineMaterials()
{
    auto nist = G4NistManager::Instance();

    auto O = G4NistManager::Instance()->FindOrBuildMaterial("G4_O");
    auto C = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
    auto Co = G4NistManager::Instance()->FindOrBuildMaterial("G4_Co");
    auto Cr = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cr");
    auto Fe = G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
    auto Zr = G4NistManager::Instance()->FindOrBuildMaterial("G4_Zr");
    auto Sn = G4NistManager::Instance()->FindOrBuildMaterial("G4_Sn");
    auto He = G4NistManager::Instance()->FindOrBuildMaterial("G4_He");

    auto U5 = new G4Isotope("U235", 92, 235, 235.01*g/mole);
    auto U8 = new G4Isotope("U238", 92, 238, 238.03*g/mole);

    nist->FindOrBuildMaterial("G4_Cu");
    nist->FindOrBuildMaterial("G4_Pb");
    nist->FindOrBuildMaterial("G4_AIR");
    nist->FindOrBuildMaterial("G4_POLYPROPYLENE");
    nist->FindOrBuildMaterial("G4_WATER");

    // Enriched U
    auto LEU  = new G4Element("enriched Uranium", "LEU", 2);
    LEU->AddIsotope(U5, 4.2*perCent);
    LEU->AddIsotope(U8, 95.8*perCent);
    auto mLEU = new G4Material("mLEU", 10.52*g/cm3, 1);
    mLEU->AddElement(LEU, 1.0);

    // Natural U
    auto NU = new G4Element("natural Uranium", "NU", 2);
    NU->AddIsotope(U5, 0.74*perCent);
    NU->AddIsotope(U8, 99.26*perCent);
    auto mNU = new G4Material("mNU", 10.43*g/cm3, 1);
    mNU->AddElement(NU, 1.0);

    // Zircaloy-4
    auto Zir4 = new G4Material("Zir4", 6.56*g/cm3, 5);
    Zir4->AddMaterial(O, 0.001196);
    Zir4->AddMaterial(Cr, 0.000997);
    Zir4->AddMaterial(Fe, 0.001994);
    Zir4->AddMaterial(Zr, 0.981858);
    Zir4->AddMaterial(Sn, 0.01395);

    // He gas with 20 bar
    auto HeGas = new G4Material("HeGas", 1.842*mg/cm3, 1, kStateGas, CLHEP::STP_Temperature, 20.*bar);
    HeGas->AddMaterial(He, 1.0);

    // SUS
    auto SUS = new G4Material("SUS", 7.7*g/cm3, 3);
    SUS->AddMaterial(C, 0.04);
    SUS->AddMaterial(Fe, 0.88);
    SUS->AddMaterial(Co, 0.08);

    fDefineMaterialsFlag = true;
}
