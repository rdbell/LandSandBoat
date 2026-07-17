#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Camouflage retain-chance policy from CBattleEntity::OnRangedAttack
// after RemoveAmmo (RETAIN_CAMOUFLAGE > 0 branch). Host injects behind/beside
// geometry, distances, and RNG.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1391: residual pure port (full retain / strip / facing / chance suite)
//   - 3047: ShouldEvaluateCamouflageRetain (RETAIN_CAMOUFLAGE > 0 evaluate gate)
//   - 3058: ShouldStripAllDetectableOnFail (roll > retainChance strip-all gate)
//
// Production host:
//   - CBattleEntity::OnRangedAttack (~3472) injects getMod(Mod::RETAIN_CAMOUFLAGE)
//     into ShouldEvaluateCamouflageRetain before retain roll / strip logic.
//   - Same host injects retainChance + roll into ShouldStripAllDetectableOnFail
//     after the retain path opens.
//
// Go dual-wire:
//   - ranger.ShouldEvaluateCamouflageRetain (evaluate_camouflage_retain.go)
//   - ranger.ShouldStripAllDetectableOnFail (strip_detectable_fail.go)
// Residual Go surface: internal/ranger/camouflage_retain.go (facing / chance /
// partial-stealth / without-retain helpers).

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

// --- Slice 3047: ShouldEvaluateCamouflageRetain pure dual-wire ---
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// getMod(Mod::RETAIN_CAMOUFLAGE) into ShouldEvaluateCamouflageRetain before
// retain roll / strip logic (~3472). When false, host falls through to
// ShouldStripAllDetectableWithoutRetain (else-if strip-all Detectable).
// Go dual-wire: ranger.ShouldEvaluateCamouflageRetain
// (internal/ranger/evaluate_camouflage_retain.go).
// Sibling residual gates remain on this header (facing / chance / strip).

// ShouldEvaluateCamouflageRetain is the RETAIN_CAMOUFLAGE mod gate before
// retain roll / strip logic.
//
// Formula (slice 3047 dual-wire):
//   ShouldEvaluateCamouflageRetain(retainCamouflageMod) = retainCamouflageMod > 0
//
// retainCamouflageMod — host-evaluated getMod(Mod::RETAIN_CAMOUFLAGE)
// true  → evaluate retain chance / strip Detectable or partial stealth
// false → fall through to ShouldStripAllDetectableWithoutRetain (strip all)
//
// Dual-wire of Go ranger.ShouldEvaluateCamouflageRetain.
// Call site: CBattleEntity::OnRangedAttack (~3472).
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

// --- Slice 3058: ShouldStripAllDetectableOnFail pure dual-wire ---
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects host-resolved
// retainChance + RNG roll in [0, 100) into ShouldStripAllDetectableOnFail after
// ShouldEvaluateCamouflageRetain opens the retain path. When true, host deletes
// all Detectable flags; when false, host uses ShouldStripPartialStealthOnRetain
// (Sneak/Deodorize/Illusion only).
// Go dual-wire: ranger.ShouldStripAllDetectableOnFail
// (internal/ranger/strip_detectable_fail.go).
// Siblings NOT dual-wired this slice: ShouldStripPartialStealthOnRetain,
// ShouldStripAllDetectableWithoutRetain (remain residual 1391).

// ShouldStripAllDetectableOnFail mirrors roll > retainChance (delete Detectable flag).
// Host injects roll in [0, 100).
//
// Formula (slice 3058 dual-wire):
//   ShouldStripAllDetectableOnFail(retainChance, roll0to99) = roll0to99 > retainChance
//   (Go: roll0to99 > int(retainChance))
//
// retainChance — host-resolved ResolveCamouflageRetainChance result
// roll0to99    — host-injected RNG roll in [0, 100)
// true  → strip all Detectable (retain failed)
// false → retain succeeds; host may strip partial stealth only
//
// Dual-wire of Go ranger.ShouldStripAllDetectableOnFail.
// Call site: CBattleEntity::OnRangedAttack (retain path after evaluate gate).
inline auto ShouldStripAllDetectableOnFail(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

// When retain succeeds (not strip all), host deletes Sneak/Deodorize/Illusion only.
// Residual 1391; complement of dual-wire ShouldStripAllDetectableOnFail (3058).
inline auto ShouldStripPartialStealthOnRetain(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// ShouldStripAllDetectableWithoutRetain mirrors else branch when RETAIN_CAMOUFLAGE <= 0.
// Residual 1391; not dual-wired in slice 3058.
inline auto ShouldStripAllDetectableWithoutRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod <= 0;
}

} // namespace camouflageretainhelpers
