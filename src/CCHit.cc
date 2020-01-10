#include "CCHit.hh"

G4ThreadLocal G4Allocator<CCHit>* CCHitAllocator;

CCHit::CCHit()
: G4VHit(), fEDep(0.), fPos(G4ThreeVector()), fTime(0.), fWeight(1.)
{}

CCHit::CCHit(G4double eDep, G4ThreeVector pos, G4double time, G4double weight)
: G4VHit(), fEDep(eDep), fPos(pos), fTime(time), fWeight(weight)
{}

CCHit::~CCHit()
{}
