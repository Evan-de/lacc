#include "LACCBuilder.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

G4bool LAScintDet::fDefineMaterialsFlag = false;

LACC::LACC(G4String name, G4double scThk, G4double abThk, G4double sc2abDist)
: fName(name), fScatterThickness(scThk), fAbsorberThickness(abThk)
{
    // Geometry tree view
    // - LACC
    // | - Scatter
    // | - Absorber

    fScatter = std::make_shared<LAScintDet>(fScatterThickness);
    fAbsorber = std::make_shared<LAScintDet>(fAbsorberThickness);

    G4double scatterHeight = 2*static_cast<G4Box*>(fScatter->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    G4double absorberHeight = 2*static_cast<G4Box*>(fAbsorber->GetLogicalVolume()->GetSolid())->GetZHalfLength();
    G4double laccWidth = 2*static_cast<G4Box*>(fScatter->GetLogicalVolume()->GetSolid())->GetXHalfLength();
    fScatter2AbsorberDistance = (sc2abDist>scatterHeight)? sc2abDist: scatterHeight;
    G4double laccHeight = fScatter2AbsorberDistance + absorberHeight;
    auto laccSol = new G4Box(fName, laccWidth/2., laccWidth/2., laccHeight/2.);
    auto nistAir = G4Material::GetMaterial("G4_AIR");
    fLACCLV = new G4LogicalVolume(laccSol, nistAir, fName);

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., laccHeight/2. - scatterHeight/2.),
                      fScatter->GetLogicalVolume(), "Scatter", fLACCLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - laccHeight/2. + absorberHeight/2.),
                      fAbsorber->GetLogicalVolume(), "Absorber", fLACCLV, false, 1);
}

LAScintDet::LAScintDet(G4double crystalThickness)
: fCrystalThickness(crystalThickness)
{
    if(!fDefineMaterialsFlag) DefineMaterials();

    // Geometry tree view
    // - LAScintDetector
    // | - FrontHousing
    // | - RearHousing

    auto frontHousingLV = ConstructFrontHousing();
    auto rearHousingLV = ConstructRearHousing();

    G4double frontHousingHeight = 2*static_cast<G4Box*>(frontHousingLV->GetSolid())->GetZHalfLength();
    G4double rearHousingHeight = 2*static_cast<G4Box*>(rearHousingLV->GetSolid())->GetZHalfLength();
    G4double detectorWidth = 2*static_cast<G4Box*>(frontHousingLV->GetSolid())->GetXHalfLength();
    G4double detectorHeight = frontHousingHeight + rearHousingHeight;
    auto detectorSol = new G4Box("LAScintDetector", detectorWidth/2., detectorWidth/2., detectorHeight/2.);
    auto nistAir = G4Material::GetMaterial("G4_AIR");
    fDetectorLV = new G4LogicalVolume(detectorSol, nistAir, "LAScintDetector");

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., detectorHeight/2. - frontHousingHeight/2.),
                      frontHousingLV, "FrontHousing", fDetectorLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - detectorHeight/2. + rearHousingHeight/2.),
                      rearHousingLV, "RearHousing", fDetectorLV, false, 0);
}

G4LogicalVolume* LAScintDet::ConstructFrontHousing()
{
    // Geometry tree view
    // - FrontHousing
    // | - 1.5 mm Aluminum (FrontHousing itself)
    // | - PaintFront
    // | - PaintSide
    // | | - Crystal
    // | - OpticalGlue
    // | - OpticalWindow
    // | - OpticalGrease

    // Fillet setting
    auto filletRotMat = new G4RotationMatrix();
    filletRotMat->rotateZ(45.*deg);

    // Crystal
    G4double crystalWidth = 270.*mm, crystalFilletWidth = 353.6*mm;
    auto crystalMainSol = new G4Box("CrystalMain", crystalWidth/2., crystalWidth/2., fCrystalThickness/2.);
    auto crystalFilletSol = new G4Box("CrystalFillet", crystalFilletWidth/2., crystalFilletWidth/2., fCrystalThickness/2.);
    auto crystalSol = new G4IntersectionSolid("Crystal", crystalMainSol, crystalFilletSol, filletRotMat, G4ThreeVector());
    auto NaITl = G4Material::GetMaterial("NaITl");
    fCrystalLV = new G4LogicalVolume(crystalSol, NaITl, "Crystal");
    auto crystalVA = new G4VisAttributes(G4Colour::White());
    crystalVA->SetForceSolid();
    fCrystalLV->SetVisAttributes(crystalVA);

    // Paint (For different type reflector on side & front, two geoms were divided.)
    G4double paintThickness = 2.5*mm;
    auto paintFilletSol = new G4Box("PaintFillet",
                                    crystalFilletWidth/2. + paintThickness,
                                    crystalFilletWidth/2. + paintThickness,
                                    (fCrystalThickness + paintThickness)/2);
    auto paintSideMainSol = new G4Box("PaintSideMain",
                                  crystalWidth/2. + paintThickness,
                                  crystalWidth/2. + paintThickness,
                                  fCrystalThickness/2.);
    auto paintSideSol = new G4IntersectionSolid("paintSide", paintSideMainSol, crystalFilletSol, filletRotMat, G4ThreeVector());
    auto nistMgO = G4Material::GetMaterial("G4_MAGNESIUM_OXIDE");
    auto paintSideLV = new G4LogicalVolume(paintSideSol, nistMgO, "PaintSide");

    auto paintFrontMainSol = new G4Box("PaintFrontMain",
                                   crystalWidth/2. + paintThickness,
                                   crystalWidth/2. + paintThickness,
                                   paintThickness/2.);
    auto paintFrontSol = new G4IntersectionSolid("PaintFront", paintFrontMainSol, paintFilletSol, filletRotMat, G4ThreeVector());
    auto paintFrontLV = new G4LogicalVolume(paintFrontSol, nistMgO, "PaintFront");

    // Optical Glue
    G4double opticalGlueThickness = 0.5*mm;
    auto opticalGlueMainSol = new G4Box("OpticalGlueMain",
                                        crystalWidth/2. + paintThickness,
                                        crystalWidth/2. + paintThickness,
                                        opticalGlueThickness/2.);
    auto opticalGlueFilletSol = new G4Box("OpticalGlueFillet",
                                          crystalFilletWidth/2. + paintThickness,
                                          crystalFilletWidth/2. + paintThickness,
                                          opticalGlueThickness/2);
    auto opticalGlueSol = new G4IntersectionSolid("OpticalGlue", opticalGlueMainSol, opticalGlueFilletSol, filletRotMat, G4ThreeVector());
    auto BC630 = G4Material::GetMaterial("BC630");
    auto opticalGlueLV = new G4LogicalVolume(opticalGlueSol, BC630, "OpticalGlue");

    // Optical Window
    G4double opticalWindowWidth = 307.*mm, opticalWindowThickness = 10.*mm;
    auto opticalWindowSol = new G4Box("OpticalWindow", opticalWindowWidth/2., opticalWindowWidth/2., opticalWindowThickness/2.);
    auto fusedSilica = G4Material::GetMaterial("FusedSilica");
    auto opticalWindowLV = new G4LogicalVolume(opticalWindowSol, fusedSilica, "OpticalWindow");
    auto opticalWindowVA = new G4VisAttributes(G4Colour::Cyan());
    opticalWindowVA->SetForceSolid();
    opticalWindowLV->SetVisAttributes(opticalWindowVA);

    // Optical Grease
    G4double opticalGreaseThickness = 1.*mm;
    auto opticalGreaseSol = new G4Box("OpticalGrease", opticalWindowWidth/2., opticalWindowWidth/2., opticalGreaseThickness/2.);
    auto opticalGreaseLV = new G4LogicalVolume(opticalGreaseSol, BC630, "OpticalGrease");

    // Front Housing
    G4double frontHousingWidth = 400.*mm;
    G4double frontHousingAluminumThickness = 1.5*mm;
    G4double frontHousingHeight =
            frontHousingAluminumThickness +
            paintThickness +
            fCrystalThickness +
            opticalGlueThickness +
            opticalWindowThickness +
            opticalGreaseThickness;
    auto frontHousingSol = new G4Box("FrontHousing", frontHousingWidth/2., frontHousingWidth/2., frontHousingHeight/2);
    auto nistAl = G4Material::GetMaterial("G4_Al");
    auto frontHousingLV = new G4LogicalVolume(frontHousingSol, nistAl, "FrontHousing");

    // Construction
    new G4PVPlacement(nullptr, G4ThreeVector(), fCrystalLV, "Crystal", paintSideLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., frontHousingHeight/2. - frontHousingAluminumThickness - paintThickness/2.),
                      paintFrontLV, "Crystal", frontHousingLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., frontHousingHeight/2. - frontHousingAluminumThickness - paintThickness - fCrystalThickness/2.),
                      paintSideLV, "Crystal", frontHousingLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - frontHousingHeight/2. + opticalGreaseThickness + opticalWindowThickness + opticalGlueThickness/2.),
                      opticalGlueLV, "Crystal", frontHousingLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - frontHousingHeight/2. + opticalGreaseThickness + opticalWindowThickness/2.),
                      opticalWindowLV, "Crystal", frontHousingLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - frontHousingHeight/2. + opticalGreaseThickness/2.),
                      opticalGreaseLV, "Crystal", frontHousingLV, false, 0);

    // Return
    return frontHousingLV;
}

G4LogicalVolume* LAScintDet::ConstructRearHousing()
{
    // - RearHousing
    // | - 1.5 mm Aluminum (RearHousing itself)
    // | - Inner Void
    // | | - PMT (36 ea. 6x6)
    // | | | - PMT Front Base (1.5 mm thick Pyrex Glass (BSiO2))
    // | | | | - PMT Front Void
    // | | | | | - PMT PhotoCathode
    // | | | - PMT Rear Base
    // | | | | - PMT Rear Void
    // | | | - PMT Tail 1
    // | | | - PMT PCB
    // | | | - PMT Tail 2
    // | | - Pillar (12 ea. 6x2)
    // | | - Holder (6 ea. 6x1)

    // PMT PhotoCathode
    G4double pmtPhotoCathodeWidth = 48.*mm, pmtPhotoCathodeThickness = 0.5*mm;
    auto pmtPhotoCathodeSol = new G4Box("PMTPhotoCathode", pmtPhotoCathodeWidth/2., pmtPhotoCathodeWidth/2., pmtPhotoCathodeThickness/2.);
    auto bialkaliCathode = G4Material::GetMaterial("BialkaliCathode");
    fPMTPhotoCathodeLV = new G4LogicalVolume(pmtPhotoCathodeSol, bialkaliCathode, "PMTPhotoCathode");
    auto pmtPhotoCathodeVA = new G4VisAttributes(G4Colour::Brown());
    pmtPhotoCathodeVA->SetForceSolid();
    fPMTPhotoCathodeLV->SetVisAttributes(pmtPhotoCathodeVA);

    // PMT Front Base (& Void)
    G4double pmtGlassThickness = 1.5*mm;
    G4double pmtFrontBaseWidth = 51.*mm, pmtFrontBaseHeight = 50.*mm;
    auto pmtFrontVoidSol = new G4Box("PMTFrontVoid",
                                     pmtFrontBaseWidth/2. - pmtGlassThickness,
                                     pmtFrontBaseWidth/2. - pmtGlassThickness,
                                     (pmtFrontBaseHeight - pmtGlassThickness)/2.);
    auto nistAir = G4Material::GetMaterial("G4_AIR");
    G4LogicalVolume* pmtFrontVoidLV = new G4LogicalVolume(pmtFrontVoidSol, nistAir, "PMTFrontVoid");
    auto voidVA = new G4VisAttributes(G4VisAttributes::Invisible);
    pmtFrontVoidLV->SetVisAttributes(voidVA);

    auto pmtFrontBaseSol = new G4Box("PMTFrontBase", pmtFrontBaseWidth/2, pmtFrontBaseWidth/2, pmtFrontBaseHeight/2);
    auto nistPyrexGlass = G4Material::GetMaterial("G4_Pyrex_Glass");
    auto pmtFrontBaseLV = new G4LogicalVolume(pmtFrontBaseSol, nistPyrexGlass, "PMTFrontBase");
    auto glassVA = new G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.4));
    glassVA->SetForceSolid();
    pmtFrontBaseLV->SetVisAttributes(glassVA);

    // PMT Rear Base (& Void)
    G4double pmtRearBaseDiameter = pmtFrontBaseWidth, pmtRearBaseHeight = 58.*mm;
    auto pmtRearVoidSol = new G4Tubs("PMTRearVoid", 0., pmtRearBaseDiameter/2. - pmtGlassThickness,
                                     (pmtRearBaseHeight - pmtGlassThickness)/2.,
                                     0., 360.*deg);
    auto pmtRearVoidLV = new G4LogicalVolume(pmtRearVoidSol, nistAir, "PMTRearVoid");
    pmtRearVoidLV->SetVisAttributes(voidVA);

    auto pmtRearBaseSol = new G4Tubs("PMTRearBase", 0., pmtRearBaseDiameter/2., pmtRearBaseHeight/2., 0., 360.*deg);
    auto pmtRearBaseLV = new G4LogicalVolume(pmtRearBaseSol, nistPyrexGlass, "PMTRearBase");
    pmtRearBaseLV->SetVisAttributes(glassVA);

    // PMT Tail
    G4double pmtTailDiameter = 26.*mm, pmtTailHeight = 18.5*mm;
    auto pmtTailSol = new G4Tubs("PMTTail", 0., pmtTailDiameter/2., pmtTailHeight/2., 0., 360.*deg);
    auto nistPolyEthylene = G4Material::GetMaterial("G4_POLYETHYLENE");
    auto pmtTailLV = new G4LogicalVolume(pmtTailSol, nistPolyEthylene, "PMTTail");
    auto pmtTailVA = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
    pmtTailVA->SetForceSolid();
    pmtTailLV->SetVisAttributes(pmtTailVA);

    // PMT PCB
    G4double pmtPCBDiameter = pmtRearBaseDiameter, pmtPCBThickness = 1.5*mm;
    auto pmtPCBSol = new G4Tubs("PMTPCB", 0., pmtPCBDiameter/2., pmtPCBThickness/2., 0., 360.*deg);
    auto pmtPCBLV = new G4LogicalVolume(pmtPCBSol, nistPolyEthylene, "PMTPCB");
    auto pmtPCBVA = new G4VisAttributes(G4Colour(0.15, 0.4, 0.15));
    pmtPCBVA->SetForceSolid();
    pmtPCBLV->SetVisAttributes(pmtPCBVA);

    // PMT Whole
    G4double pmtWholeWidth = pmtFrontBaseWidth;
    G4double pmtWholeHeight = pmtFrontBaseHeight + pmtRearBaseHeight + pmtTailHeight + pmtPCBThickness + pmtTailHeight;
    G4double pmt2pmtDistance = 52.2*mm;
    auto pmtSol = new G4Box("PMT", pmtWholeWidth/2., pmtWholeWidth/2., pmtWholeHeight/2.);
    auto pmtLV = new G4LogicalVolume(pmtSol, nistAir, "PMT");
    pmtLV->SetVisAttributes(voidVA);

    // Pillar
    G4double pillarDiameter = 9.5*mm, pillarHeight = pmtWholeHeight;
    G4double pillar2pillarDistance = 5*pmt2pmtDistance + pmtWholeWidth + 2*6.1*mm + pillarDiameter;
    auto pillarSol = new G4Tubs("Pillar", 0., pillarDiameter/2, pillarHeight/2., 0., 360.*deg);
    auto nistAl = G4Material::GetMaterial("G4_Al");
    auto pillarLV = new G4LogicalVolume(pillarSol, nistAl, "Pillar");
    auto aluminumVA = new G4VisAttributes(G4Colour::Gray());
    aluminumVA->SetForceSolid();
    pillarLV->SetVisAttributes(aluminumVA);

    // Holder
    G4double holderWidth = 350.*mm;
    G4double holderLength = 20.0*mm, holderHeight = 6.5*mm;
    auto holderSol = new G4Box("Holder", holderWidth/2., holderLength/2., holderHeight/2.);
    auto holderLV = new G4LogicalVolume(holderSol, nistAl, "Holder");
    holderLV->SetVisAttributes(aluminumVA);

    // Rear Housing
    G4double rearHousingWidth = 355.*mm, rearHousingHeight = 200.*mm;
    G4double rearHousingThickness = 1.5*mm;
    auto rearInnerVoidSol = new G4Box("RearInnerVoid", rearHousingWidth/2. - rearHousingThickness,
                                  rearHousingWidth/2. - rearHousingThickness,
                                  (rearHousingHeight - rearHousingThickness)/2.);
    auto rearInnerVoidLV = new G4LogicalVolume(rearInnerVoidSol, nistAir, "RearInnerVoid");
    auto rearHousingSol = new G4Box("RearHousing", rearHousingWidth/2., rearHousingWidth/2., rearHousingHeight/2.);
    auto rearHousingLV = new G4LogicalVolume(rearHousingSol, nistAl, "RearHousing");

    // Construction
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., (pmtFrontBaseHeight - pmtGlassThickness)/2. - pmtPhotoCathodeThickness/2.),
                      fPMTPhotoCathodeLV, "PMTPhotoCathode", pmtFrontVoidLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - pmtGlassThickness/2.),
                      pmtFrontVoidLV, "PMTFrontVoid", pmtFrontBaseLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., pmtGlassThickness/2.),
                      pmtRearVoidLV, "PMTRearVoid", pmtRearBaseLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., pmtWholeHeight/2. - pmtFrontBaseHeight/2.),
                      pmtFrontBaseLV, "PMTFrontBase", pmtLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., pmtWholeHeight/2. - pmtFrontBaseHeight - pmtRearBaseHeight/2.),
                      pmtRearBaseLV, "PMTRearBase", pmtLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - pmtWholeHeight/2. + pmtTailHeight + pmtPCBThickness + pmtTailHeight/2.),
                      pmtTailLV, "PMTTail1", pmtLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - pmtWholeHeight/2. + pmtTailHeight + pmtPCBThickness/2.),
                      pmtPCBLV, "PMTPCB", pmtLV, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., - pmtWholeHeight/2. + pmtTailHeight/2.),
                      pmtTailLV, "PMTTail2", pmtLV, false, 0);
    G4int nRows = 6;
    G4int nCols = 6;
    std::vector<G4double> rowPosVec;
    for(G4int i = 0; i<nRows; ++i)
    {
        G4double rowPos = - pmt2pmtDistance*i + static_cast<G4double>(nRows - 1)/2.*pmt2pmtDistance;
        rowPosVec.push_back(rowPos);
        for(G4int j = 0; j<nCols; ++j)
        {
            G4double colPos = pmt2pmtDistance*j - static_cast<G4double>(nCols - 1)/2.*pmt2pmtDistance;

            new G4PVPlacement(nullptr, G4ThreeVector(colPos, rowPos, (rearHousingHeight - rearHousingThickness)/2. - pmtWholeHeight/2.),
                              pmtLV, "PMT", rearInnerVoidLV, false, 100 + j + i*nCols);
        }
    }
    for(size_t i = 0; i<rowPosVec.size(); ++i)
    {
        new G4PVPlacement(nullptr, G4ThreeVector(- pillar2pillarDistance/2., rowPosVec[i], (rearHousingHeight - rearHousingThickness)/2. - pmtWholeHeight/2.),
                          pillarLV, "Pillar", rearInnerVoidLV, false, 0);
        new G4PVPlacement(nullptr, G4ThreeVector(pillar2pillarDistance/2., rowPosVec[i], (rearHousingHeight - rearHousingThickness)/2. - pmtWholeHeight/2.),
                          pillarLV, "Pillar", rearInnerVoidLV, false, 0);
        new G4PVPlacement(nullptr, G4ThreeVector(0., rowPosVec[i], (rearHousingHeight - rearHousingThickness)/2. - pmtWholeHeight - holderHeight/2.),
                          holderLV, "Holder", rearInnerVoidLV, false, 0);
    }
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., rearHousingThickness/2.), rearInnerVoidLV, "PMTFrontVoid", rearHousingLV, false, 0);

    // Return
    return rearHousingLV;
}

void LAScintDet::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* nistElH = nist->FindOrBuildElement("H");
    G4Element* nistElC = nist->FindOrBuildElement("C");
    G4Element* nistElO = nist->FindOrBuildElement("O");
    G4Element* nistElSi = nist->FindOrBuildElement("Si");
    G4Element* nistElTl = nist->FindOrBuildElement("Tl");
    G4Element* nistElCs = nist->FindOrBuildElement("Cs");
    G4Element* nistElSb = nist->FindOrBuildElement("Sb");
    G4Element* nistElRb = nist->FindOrBuildElement("Rb");

    nist->FindOrBuildMaterial("G4_Galactic");
    nist->FindOrBuildMaterial("G4_AIR");
    nist->FindOrBuildMaterial("G4_Al");
    nist->FindOrBuildMaterial("G4_Cu");
    nist->FindOrBuildMaterial("G4_W");
    nist->FindOrBuildMaterial("G4_MAGNESIUM_OXIDE"); // paint
    nist->FindOrBuildMaterial("G4_Pyrex_Glass"); // PMT front glass
    nist->FindOrBuildMaterial("G4_POLYETHYLENE"); // PMT tail

    G4Material* NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    G4Material* NaITl = new G4Material("NaITl", 3.67*g/cm3, 2, kStateSolid); // crystal
    NaITl->AddMaterial(NaI, 99.6*perCent);
    NaITl->AddElement(nistElTl, 0.4*perCent);

    G4Material* Polydimethylsiloxane = new G4Material("Polydimethylsiloxane", 0.97*g/cm3, 4, kStateSolid); // BC630 material
    Polydimethylsiloxane->AddElement(nistElSi, 1);
    Polydimethylsiloxane->AddElement(nistElO, 1);
    Polydimethylsiloxane->AddElement(nistElC, 2);
    Polydimethylsiloxane->AddElement(nistElH, 6);

    G4Material* FusedSilica = new G4Material("FusedSilica", 2.201*g/cm3, 2, kStateSolid); // Optical window
    FusedSilica->AddElement(nistElSi, 1);
    FusedSilica->AddElement(nistElO, 2);

    G4Material* BC630 = new G4Material("BC630", 1.06*g/cm3, 2, kStateLiquid); // Optical grease
    BC630->AddMaterial(Polydimethylsiloxane, 0.95);
    BC630->AddMaterial(FusedSilica, 0.05);

    G4Material* BialkaliCathode = new G4Material("BialkaliCathode", 3.*g/cm3, 3, kStateSolid);
    BialkaliCathode->AddElement(nistElSb, 1);
    BialkaliCathode->AddElement(nistElRb, 1);
    BialkaliCathode->AddElement(nistElCs, 1);

    fDefineMaterialsFlag = true;
}
