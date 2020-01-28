#ifndef PRIMARYSAMPLINGTOOLS_HH
#define PRIMARYSAMPLINGTOOLS_HH

#include "G4ThreeVector.hh"
#include "G4RandomDirection.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"

G4ThreeVector SampleDirectionFromTo(const G4ThreeVector& referencePoint,
                                    const G4String& physicalVolumeName,
                                    G4double& particleWeight,
                                    G4double margin = 0.)
{
    // No physical volume setting
    if(!physicalVolumeName.size()) return G4RandomDirection();

    // Invalid physical volume name
    auto physicalVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(physicalVolumeName);
    if(!physicalVolume)
    {
        G4Exception("RandomDirectionFromTo()", "", JustWarning,
                    G4String("      invalid physical volume '" + physicalVolumeName + "'" ).c_str());
        return G4RandomDirection();
    }

    // Get the envelope box of the physical volume
    G4ThreeVector pvPosition, pvMin, pvMax;
    G4RotationMatrix pvRotation;
    pvPosition = physicalVolume->GetObjectTranslation();
    pvRotation = physicalVolume->GetObjectRotationValue();
    physicalVolume->GetLogicalVolume()->GetSolid()->BoundingLimits(pvMin, pvMax);
    pvMin.rotate(pvRotation.axisAngle());
    pvMax.rotate(pvRotation.axisAngle());
    // Add margin to the box
    if(margin>0.)
    {
        pvMin = pvMin - G4ThreeVector(margin, margin, margin);
        pvMax = pvMax + G4ThreeVector(margin, margin, margin);
    }

    // Calculate the direction to the box
    G4ThreeVector targetVector = pvPosition - referencePoint;
    G4ThreeVectorList pvBoxVerticies =
    {
        G4ThreeVector(pvMin.x(), pvMin.y(), pvMin.z()) + targetVector,
        G4ThreeVector(pvMin.x(), pvMin.y(), pvMax.z()) + targetVector,
        G4ThreeVector(pvMin.x(), pvMax.y(), pvMin.z()) + targetVector,
        G4ThreeVector(pvMin.x(), pvMax.y(), pvMax.z()) + targetVector,
        G4ThreeVector(pvMax.x(), pvMin.y(), pvMin.z()) + targetVector,
        G4ThreeVector(pvMax.x(), pvMin.y(), pvMax.z()) + targetVector,
        G4ThreeVector(pvMax.x(), pvMax.y(), pvMin.z()) + targetVector,
        G4ThreeVector(pvMax.x(), pvMax.y(), pvMax.z()) + targetVector
    };
    G4double maxTheta = 0.;
    for(const auto& vertex: pvBoxVerticies)
    {
        G4double target2VertexAngle = targetVector.angle(vertex);
        maxTheta = (target2VertexAngle>maxTheta) ? target2VertexAngle : maxTheta;
    }

    // If the ref point is in the bounding box of the physical volume, return 4pi dir.
    G4double cosTheta = std::cos(maxTheta);
    if(cosTheta<=0.) return G4RandomDirection();

    // Multiply particle weight as its solid angle reduction
    G4double solidAngle = CLHEP::twopi * (1 - cosTheta);
    particleWeight *= solidAngle / (4 * CLHEP::pi);

    // Sample a direction and rotate to targetVector
    G4ThreeVector dirVec = G4RandomDirection(cosTheta);
    G4ThreeVector zUnit = G4ThreeVector(0., 0., 1);
    if(!(targetVector.isParallel(zUnit)))
        dirVec.rotate(zUnit.cross(targetVector), zUnit.angle(targetVector));
    else
        dirVec *= targetVector.unit().dot(zUnit);

    return dirVec;
}

#endif // PRIMARYSAMPLINGTOOLS_HH
