#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Camouflage retain-chance policy from CBattleEntity::OnRangedAttack
// after RemoveAmmo (RETAIN_CAMOUFLAGE > 0 branch). Host injects behind/beside
// geometry, distances, and RNG.

namespace camouflageretainhelpers
{

// Base retain chance when RETAIN_CAMOUFLAGE is up (non-Barrage default).
constexpr int16 BaseRetainChance = 40;

// Facing cone allowance for behind/beside tests (rotAllowance = 25).
constexpr uint8 RotAllowance = 25;

// Distance band offsets from melee range (float thresholds).
constexpr float BehindFullOffset   = 0.6f; // > melee + .6 → 100%
constexpr float BehindPartialOffset = 0.1f; // > melee + .1 → base + 1.6*dist
constexpr float BesideFullOffset   = 5.0f;
constexpr float BesidePartialOffset = 3.3f;
constexpr float FrontFullOffset    = 8.1f;
constexpr float FrontPartialOffset = 7.1f;

// Distance bonus coefficient: retainChance += 1.6 * distanceToTarget
constexpr float DistanceBonusCoeff = 1.6f;

// Relative facing zone for pure retain math.
enum class CamouflageFacingZone : uint8
{
    Front  = 0,
    Beside = 1,
    Behind = 2,
};

// ShouldEvaluateCamouflageRetain mirrors RETAIN_CAMOUFLAGE > 0.
inline auto ShouldEvaluateCamouflageRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod > 0;
}

// ResolveCamouflageFacingZone prefers behind over beside over front
// (matches if behind / else if beside / else).
inline auto ResolveCamouflageFacingZone(const bool isBehind, const bool isBeside) -> CamouflageFacingZone
{
    if (isBehind)
    {
        return CamouflageFacingZone::Behind;
    }
    if (isBeside)
    {
        return CamouflageFacingZone::Beside;
    }
    return CamouflageFacingZone::Front;
}

// ResolveCamouflageRetainChance mirrors the pure retain chance ladder.
// isBarrage forces 0. Otherwise zone + distance bands vs meleeRange.
// Partial bands use integer trunc toward zero of (base + 1.6f * distance)
// after float add, matching C++ int16 assignment from float expression.
inline auto ResolveCamouflageRetainChance(
    const bool isBarrage,
    const CamouflageFacingZone zone,
    const float distanceToTarget,
    const float meleeRange) -> int16
{
    if (isBarrage)
    {
        return 0;
    }

    float fullOffset    = FrontFullOffset;
    float partialOffset = FrontPartialOffset;
    switch (zone)
    {
        case CamouflageFacingZone::Behind:
            fullOffset    = BehindFullOffset;
            partialOffset = BehindPartialOffset;
            break;
        case CamouflageFacingZone::Beside:
            fullOffset    = BesideFullOffset;
            partialOffset = BesidePartialOffset;
            break;
        case CamouflageFacingZone::Front:
        default:
            break;
    }

    if (distanceToTarget > meleeRange + fullOffset)
    {
        return 100;
    }
    if (distanceToTarget > meleeRange + partialOffset)
    {
        // int16(base + 1.6f * distance) — float then trunc toward zero.
        return static_cast<int16>(static_cast<float>(BaseRetainChance) + DistanceBonusCoeff * distanceToTarget);
    }
    return 0;
}

// ShouldStripAllDetectableOnFail mirrors roll > retainChance (delete Detectable flag).
// Host injects roll in [0, 100).
inline auto ShouldStripAllDetectableOnFail(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

// When retain succeeds (not strip all), host deletes Sneak/Deodorize/Illusion only.
inline auto ShouldStripPartialStealthOnRetain(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// ShouldStripAllDetectableWithoutRetain mirrors else branch when RETAIN_CAMOUFLAGE <= 0.
inline auto ShouldStripAllDetectableWithoutRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod <= 0;
}

} // namespace camouflageretainhelpers
