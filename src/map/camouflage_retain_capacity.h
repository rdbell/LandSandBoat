#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Camouflage retain-chance policy from CBattleEntity::OnRangedAttack
// after RemoveAmmo (RETAIN_CAMOUFLAGE > 0 branch). Host injects behind/beside
// geometry, distances, and RNG.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1391: residual pure port (full retain / strip / facing / chance suite)
//   - 3047: ShouldEvaluateCamouflageRetain residual dual-wire expand
//   - 3174: ShouldEvaluateCamouflageRetain dedicated dual-wire
//           (retainCamouflageMod > 0; residual expand 3047 / pure 1391)
//   - 3058: ShouldStripAllDetectableOnFail residual dual-wire expand
//   - 3344: ShouldStripAllDetectableOnFail dedicated dual-wire
//           (roll0to99 > retainChance; residual expand 3058 / pure 1391)
//   - 3082: ShouldStripPartialStealthOnRetain residual dual-wire expand
//   - 3394: ShouldStripPartialStealthOnRetain prior dedicated dual-wire expand residual 3082
//           (retained)
//   - 3445: ShouldStripPartialStealthOnRetain prior dedicated dual-wire expand residual 3082
//           (retained)
//   - 3510: ShouldStripPartialStealthOnRetain dedicated dual-wire
//           (!stripAllDetectable partial gate; expand residual 3082; prior 3445 / 3394 retained)
//   - 3118: ShouldStripAllDetectableWithoutRetain (mod <= 0 without-retain strip)
//
// Production host:
//   - CBattleEntity::OnRangedAttack (~3472) injects getMod(Mod::RETAIN_CAMOUFLAGE)
//     into ShouldEvaluateCamouflageRetain before retain roll / strip logic.
//   - Same host injects retainChance + roll into ShouldStripAllDetectableOnFail
//     after the retain path opens.
//   - Same host injects false into ShouldStripPartialStealthOnRetain on the
//     retain-success else-if (Sneak/Deodorize/Illusion only).
//   - Same host injects getMod(Mod::RETAIN_CAMOUFLAGE) into
//     ShouldStripAllDetectableWithoutRetain on the else-if after evaluate fails.
//
// Go dual-wire:
//   - ranger.ShouldEvaluateCamouflageRetain (evaluate_camouflage_retain.go)
//   - ranger.ShouldStripAllDetectableOnFail (strip_detectable_fail.go)
//   - ranger.ShouldStripPartialStealthOnRetain (strip_partial_retain.go)
//   - ranger.ShouldStripAllDetectableWithoutRetain (strip_without_retain.go)
// Residual Go surface: internal/ranger/camouflage_retain.go (facing / chance helpers).

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

// --- Slice 3174: ShouldEvaluateCamouflageRetain dedicated pure dual-wire ---
// Residual dual-wire expand: slice 3047.
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// getMod(Mod::RETAIN_CAMOUFLAGE) into ShouldEvaluateCamouflageRetain before
// retain roll / strip logic (~3472). When false, host falls through to
// ShouldStripAllDetectableWithoutRetain (else-if strip-all Detectable).
// Go dual-wire: ranger.ShouldEvaluateCamouflageRetain
// (internal/ranger/evaluate_camouflage_retain.go).
// Residual dual-wire suite: 3047 / test_ranger_evaluate_camouflage_retain_3047.
// Dedicated dual-wire suite: 3174 / test_ranger_evaluate_camouflage_retain_3174.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite. Sibling residual gates remain on this header
// (facing / chance / strip). Siblings left alone: 3058, 3082, 3118.

// ShouldEvaluateCamouflageRetain is the RETAIN_CAMOUFLAGE mod gate before
// retain roll / strip logic.
//
// Formula (slice 3174 dedicated dual-wire; residual expand 3047 / pure 1391 —
// formula unchanged):
//   ShouldEvaluateCamouflageRetain(retainCamouflageMod) = retainCamouflageMod > 0
//
// retainCamouflageMod — host-evaluated getMod(Mod::RETAIN_CAMOUFLAGE)
// true  → evaluate retain chance / strip Detectable or partial stealth
// false → fall through to ShouldStripAllDetectableWithoutRetain (strip all)
//
// Dual-wire of Go ranger.ShouldEvaluateCamouflageRetain
// (internal/ranger/evaluate_camouflage_retain.go). Prior pure port: slice 1391.
// Residual dual-wire suite: 3047 / test_ranger_evaluate_camouflage_retain_3047.
// Dedicated dual-wire suite is test_ranger_evaluate_camouflage_retain_3174.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite.
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

// --- Slice 3344: ShouldStripAllDetectableOnFail dedicated pure dual-wire ---
// Residual dual-wire expand: slice 3058.
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects host-resolved
// retainChance + RNG roll in [0, 100) into ShouldStripAllDetectableOnFail after
// ShouldEvaluateCamouflageRetain opens the retain path. When true, host deletes
// all Detectable flags; when false, host uses ShouldStripPartialStealthOnRetain
// (Sneak/Deodorize/Illusion only).
// Go dual-wire: ranger.ShouldStripAllDetectableOnFail
// (internal/ranger/strip_detectable_fail.go).
// Residual dual-wire suite: 3058 / test_ranger_strip_detectable_fail_3058.
// Dedicated dual-wire suite: 3344 / test_ranger_strip_detectable_fail_3344.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite. Sibling residual gates remain on this header
// (facing / chance / strip). Siblings left alone: evaluate_camouflage_retain /
// strip_partial / strip_without_retain (3174 / 3082 / 3118).

// ShouldStripAllDetectableOnFail mirrors roll > retainChance (delete Detectable flag).
// Host injects roll in [0, 100).
//
// Formula (slice 3344 dedicated dual-wire; residual expand 3058 / pure 1391 —
// formula unchanged):
//   ShouldStripAllDetectableOnFail(retainChance, roll0to99) = roll0to99 > retainChance
//   (Go: roll0to99 > int(retainChance))
//
// retainChance — host-resolved ResolveCamouflageRetainChance result
// roll0to99    — host-injected RNG roll in [0, 100)
// true  → strip all Detectable (retain failed)
// false → retain succeeds; host may strip partial stealth only
//
// Dual-wire of Go ranger.ShouldStripAllDetectableOnFail
// (internal/ranger/strip_detectable_fail.go). Prior pure port: slice 1391.
// Residual dual-wire suite: 3058 / test_ranger_strip_detectable_fail_3058.
// Dedicated dual-wire suite is test_ranger_strip_detectable_fail_3344.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite.
// Call site: CBattleEntity::OnRangedAttack (retain path after evaluate gate).
inline auto ShouldStripAllDetectableOnFail(const int16 retainChance, const int roll0to99) -> bool
{
    return roll0to99 > retainChance;
}

// --- Slice 3082 residual / 3394·3445 prior dedicated / 3510 dedicated —
// ShouldStripPartialStealthOnRetain pure dual-wire ---
// Residual dual-wire expand: slice 3082.
// Prior dedicated dual-wire expand residual 3082: slices 3394 / 3445 (retained).
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects false into
// ShouldStripPartialStealthOnRetain when ShouldStripAllDetectableOnFail returns
// false (retain succeeded). When true, host strips Sneak/Deodorize/Illusion only
// (not full Detectable flag). Complement of dual-wire ShouldStripAllDetectableOnFail
// (3344 dedicated / 3058 residual).
// Go dual-wire: ranger.ShouldStripPartialStealthOnRetain
// (internal/ranger/strip_partial_retain.go).
// Residual dual-wire suite: 3082 / test_ranger_strip_partial_retain_3082.
// Prior dedicated dual-wire suites: 3394 / test_ranger_strip_partial_retain_3394,
// 3445 / test_ranger_strip_partial_retain_3445 (retained).
// Dedicated dual-wire suite: 3510 / test_ranger_strip_partial_retain_3510.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite. Sibling residual gates remain on this header
// (facing / chance / strip). Siblings left alone: evaluate_camouflage_retain /
// strip_detectable_fail / strip_without_retain (3174 / 3344·3058 / 3118).

// When retain succeeds (not strip all), host deletes Sneak/Deodorize/Illusion only.
//
// Formula (slice 3510 dedicated dual-wire expand residual 3082; prior dedicated
// 3445 / 3394 / pure 1391 — formula unchanged):
//   ShouldStripPartialStealthOnRetain(stripAllDetectable) = !stripAllDetectable
//
// stripAllDetectable — host-resolved ShouldStripAllDetectableOnFail result
// true  → strip Sneak/Deodorize/Illusion only (retain succeeded; not strip-all)
// false → strip-all Detectable path already taken; no partial-stealth strip
//
// Dual-wire of Go ranger.ShouldStripPartialStealthOnRetain
// (internal/ranger/strip_partial_retain.go). Prior pure port: slice 1391.
// Residual dual-wire suite: 3082 / test_ranger_strip_partial_retain_3082.
// Prior dedicated dual-wire suites: 3394 / test_ranger_strip_partial_retain_3394,
// 3445 / test_ranger_strip_partial_retain_3445 (retained). Dedicated dual-wire
// suite is test_ranger_strip_partial_retain_3510.
// Formula is unchanged; this slice only expands dual-wire docs + index +
// dedicated suite.
// Call site: CBattleEntity::OnRangedAttack (retain-success else-if after strip-all gate).
inline auto ShouldStripPartialStealthOnRetain(const bool stripAllDetectable) -> bool
{
    return !stripAllDetectable;
}

// --- Slice 3118: ShouldStripAllDetectableWithoutRetain pure dual-wire ---
// Residual pure port: slice 1391 (OnRangedAttack RETAIN_CAMOUFLAGE policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// getMod(Mod::RETAIN_CAMOUFLAGE) into ShouldStripAllDetectableWithoutRetain on
// the else-if after ShouldEvaluateCamouflageRetain returns false. When true,
// host deletes all Detectable flags (no RETAIN_CAMOUFLAGE active). Complement of
// dual-wire ShouldEvaluateCamouflageRetain (3174 dedicated / 3047 residual).
// Go dual-wire: ranger.ShouldStripAllDetectableWithoutRetain
// (internal/ranger/strip_without_retain.go).
// Sibling dual-wires left alone this slice: 3058, 3082.

// ShouldStripAllDetectableWithoutRetain mirrors else branch when RETAIN_CAMOUFLAGE <= 0.
//
// Formula (slice 3118 dual-wire):
//   ShouldStripAllDetectableWithoutRetain(retainCamouflageMod) = retainCamouflageMod <= 0
//
// retainCamouflageMod — host-evaluated getMod(Mod::RETAIN_CAMOUFLAGE)
// true  → strip all Detectable (no RETAIN_CAMOUFLAGE / non-positive mod)
// false → evaluate path already open (mod > 0); not the without-retain branch
//
// Dual-wire of Go ranger.ShouldStripAllDetectableWithoutRetain.
// Call site: CBattleEntity::OnRangedAttack (else-if after evaluate gate).
inline auto ShouldStripAllDetectableWithoutRetain(const int16 retainCamouflageMod) -> bool
{
    return retainCamouflageMod <= 0;
}

} // namespace camouflageretainhelpers
