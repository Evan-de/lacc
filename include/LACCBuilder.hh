#ifndef LACCBUILDER_HH
#define LACCBUILDER_HH

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4UImanager.hh"

class G4LogicalVolume;
class LAScintDet;

class LACC
{
public:
    LACC(G4String name,
         G4double scThk = 2.*cm,
         G4double abThk = 3.*cm,
         G4double sc2abDist = 25.*cm);

    void SetName(G4String name) { fName = name; }
    G4String GetName() const { return fName; }
    G4LogicalVolume* GetLogicalVolume() const { return fLACCLV; }
    std::shared_ptr<LAScintDet> GetScatter() const { return std::move(fScatter); }
    std::shared_ptr<LAScintDet> GetAbsorber() const { return std::move(fAbsorber); }
    void Print() { G4UImanager::GetUIpointer()->ApplyCommand("/vis/drawTree " + fName); }

private:
    G4String fName;

    G4double fScatterThickness, fAbsorberThickness;
    G4double fScatter2AbsorberDistance;

    std::shared_ptr<LAScintDet> fScatter;
    std::shared_ptr<LAScintDet> fAbsorber;
    G4LogicalVolume* fLACCLV;
};

class LACCStore: public std::vector< std::shared_ptr<LACC> >
{
public:
    static auto GetInstance()
    {
        static LACCStore* fInstance = new LACCStore;
        return fInstance;
    }
    static void Register(std::shared_ptr<LACC> pLACC)
    { GetInstance()->push_back(pLACC); }

    std::shared_ptr<LACC> GetLACC(const G4String& name) const;

    ~LACCStore() { GetInstance()->clear(); }

private:
    LACCStore(): std::vector< std::shared_ptr<LACC> >() {}
};

class LAScintDet
{
public:
    LAScintDet(G4double crystalThickness);

    G4LogicalVolume* GetLogicalVolume() const { return fDetectorLV; }
    G4LogicalVolume* GetCrystalLV() const { return fCrystalLV; }
    G4LogicalVolume* GetPMTPhotoCathodeLV() const { return fPMTPhotoCathodeLV; }

private:
    void DefineMaterials();
    static G4bool fDefineMaterialsFlag;

    G4LogicalVolume* ConstructFrontHousing();
    G4LogicalVolume* ConstructRearHousing();

    G4double fCrystalThickness;

    G4LogicalVolume* fDetectorLV;
    G4LogicalVolume* fCrystalLV;
    G4LogicalVolume* fPMTPhotoCathodeLV;
};

#endif // LACCBUILDER_HH
