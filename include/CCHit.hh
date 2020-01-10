#ifndef CCHIT_HH
#define CCHIT_HH

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class CCHit: public G4VHit
{
public:
    inline void* operator new(size_t);
    inline void operator delete(void* aHit);

    CCHit();
    CCHit(G4double eDep, G4ThreeVector pos, G4double time);

    virtual ~CCHit() override;

    CCHit& operator +=(const CCHit& right)
    {
        this->AddDepEAndPosition(right.fEDep, right.fPos);
        return *this;
    }

    void SetDepE(G4double eDep) { fEDep = eDep; }
    G4double GetDepE() const { return fEDep; }
    void SetPosition(G4ThreeVector pos) { fPos = pos; }
    G4ThreeVector GetPosition() const { return fPos; }
    void SetTime(G4double time) { fTime = time; }
    G4double GetTime() const { return fTime; }

    void AddDepEAndPosition(G4double eDep, G4ThreeVector pos)
    {
        fPos = (fPos*fEDep + pos*eDep)/(fEDep + eDep);
        fEDep += eDep;
    }

private:
    G4double fEDep;
    G4ThreeVector fPos;
    G4double fTime;
};

extern G4ThreadLocal G4Allocator<CCHit>* CCHitAllocator;

inline void* CCHit::operator new(size_t)
{
    if(!CCHitAllocator)
        CCHitAllocator = new G4Allocator<CCHit>;
    return static_cast<void*>(CCHitAllocator->MallocSingle());
}

inline void CCHit::operator delete(void* aHit)
{
    CCHitAllocator->FreeSingle(static_cast<CCHit*>(aHit));
}

#include "G4THitsMap.hh"
using CCHitsMap = G4THitsMap<CCHit>;

#endif // CCHIT_HH
