#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure OnRangedAttack ammo-consumption and RemoveAmmo policy halves.
// Inventory mutation, trait/merit/JP lookups, and packets remain host-injected.

namespace rangedammohelpers
{

// MsgBasic pins for ranged hit quality messages (distance penalty path).
constexpr uint16 MsgRangedAttackHit      = 352; // MsgBasic::RangedAttackHit
constexpr uint16 MsgRangedAttackCrit     = 353; // MsgBasic::RangedAttackCrit
constexpr uint16 MsgRangedAttackSquarely = 576; // MsgBasic::RangedAttackSquarely
constexpr uint16 MsgRangedAttackPummels  = 577; // MsgBasic::RangedAttackPummels
constexpr uint16 MsgUsesSangeTakesDamage = 77;  // MsgBasic::UsesSangeTakesDamage
constexpr uint16 MsgUsesBarrageTakesDamage = 157; // MsgBasic::UsesBarrageTakesDamage

// ResolveRecycleChance mirrors:
//   RECYCLE mod + (TRAIT_RECYCLE ? merit : 0) + JP_AMMO_CONSUMPTION
// Host injects each component already resolved.
inline auto ResolveRecycleChance(
    const int16 recycleMod,
    const bool hasRecycleTrait,
    const int16 recycleMerit,
    const int16 ammoConsumptionJP) -> int16
{
    int16 chance = recycleMod;
    if (hasRecycleTrait)
    {
        chance = static_cast<int16>(chance + recycleMerit);
    }
    chance = static_cast<int16>(chance + ammoConsumptionJP);
    return chance;
}

// ApplyUnlimitedShotToRecycleChance forces recycleChance = 100 when UnlimitedShot is up.
inline auto ApplyUnlimitedShotToRecycleChance(const int16 recycleChance, const bool hasUnlimitedShot) -> int16
{
    if (hasUnlimitedShot)
    {
        return 100;
    }
    return recycleChance;
}

// --- Slice 3364: ShouldConsumeAmmo pure dual-wire (dedicated expand residual 2986) ---
// Residual dual-wire: slice 2986 (ShouldConsumeAmmo pure dual-wire suite retained).
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// (PAmmo != nullptr), recycleChance (post ResolveRecycleChance +
// ApplyUnlimitedShotToRecycleChance), and xirand::GetRandomNumber(100) into
// ShouldConsumeAmmo before ++ammoConsumed / scavenge track / hitCount truncate.
// Go dual-wire: attackutils.ShouldConsumeAmmo
// (internal/attackutils/consume_ammo.go).
//
// Dual-wire notes (slice 3364 dedicated expand residual 2986):
//   Formula unchanged from residual 1390 / residual dual-wire 2986:
//     if !hasAmmo → false
//     else → roll0to99 > recycleChance
//   Early-return form (production free function + 3364 / 2986 inline/pin):
//     if (!hasAmmo) return false; return roll0to99 > recycleChance;
//   Index 3364: attackutils.ShouldConsumeAmmo pure dual-wire
//     (dedicated expand residual 2986).
//   Residual dual-wire suite: test_ranged_consume_ammo_2986 (retained).
//   Dedicated dual-wire suite: test_ranged_consume_ammo_3364.
//
// Sibling dual-wires (ShouldDeleteUnlimitedShot, truncate, etc.) left alone.

// ShouldConsumeAmmo mirrors:
//   PAmmo != nullptr && xirand::GetRandomNumber(100) > recycleChance
// Host injects roll in [0, 100).
//
// Formula (slice 3364 dedicated dual-wire expand residual 2986; prior
// residual 2986 / pure 1390 — formula unchanged):
//   if !hasAmmo → false
//   else → roll0to99 > recycleChance
//
// hasAmmo       — host-evaluated (PAmmo != nullptr)
// recycleChance — host-resolved RECYCLE + trait merit + JP + UnlimitedShot
// roll0to99     — host-injected xirand::GetRandomNumber(100) in [0, 100)
// true  → consume ammo for this shot
// false → skip consume (no ammo, or recycle succeeds; strict >)
//
// Dual-wire of Go attackutils.ShouldConsumeAmmo (consume_ammo.go / slice 3364).
// Call site: CBattleEntity::OnRangedAttack (~3291).
inline auto ShouldConsumeAmmo(const bool hasAmmo, const int16 recycleChance, const int roll0to99) -> bool
{
    if (!hasAmmo)
    {
        return false;
    }
    return roll0to99 > recycleChance;
}

// --- Slice 3451: ShouldDeleteUnlimitedShot pure dual-wire (dedicated expand residual 3000) ---
// Prior dedicated dual-wire: slice 3399 (suite retained).
// Residual dual-wire: slice 3000 (ShouldDeleteUnlimitedShot pure dual-wire suite retained).
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// hasUnlimitedShot (StatusEffect::UnlimitedShot), hitOccured (any shot hit),
// and getMod(Mod::RETAIN_UNLIMITED_SHOT) into ShouldDeleteUnlimitedShot before
// DelStatusEffect(UnlimitedShot) on true.
// Go dual-wire: attackutils.ShouldDeleteUnlimitedShot
// (internal/attackutils/delete_unlimited_shot.go).
//
// Dual-wire notes (slice 3451 dedicated expand residual 3000; prior 3399 retained):
//   Formula unchanged from residual 1390 / residual dual-wire 3000 / dedicated 3399:
//     if !hasUnlimitedShot → false
//     else → hitOccured || retainUnlimitedShotMod <= 0
//   Early-return form (production free function + 3451 / 3399 / 3000 inline/pin):
//     if (!hasUnlimitedShot) return false; return hitOccured || retainUnlimitedShotMod <= 0;
//   Index 3451: attackutils.ShouldDeleteUnlimitedShot pure dual-wire
//     (dedicated expand residual 3000).
//   Residual dual-wire suite: test_ranged_delete_unlimited_shot_3000 (retained).
//   Prior dedicated dual-wire suite: test_ranged_delete_unlimited_shot_3399 (retained).
//   Dedicated dual-wire suite: test_ranged_delete_unlimited_shot_3451.
//
// Sibling dual-wires (ShouldConsumeAmmo 3364/2986, truncate, etc.) left alone.

// ShouldDeleteUnlimitedShot mirrors:
//   hasUnlimitedShot && (hitOccured || RETAIN_UNLIMITED_SHOT <= 0)
//
// Formula (slice 3451 dedicated dual-wire expand residual 3000; prior
// dedicated 3399 / residual 3000 / pure 1390 — formula unchanged):
//   if !hasUnlimitedShot → false
//   else → hitOccured || retainUnlimitedShotMod <= 0
//
// hasUnlimitedShot       — host-evaluated StatusEffect::UnlimitedShot present
// hitOccured             — host-tracked whether any ranged shot hit this action
// retainUnlimitedShotMod — host-injected getMod(Mod::RETAIN_UNLIMITED_SHOT)
// true  → host DelStatusEffect(UnlimitedShot)
// false → keep UnlimitedShot (no effect, or miss with retain mod > 0)
//
// Dual-wire of Go attackutils.ShouldDeleteUnlimitedShot
// (delete_unlimited_shot.go / slice 3451; prior 3399 retained).
// Call site: CBattleEntity::OnRangedAttack (~3280).
inline auto ShouldDeleteUnlimitedShot(
    const bool hasUnlimitedShot,
    const bool hitOccured,
    const int16 retainUnlimitedShotMod) -> bool
{
    if (!hasUnlimitedShot)
    {
        return false;
    }
    return hitOccured || retainUnlimitedShotMod <= 0;
}

// --- Slice 3010: ShouldTruncateHitCountOnAmmoDeplete pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// consumingThisShot = true (literal; already inside ShouldConsumeAmmo true branch),
// ammoQuantityBeforeConsume = static_cast<uint8>(PAmmo->getQuantity()),
// and shotIndexI = i into ShouldTruncateHitCountOnAmmoDeplete before
// hitCount = i on true (~3295).
// Go dual-wire: attackutils.ShouldTruncateHitCountOnAmmoDeplete
// (internal/attackutils/truncate_hit_count.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007).

// ShouldTruncateHitCountOnAmmoDeplete mirrors:
//   PAmmo->getQuantity() == i  (after deciding to consume this shot)
// When true, host sets hitCount = i to stop further shots.
//
// Formula (slice 3010 dual-wire):
//   if !consumingThisShot → false
//   else → ammoQuantityBeforeConsume == shotIndexI
//
// consumingThisShot         — host already decided to consume this shot
// ammoQuantityBeforeConsume — host-read PAmmo->getQuantity() before decrement
// shotIndexI                — current multi-shot loop index i
// true  → host sets hitCount = i to stop further barrage/sange shots
// false → continue remaining shots
//
// Dual-wire of Go attackutils.ShouldTruncateHitCountOnAmmoDeplete.
// Call site: CBattleEntity::OnRangedAttack (~3295) injects true + qty + i
// inside the ShouldConsumeAmmo true branch.
inline auto ShouldTruncateHitCountOnAmmoDeplete(
    const bool consumingThisShot,
    const uint8 ammoQuantityBeforeConsume,
    const uint8 shotIndexI) -> bool
{
    if (!consumingThisShot)
    {
        return false;
    }
    return ammoQuantityBeforeConsume == shotIndexI;
}

// --- Slice 3007: ShouldDeleteFlashyAndStealthShot pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects true on the char ammo
// path into ShouldDeleteFlashyAndStealthShot before DelStatusEffect(FlashyShot)
// and DelStatusEffect(StealthShot) on true (~3285).
// Go dual-wire: attackutils.ShouldDeleteFlashyAndStealthShot
// (internal/attackutils/delete_flashy_stealth.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000).

// ShouldDeleteFlashyAndStealthShot mirrors end-of-shot status cleanup for PC.
// Always true on the char ammo path (original always deletes both).
//
// Formula (slice 3007 dual-wire; identity):
//   return isChar
//
// isChar — host-evaluated entity is a character (PC ammo path)
// true  → host DelStatusEffect(FlashyShot) and DelStatusEffect(StealthShot)
// false → skip cleanup (non-char path does not clear these effects here)
//
// Dual-wire of Go attackutils.ShouldDeleteFlashyAndStealthShot.
// Call site: CBattleEntity::OnRangedAttack (~3285) injects true.
inline auto ShouldDeleteFlashyAndStealthShot(const bool isChar) -> bool
{
    return isChar;
}

// RemoveAmmoEmptiesSlot mirrors (quantity - consumeQty) < 1.
// Returns true when host should unequip (emptied), false when only decrement.
// No ammo → false (RemoveAmmo returns false without action).
inline auto RemoveAmmoEmptiesSlot(const bool hasAmmo, const uint16 quantity, const int consumeQty) -> bool
{
    if (!hasAmmo)
    {
        return false;
    }
    return (static_cast<int>(quantity) - consumeQty) < 1;
}

// RemoveAmmoShouldAct mirrors PItem != nullptr.
inline auto RemoveAmmoShouldAct(const bool hasAmmo) -> bool
{
    return hasAmmo;
}

// Distance penalty message selection for successful ranged hits (non-crit path):
//   0 → Pummels, <= 15 → Squarely, else → Hit
enum class RangedDistanceMsg : uint8
{
    Hit      = 0,
    Squarely = 1,
    Pummels  = 2,
};

inline auto ResolveRangedDistanceMessage(const int distancePenalty) -> RangedDistanceMsg
{
    if (distancePenalty == 0)
    {
        return RangedDistanceMsg::Pummels;
    }
    if (distancePenalty <= 15)
    {
        return RangedDistanceMsg::Squarely;
    }
    return RangedDistanceMsg::Hit;
}

// Map RangedDistanceMsg to MsgBasic pin values.
inline auto RangedDistanceMsgID(const RangedDistanceMsg msg) -> uint16
{
    switch (msg)
    {
        case RangedDistanceMsg::Pummels:
            return MsgRangedAttackPummels;
        case RangedDistanceMsg::Squarely:
            return MsgRangedAttackSquarely;
        case RangedDistanceMsg::Hit:
        default:
            return MsgRangedAttackHit;
    }
}

// --- Slice 3013: ShouldApplyDistancePenaltyMessage pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects isChar and
// (actionResult.messageID == MsgBasic::RangedAttackCrit) into
// ShouldApplyDistancePenaltyMessage before resolving AttackDistancePenalty and
// overwriting messageID via RangedDistanceMsgID (~3309).
// Go dual-wire: attackutils.ShouldApplyDistancePenaltyMessage
// (internal/attackutils/distance_penalty_message.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007).

// ShouldApplyDistancePenaltyMessage mirrors isChar && messageID != Crit.
//
// Formula (slice 3013 dual-wire):
//   return isChar && !isCritMessage
//
// isChar        — host-evaluated entity is a character (PC ranged path)
// isCritMessage — host-evaluated actionResult.messageID == RangedAttackCrit
// true  → host resolves AttackDistancePenalty + RangedDistanceMsg and overwrites
//         actionResult.messageID (Pummels / Squarely / Hit)
// false → keep existing message (non-char, or crit already selected)
//
// Dual-wire of Go attackutils.ShouldApplyDistancePenaltyMessage.
// Call site: CBattleEntity::OnRangedAttack (~3309).
inline auto ShouldApplyDistancePenaltyMessage(const bool isChar, const bool isCritMessage) -> bool
{
    return isChar && !isCritMessage;
}

// --- Slice 3035: ShouldForceBarrageSangeHitResolution pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects true (literal on the
// successful-hit path), (actionResult.resolution == ActionResolution::Hit),
// isBarrage, and isSange into ShouldForceBarrageSangeHitResolution before
// setting actionResult.resolution = ActionResolution::Hit on true (~3343).
// Go dual-wire: attackutils.ShouldForceBarrageSangeHitResolution
// (internal/attackutils/force_barrage_sange_hit.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007), ShouldTruncateHitCountOnAmmoDeplete (3010),
// ShouldApplyDistancePenaltyMessage (3013), ShouldDeleteBarrageStatus (3018),
// ShouldApplyRangedDamageMultiplier (3023), ShouldDeleteSangeStatus (3030).

// ShouldForceBarrageSangeHitResolution mirrors:
//   hitOccured && resolution != Hit && (isBarrage || isSange)
// Any misses with Barrage/Sange cause remaining shots to miss, so resolution
// must be forced to Hit when at least one hit occurred.
//
// Formula (slice 3035 dual-wire):
//   return hitOccured && !resolutionIsHit && (isBarrage || isSange)
//
// hitOccured      — host-tracked whether any ranged shot hit this action
//                   (production injects true on the successful-hit path)
// resolutionIsHit — host-evaluated actionResult.resolution == ActionResolution::Hit
// isBarrage       — host-evaluated StatusEffect::Barrage present at shot start
// isSange         — host-evaluated StatusEffect::Sange present at shot start
// true  → host sets actionResult.resolution = ActionResolution::Hit
// false → keep existing resolution (no hit, already Hit, or neither effect)
//
// Dual-wire of Go attackutils.ShouldForceBarrageSangeHitResolution.
// Call site: CBattleEntity::OnRangedAttack (~3343).
inline auto ShouldForceBarrageSangeHitResolution(
    const bool hitOccured,
    const bool resolutionIsHit,
    const bool isBarrage,
    const bool isSange) -> bool
{
    return hitOccured && !resolutionIsHit && (isBarrage || isSange);
}

// --- Slice 3023: ShouldApplyRangedDamageMultiplier pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects isChar and
// (slot == SLOT_RANGED) into ShouldApplyRangedDamageMultiplier before
// CheckForDamageMultiplier(PChar, PItem, totalDamage, attackType, slot, true)
// with ResolveRangedPhysicalAttackType(isRapidShot) (~3352).
// Go dual-wire: attackutils.ShouldApplyRangedDamageMultiplier
// (internal/attackutils/ranged_damage_multiplier.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007), ShouldTruncateHitCountOnAmmoDeplete (3010),
// ShouldApplyDistancePenaltyMessage (3013), ShouldDeleteBarrageStatus (3018),
// ShouldDeleteSangeStatus (3030).

// ShouldApplyRangedDamageMultiplier mirrors isChar && slot == SLOT_RANGED.
//
// Formula (slice 3023 dual-wire):
//   return isChar && slotIsRanged
//
// isChar       — host-evaluated entity is a character (PC ranged path)
// slotIsRanged — host-evaluated slot == SLOT_RANGED (not ammo/throwing-only)
// true  → host CheckForDamageMultiplier(PChar, PItem, totalDamage, attackType,
//         slot, true) with ResolveRangedPhysicalAttackType(isRapidShot)
// false → skip damage-multiplier adjustment (non-char, or non-ranged slot)
//
// Dual-wire of Go attackutils.ShouldApplyRangedDamageMultiplier.
// Call site: CBattleEntity::OnRangedAttack (~3352).
inline auto ShouldApplyRangedDamageMultiplier(const bool isChar, const bool slotIsRanged) -> bool
{
    return isChar && slotIsRanged;
}

// ResolveRangedPhysicalAttackType mirrors RapidShot ? RAPID_SHOT : RANGED.
// PHYSICAL_ATTACK_TYPE: RANGED=5, RAPID_SHOT=6.
constexpr uint8 AttackTypeRanged    = 5;
constexpr uint8 AttackTypeRapidShot = 6;

inline auto ResolveRangedPhysicalAttackType(const bool isRapidShot) -> uint8
{
    return isRapidShot ? AttackTypeRapidShot : AttackTypeRanged;
}

// --- Slice 3018: ShouldDeleteBarrageStatus pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects isBarrage
// (StatusEffectContainer->HasStatusEffect(Barrage, 0) captured at shot start)
// into ShouldDeleteBarrageStatus before DelStatusEffectSilent(Barrage) on true
// (~3449).
// Go dual-wire: attackutils.ShouldDeleteBarrageStatus
// (internal/attackutils/delete_barrage_status.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007), ShouldTruncateHitCountOnAmmoDeplete (3010),
// ShouldApplyDistancePenaltyMessage (3013).

// ShouldDeleteBarrageStatus mirrors isBarrage after shot resolution.
//
// Formula (slice 3018 dual-wire; identity):
//   return isBarrage
//
// isBarrage — host-evaluated StatusEffect::Barrage present at shot start
// true  → host DelStatusEffectSilent(Barrage)
// false → skip cleanup (Barrage was not active for this action)
//
// Dual-wire of Go attackutils.ShouldDeleteBarrageStatus.
// Call site: CBattleEntity::OnRangedAttack (~3449).
inline auto ShouldDeleteBarrageStatus(const bool isBarrage) -> bool
{
    return isBarrage;
}

// --- Slice 3030: ShouldDeleteSangeStatus pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects isSange
// (StatusEffectContainer->HasStatusEffect(Sange, 0) captured at shot start)
// into ShouldDeleteSangeStatus before DelStatusEffectSilent(Sange) on true
// (~3454).
// Go dual-wire: attackutils.ShouldDeleteSangeStatus
// (internal/attackutils/delete_sange_status.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007), ShouldTruncateHitCountOnAmmoDeplete (3010),
// ShouldApplyDistancePenaltyMessage (3013), ShouldDeleteBarrageStatus (3018),
// ShouldApplyRangedDamageMultiplier (3023).

// ShouldDeleteSangeStatus mirrors isSange after shot resolution.
//
// Formula (slice 3030 dual-wire; identity):
//   return isSange
//
// isSange — host-evaluated StatusEffect::Sange present at shot start
// true  → host DelStatusEffectSilent(Sange)
// false → skip cleanup (Sange was not active for this action)
//
// Dual-wire of Go attackutils.ShouldDeleteSangeStatus.
// Call site: CBattleEntity::OnRangedAttack (~3454).
inline auto ShouldDeleteSangeStatus(const bool isSange) -> bool
{
    return isSange;
}

// --- Slice 3040: ShouldUseSangeDisplayMessage pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects isSange
// (StatusEffectContainer->HasStatusEffect(Sange, 0) captured at shot start)
// into ShouldUseSangeDisplayMessage when (isBarrage || isSange) && hitOccured
// before mapping messageID to UsesSangeTakesDamage vs UsesBarrageTakesDamage
// (~3443).
// Go dual-wire: attackutils.ShouldUseSangeDisplayMessage
// (internal/attackutils/use_sange_display_message.go).
// Sibling dual-wires: ShouldConsumeAmmo (2986), ShouldDeleteUnlimitedShot (3000),
// ShouldDeleteFlashyAndStealthShot (3007), ShouldTruncateHitCountOnAmmoDeplete (3010),
// ShouldApplyDistancePenaltyMessage (3013), ShouldDeleteBarrageStatus (3018),
// ShouldApplyRangedDamageMultiplier (3023), ShouldDeleteSangeStatus (3030),
// ShouldForceBarrageSangeHitResolution (3035).

// ShouldUseSangeDisplayMessage selects Sange vs Barrage ability damage msg.
//
// Formula (slice 3040 dual-wire; identity):
//   return isSange
//
// isSange — host-evaluated StatusEffect::Sange present at shot start
// true  → host maps messageID to MsgBasic::UsesSangeTakesDamage (77)
// false → host maps messageID to MsgBasic::UsesBarrageTakesDamage (157)
//
// Host gate before call: (isBarrage || isSange) && hitOccured; on a full miss
// the existing RangedAttackMiss message is left in place.
//
// Dual-wire of Go attackutils.ShouldUseSangeDisplayMessage.
// Call site: CBattleEntity::OnRangedAttack (~3443).
inline auto ShouldUseSangeDisplayMessage(const bool isSange) -> bool
{
    return isSange;
}

} // namespace rangedammohelpers
