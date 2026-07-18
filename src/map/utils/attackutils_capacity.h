#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure attackutils::CheckForDamageMultiplier policy halves.
//
// Dual-wire index (append-only notes on existing free functions):
//   - 1380: CheckForDamageMultiplier residual pure-gate suite
//   - 3091: ShouldRejectNullWeapon (weaponNull identity null-PWeapon gate)
//   - 3120: ShouldApplyZanshinDoubleDamage (ZANSHIN && rateProcs type double)
//   - 3138: ShouldApplyTATripleDamage (TRIPLE && rateProcs type triple)
//   - 3249: ShouldApplyDADoubleDamage residual dual-wire (expand residual 3120)
//   - 3289: ShouldApplyDADoubleDamage prior dedicated dual-wire (da_double_damage.go;
//           residual expand 3120 / prior 3249)
//   - 3320: ShouldApplyDADoubleDamage dedicated dual-wire (da_double_damage.go;
//           residual expand 3120 / prior 3289)
//   - 3904: ShouldApplyRapidShotDoubleDamage (RAPID_SHOT && rateProcs)
//   - 3905: ShouldApplySambaDoubleDamage (SAMBA && rateProcs)
//   - 3906: RollRatePercent (rate-mod roll boundary)
//   - 3907: ApplyOccProcDamage (outcome-to-damage mapping)
//   - 3908: ResolveAllowProcLadder (exclusive Occ precedence)

namespace attackutilshelpers
{

// PHYSICAL_ATTACK_TYPE pins used by damage multiplier selection.
constexpr uint8 AttackTypeNormal    = 0;
constexpr uint8 AttackTypeDouble    = 1;
constexpr uint8 AttackTypeTriple    = 2;
constexpr uint8 AttackTypeZanshin   = 3;
constexpr uint8 AttackTypeRanged    = 5;
constexpr uint8 AttackTypeRapidShot = 6;
constexpr uint8 AttackTypeSamba     = 7;

// SLOT_MAIN pin.
constexpr uint8 SlotMain = 0;

// --- Slice 3091: ShouldRejectNullWeapon pure dual-wire ---
// Residual pure port: slice 1380 (CheckForDamageMultiplier pure-gate suite).
// Production host: attackutils::CheckForDamageMultiplier injects
// (PWeapon == nullptr) into ShouldRejectNullWeapon; on true return damage
// unchanged before REM occ / allowProc ladder / type rates.
// Go dual-wire: attackutils.ShouldRejectNullWeapon
// (internal/attackutils/reject_null_weapon.go).
// Residual siblings (ranged/main rem occ, occ chance ladder, type double
// rates) remain in the 1380 residual suite. Multi-hits residual reuses the
// same Go free function for its null-weapon pin; multihitshelpers twin is a
// separate C++ namespace and is left alone.
// Index 3091: attackutils.ShouldRejectNullWeapon pure dual-wire.

// ShouldRejectNullWeapon mirrors PWeapon == nullptr early return.
//
// Formula (slice 3091 dual-wire):
//   weaponNull
//
// weaponNull — host-injected (PWeapon == nullptr)
// true  → host returns original damage (passthrough, no multiplier work)
// false → proceed to REM occ selection / allowProc ladder / type rates
//
// Dual-wire of Go attackutils.ShouldRejectNullWeapon.
// Call site: attackutils::CheckForDamageMultiplier — host injects
// PWeapon == nullptr; on true return damage unchanged.
// Prior pure port: slice 1380 (damage multiplier residual pure-gate suite).
// Residual pins remain in test_attackutils_multiplier_1380; dedicated
// dual-wire suite is test_attackutils_reject_null_weapon_3091.
// Residual siblings: ShouldUseRangedRemOcc / ShouldUseMainHandRemOcc /
// occ ladder / type rates (still 1380).
inline auto ShouldRejectNullWeapon(const bool weaponNull) -> bool
{
    return weaponNull;
}

// ResolveRemOccRates selects REM occ triple/double chance (already /10 applied by host).
// Host injects the mod/10 values for the attack type branch.
inline auto ShouldUseRangedRemOcc(const uint8 attackType) -> bool
{
    return attackType == AttackTypeRanged || attackType == AttackTypeRapidShot;
}

// ShouldUseMainHandRemOcc mirrors NORMAL && weaponSlot == SLOT_MAIN.
inline auto ShouldUseMainHandRemOcc(const uint8 attackType, const uint8 weaponSlot) -> bool
{
    return attackType == AttackTypeNormal && weaponSlot == SlotMain;
}

// OccExtraDmgMultiplier mirrors GetScaledItemModifier OCC_DO_EXTRA_DMG / 100.
// Host injects the scaled raw mod; pure half divides by 100.
inline auto OccExtraDmgMultiplier(const int16 scaledOccExtraDmgMod) -> float
{
    return static_cast<float>(scaledOccExtraDmgMod) / 100.0f;
}

// OccExtraDmgChance mirrors GetScaledItemModifier EXTRA_DMG_CHANCE / 10.
inline auto OccExtraDmgChance(const int16 scaledExtraDmgChanceMod) -> int16
{
    return static_cast<int16>(scaledExtraDmgChanceMod / 10);
}

// RemOccChance mirrors rem mod / 10.
inline auto RemOccChance(const int16 remOccMod) -> int16
{
    return static_cast<int16>(remOccMod / 10);
}

// RollChancePercent mirrors (1 + rand(100)) <= chance — inclusive 1..100 vs chance.
// Host injects roll in 0..99 (xirand::GetRandomNumber(100)); pure uses 1+roll.
inline auto RollChancePercent(const int16 chance, const int roll0to99) -> bool
{
    if (chance <= 0)
    {
        return false;
    }
    return (1 + roll0to99) <= chance;
}

// Occ damage ladder preference when allowProc (exclusive first-match):
// 1) extra > 3.0 && chance roll
// 2) rem triple chance roll → *3
// 3) extra > 2.0 && chance roll
// 4) rem double chance roll → *2
// 5) extra > 0 && chance roll
enum class OccProcResult : uint8
{
    None          = 0,
    ExtraDamage   = 1,
    RemTriple     = 2,
    RemDouble     = 3,
};

// ResolveAllowProcLadder evaluates the allowProc exclusive ladder. Its order
// is semantically significant: each earlier winning arm suppresses later RNG
// checks and outcomes.
// Each *Procs flag is the result of RollChancePercent for that step.
// Host must only roll when the preceding steps fail (short-circuit RNG order).
inline auto ResolveAllowProcLadder(
    const bool allowProc,
    const float occExtraDmg,
    const bool extraGt3Procs,
    const bool remTripleProcs,
    const bool extraGt2Procs,
    const bool remDoubleProcs,
    const bool extraAnyProcs) -> OccProcResult
{
    if (!allowProc)
    {
        return OccProcResult::None;
    }
    if (occExtraDmg > 3.0f && extraGt3Procs)
    {
        return OccProcResult::ExtraDamage;
    }
    if (remTripleProcs)
    {
        return OccProcResult::RemTriple;
    }
    if (occExtraDmg > 2.0f && extraGt2Procs)
    {
        return OccProcResult::ExtraDamage;
    }
    if (remDoubleProcs)
    {
        return OccProcResult::RemDouble;
    }
    if (occExtraDmg > 0.0f && extraAnyProcs)
    {
        return OccProcResult::ExtraDamage;
    }
    return OccProcResult::None;
}

// ApplyOccProcDamage applies the ladder result. Extra damage preserves LSB's
// float multiplication followed by uint32 truncation; rem outcomes are exact
// two- or three-times multipliers and no outcome leaves damage unchanged.
inline auto ApplyOccProcDamage(
    const uint32 damage,
    const OccProcResult result,
    const float occExtraDmg) -> uint32
{
    switch (result)
    {
        case OccProcResult::ExtraDamage:
            return static_cast<uint32>(damage * occExtraDmg);
        case OccProcResult::RemTriple:
            return static_cast<uint32>(damage * 3.0f);
        case OccProcResult::RemDouble:
            return static_cast<uint32>(damage * 2.0f);
        default:
            return damage;
    }
}

// Type-specific double/triple damage after allowProc ladder fails.
// Host injects whether the rate roll succeeded for the attack type.

// --- Slice 3120: ShouldApplyZanshinDoubleDamage pure dual-wire ---
// Residual pure port: slice 1380 (CheckForDamageMultiplier pure-gate suite).
// Production host: attackutils::CheckForDamageMultiplier switch
// PHYSICAL_ATTACK_TYPE::ZANSHIN injects attackTypeU8 and
// RollRatePercent(Mod::ZANSHIN_DOUBLE_DAMAGE, rand) into
// ShouldApplyZanshinDoubleDamage; on true return
// ApplyTypeDoubleDamage(originalDamage, 2).
// Go dual-wire: attackutils.ShouldApplyZanshinDoubleDamage
// (internal/attackutils/zanshin_double_damage.go).
// Residual siblings (TA triple dual-wire 3138, DA double dual-wire 3249,
// Rapid Shot, Samba, ApplyTypeDoubleDamage, RollRatePercent, null weapon 3091,
// rem occ, allowProc ladder) remain in the 1380 residual suite / dual-wire
// siblings.
// Index 3120: attackutils.ShouldApplyZanshinDoubleDamage pure dual-wire.
//
// ShouldApplyZanshinDoubleDamage mirrors ZANSHIN + rate proc.
//
// Formula (slice 3120 dual-wire):
//   attackType == AttackTypeZanshin && rateProcs
//
// attackType — host-injected physical attack type (uint8 / enum)
// rateProcs  — host-injected RollRatePercent(ZANSHIN_DOUBLE_DAMAGE, roll)
// true  → host ApplyTypeDoubleDamage(originalDamage, 2)
// false → leave damage unchanged for this type arm
//
// Dual-wire of Go attackutils.ShouldApplyZanshinDoubleDamage.
// Call site: attackutils::CheckForDamageMultiplier — host injects
// attackTypeU8 + rate roll; on true return ApplyTypeDoubleDamage(*2).
// Prior pure port: slice 1380 (damage multiplier residual pure-gate suite).
// Residual pins remain in test_attackutils_multiplier_1380; dedicated
// dual-wire suite is test_attackutils_zanshin_dd_3120.
// Residual siblings: ShouldApplyTATripleDamage (3138 dual-wire) /
// ShouldApplyDADoubleDamage (3249 dual-wire) / Rapid / Samba /
// ApplyTypeDoubleDamage / RollRatePercent (still 1380 unless dual-wired).
inline auto ShouldApplyZanshinDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == AttackTypeZanshin && rateProcs;
}

// --- Slice 3138: ShouldApplyTATripleDamage pure dual-wire ---
// Residual pure port: slice 1380 (CheckForDamageMultiplier pure-gate suite).
// Production host: attackutils::CheckForDamageMultiplier switch
// PHYSICAL_ATTACK_TYPE::TRIPLE injects attackTypeU8 and
// RollRatePercent(Mod::TA_TRIPLE_DMG_RATE, rand) into
// ShouldApplyTATripleDamage; on true return
// ApplyTypeDoubleDamage(originalDamage, 3).
// Go dual-wire: attackutils.ShouldApplyTATripleDamage
// (internal/attackutils/ta_triple_damage.go).
// Residual siblings (Zanshin double dual-wire 3120, DA double dual-wire 3249,
// Rapid Shot, Samba, ApplyTypeDoubleDamage, RollRatePercent, null weapon 3091,
// rem occ, allowProc ladder) remain in the 1380 residual suite / dual-wire
// siblings left alone.
// Index 3138: attackutils.ShouldApplyTATripleDamage pure dual-wire.
//
// ShouldApplyTATripleDamage mirrors TRIPLE + rate proc.
//
// Formula (slice 3138 dual-wire):
//   attackType == AttackTypeTriple && rateProcs
//
// attackType — host-injected physical attack type (uint8 / enum)
// rateProcs  — host-injected RollRatePercent(TA_TRIPLE_DMG_RATE, roll)
// true  → host ApplyTypeDoubleDamage(originalDamage, 3)
// false → leave damage unchanged for this type arm
//
// Dual-wire of Go attackutils.ShouldApplyTATripleDamage.
// Call site: attackutils::CheckForDamageMultiplier — host injects
// attackTypeU8 + rate roll; on true return ApplyTypeDoubleDamage(*3).
// Prior pure port: slice 1380 (damage multiplier residual pure-gate suite).
// Residual pins remain in test_attackutils_multiplier_1380; dedicated
// dual-wire suite is test_attackutils_ta_triple_3138.
// Residual siblings: ShouldApplyZanshinDoubleDamage (3120) /
// ShouldApplyDADoubleDamage (3249 dual-wire) / Rapid / Samba /
// ApplyTypeDoubleDamage / RollRatePercent (still 1380 unless dual-wired).
inline auto ShouldApplyTATripleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == AttackTypeTriple && rateProcs;
}

// --- Slice 3320: ShouldApplyDADoubleDamage pure dual-wire ---
// Dedicated dual-wire expand residual 3120 / prior dedicated 3289.
// Residual pure port: slice 1380 (CheckForDamageMultiplier pure-gate suite).
// Residual dual-wire: slice 3249 (test_attackutils_da_double_damage_3249).
// Prior dedicated dual-wire: slice 3289 (test_attackutils_da_double_damage_3289).
// Production host: attackutils::CheckForDamageMultiplier switch
// PHYSICAL_ATTACK_TYPE::DOUBLE injects attackTypeU8 and
// RollRatePercent(Mod::DA_DOUBLE_DMG_RATE, rand) into
// ShouldApplyDADoubleDamage; on true return
// ApplyTypeDoubleDamage(originalDamage, 2).
// Go dual-wire: attackutils.ShouldApplyDADoubleDamage
// (internal/attackutils/da_double_damage.go).
// Residual siblings (Zanshin double dual-wire 3120, TA triple dual-wire 3138,
// Rapid Shot, Samba, ApplyTypeDoubleDamage, RollRatePercent, null weapon 3091,
// rem occ, allowProc ladder) remain in the 1380 residual suite / dual-wire
// siblings left alone.
// Index 3320: attackutils.ShouldApplyDADoubleDamage pure dual-wire
// (dedicated expand residual 3120 / prior 3289; formula unchanged).
//
// ShouldApplyDADoubleDamage mirrors DOUBLE + rate proc.
//
// Formula (slice 3320 dedicated dual-wire; residual expand 3120 / prior 3289 —
// formula unchanged):
//   attackType == AttackTypeDouble && rateProcs
//
// attackType — host-injected physical attack type (uint8 / enum)
// rateProcs  — host-injected RollRatePercent(DA_DOUBLE_DMG_RATE, roll)
// true  → host ApplyTypeDoubleDamage(originalDamage, 2)
// false → leave damage unchanged for this type arm
//
// Dual-wire of Go attackutils.ShouldApplyDADoubleDamage.
// Call site: attackutils::CheckForDamageMultiplier — host injects
// attackTypeU8 + rate roll; on true return ApplyTypeDoubleDamage(*2).
// Prior pure port: slice 1380 (damage multiplier residual pure-gate suite).
// Residual pins remain in test_attackutils_multiplier_1380; residual
// dual-wire suite is test_attackutils_da_double_damage_3249; prior dedicated
// dual-wire suite is test_attackutils_da_double_damage_3289; dedicated
// dual-wire suite is test_attackutils_da_double_damage_3320.
// Residual siblings: ShouldApplyZanshinDoubleDamage (3120) /
// ShouldApplyTATripleDamage (3138) / Rapid / Samba / ApplyTypeDoubleDamage /
// RollRatePercent (still 1380 unless dual-wired).
inline auto ShouldApplyDADoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == AttackTypeDouble && rateProcs;
}

// ShouldApplyRapidShotDoubleDamage mirrors RAPID_SHOT + rate proc. The host
// supplies the RAPID_SHOT_DAMAGE rate roll and doubles the original damage on
// success; other multiplier switch arms remain mutually exclusive.
inline auto ShouldApplyRapidShotDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == AttackTypeRapidShot && rateProcs;
}

// ShouldApplySambaDoubleDamage mirrors SAMBA + rate proc. The host supplies
// the SAMBA_DOUBLE_DAMAGE rate roll and doubles the original damage on success.
inline auto ShouldApplySambaDoubleDamage(const uint8 attackType, const bool rateProcs) -> bool
{
    return attackType == AttackTypeSamba && rateProcs;
}

// ApplyTypeDoubleDamage multiplies original damage by factor.
inline auto ApplyTypeDoubleDamage(const uint32 originalDamage, const uint8 factor) -> uint32
{
    return originalDamage * factor;
}

// RollRatePercent mirrors rand(100) < mod (0..99 style). Nonpositive rates
// never proc; a valid host roll exactly equal to the rate does not proc.
inline auto RollRatePercent(const int16 rateMod, const int roll0to99) -> bool
{
    if (rateMod <= 0)
    {
        return false;
    }
    return roll0to99 < rateMod;
}

} // namespace attackutilshelpers
