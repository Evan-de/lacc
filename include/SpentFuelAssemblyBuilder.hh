#ifndef SPENTFUELASSEMBLYBUILDER_HH
#define SPENTFUELASSEMBLYBUILDER_HH

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4UImanager.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class FuelRod;
class SpentFuelAssemblyParameterisation;

class SpentFuelAssembly
{
public:
    SpentFuelAssembly(G4String name,
                      G4Material* surrMat,
                      G4int nx = 16,
                      G4int ny = 16,
                      G4double interval = 1.285*cm);

    void SetName(G4String name) { fName = name; }
    G4String GetName() const { return fName; }
    G4LogicalVolume* GetLogicalVolume() const { return fSpentFuelAssemblyLV; }
    G4int GetNX() const { return fNX; }
    G4int GetNY() const { return fNY; }
    void Print() { G4UImanager::GetUIpointer()->ApplyCommand("/vis/drawTree " + fName); }

    std::shared_ptr<FuelRod> GetFuelRod() const { return std::move(fFuelRod); }
    G4ThreeVector GetFuelRodLocation(const G4int i) const;
    void SetFuelRodStatus(G4double ratio);
    void SetFuelRodStatus(std::vector<G4int> fuelRodIDVec) { fFuelRodIDVec = fuelRodIDVec; }
    void PrintFuelRodStatus(std::ostream& out) const;
    G4int SampleRandomFuelRodID() const;

private:
    G4String fName;

    G4int fNX, fNY;
    G4double fInterval;
    std::vector<G4int> fFuelRodIDVec;

    std::shared_ptr<FuelRod> fFuelRod;

    G4LogicalVolume* fSpentFuelAssemblyLV;
    SpentFuelAssemblyParameterisation* fSpentFuelAssemblyParam;
};

class SpentFuelAssemblyStore: public std::vector< std::shared_ptr<SpentFuelAssembly> >
{
public:
    static auto GetInstance()
    {
        static SpentFuelAssemblyStore* fInstance = new SpentFuelAssemblyStore;
        return fInstance;
    }
    static void Register(std::shared_ptr<SpentFuelAssembly> pSpentFuelAssembly)
    { GetInstance()->push_back(pSpentFuelAssembly); }

    std::shared_ptr<SpentFuelAssembly> GetSpentFuelAssembly(const G4String& name) const;

    ~SpentFuelAssemblyStore() { GetInstance()->clear(); }

private:
    SpentFuelAssemblyStore(): std::vector< std::shared_ptr<SpentFuelAssembly> >() {}
};

class FuelRod
{
public:
    FuelRod();

    G4LogicalVolume* GetLogicalVolume() const { return fCladdingLV; }
    G4ThreeVector SampleRandomPointInFuelRod() const;

private:
    void DefineMaterials();
    static G4bool fDefineMaterialsFlag;

    G4LogicalVolume* fCladdingLV;
    G4LogicalVolume* fFuelPelletLV;
};

#endif // SPENTFUELASSEMBLYBUILDER_HH
