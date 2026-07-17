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

// --- Slice 2986: ShouldConsumeAmmo pure dual-wire ---
// Residual pure port: slice 1390 (OnRangedAttack ammo / RemoveAmmo policy suite).
// Production host: CBattleEntity::OnRangedAttack injects
// (PAmmo != nullptr), recycleChance (post ResolveRecycleChance +
// ApplyUnlimitedShotToRecycleChance), and xirand::GetRandomNumber(100) into
// ShouldConsumeAmmo before ++ammoConsumed / scavenge track / hitCount truncate.
// Go dual-wire: attackutils.ShouldConsumeAmmo
// (internal/attackutils/consume_ammo.go).

// ShouldConsumeAmmo mirrors:
//   PAmmo != nullptr && xirand::GetRandomNumber(100) > recycleChance
// Host injects roll in [0, 100).
//
// Formula (slice 2986 dual-wire):
//   if !hasAmmo → false
//   else → roll0to99 > recycleChance
//
// hasAmmo       — host-evaluated (PAmmo != nullptr)
// recycleChance — host-resolved RECYCLE + trait merit + JP + UnlimitedShot
// roll0to99     — host-injected xirand::GetRandomNumber(100) in [0, 100)
// true  → consume ammo for this shot
// false → skip consume (no ammo, or recycle succeeds; strict >)
//
// Dual-wire of Go attackutils.ShouldConsumeAmmo.
// Call site: CBattleEntity::OnRangedAttack (~3291).
inline auto ShouldConsumeAmmo(const bool hasAmmo, const int16 recycleChance, const int roll0to99) -> bool
{
    if (!hasAmmo)
    {
        return false;
    }
    return roll0to99 > recycleChance;
}

// ShouldDeleteUnlimitedShot mirrors:
//   hasUnlimitedShot && (hitOccured || RETAIN_UNLIMITED_SHOT <= 0)
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

// ShouldTruncateHitCountOnAmmoDeplete mirrors:
//   PAmmo->getQuantity() == i  (after deciding to consume this shot)
// When true, host sets hitCount = i to stop further shots.
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

// ShouldDeleteFlashyAndStealthShot mirrors end-of-shot status cleanup for PC.
// Always true on the char ammo path (original always deletes both).
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

// ShouldApplyDistancePenaltyMessage mirrors isChar && messageID != Crit.
inline auto ShouldApplyDistancePenaltyMessage(const bool isChar, const bool isCritMessage) -> bool
{
    return isChar && !isCritMessage;
}

// ShouldForceBarrageSangeHitResolution mirrors:
//   resolution != Hit && (isBarrage || isSange) after at least one hit
inline auto ShouldForceBarrageSangeHitResolution(
    const bool hitOccured,
    const bool resolutionIsHit,
    const bool isBarrage,
    const bool isSange) -> bool
{
    return hitOccured && !resolutionIsHit && (isBarrage || isSange);
}

// ShouldApplyRangedDamageMultiplier mirrors isChar && slot == SLOT_RANGED.
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

// ShouldDeleteBarrageStatus mirrors isBarrage after shot resolution.
inline auto ShouldDeleteBarrageStatus(const bool isBarrage) -> bool
{
    return isBarrage;
}

// ShouldDeleteSangeStatus mirrors isSange after shot resolution.
inline auto ShouldDeleteSangeStatus(const bool isSange) -> bool
{
    return isSange;
}

// Barrage/Sange display message selection when damage > 0.
// Host maps to MsgBasic::UsesSangeTakesDamage vs UsesBarrageTakesDamage.
inline auto ShouldUseSangeDisplayMessage(const bool isSange) -> bool
{
    return isSange;
}

} // namespace rangedammohelpers
