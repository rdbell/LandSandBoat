#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstdint>

// Pure OnRangedAttack hit-count resolution half
// (CBattleEntity::OnRangedAttack hitCount assembly before the hit loop).
// Barrage level ladder, RNG draws, and status/equip injects remain host-side.

namespace rangedhitcounthelpers
{

// Base hit count before Barrage/Sange/multi-shot adjustments.
constexpr uint8 BaseRangedHitCount = 1;

// Triple / Double shot fixed hit counts.
constexpr uint8 TripleShotHitCount = 3;
constexpr uint8 DoubleShotHitCount = 2;

// Sange utsusemi shadow clamp [0, 7].
constexpr int32 SangeShadowClampMin = 0;
constexpr int32 SangeShadowClampMax = 7;

// ClampUtsusemiForSange mirrors std::clamp(getMod(UTSUSEMI), 0, 7).
inline auto ClampUtsusemiForSange(const int32 utsusemiMod) -> uint8
{
    const auto c = std::clamp(utsusemiMod, SangeShadowClampMin, SangeShadowClampMax);
    return static_cast<uint8>(c);
}

// CapHitCountByAmmoQuantity mirrors:
//   if (PAmmo && quantity < hitCount) hitCount = quantity
// When no ammo, quantity inject is ignored (hasAmmo false).
inline auto CapHitCountByAmmoQuantity(const uint8 hitCount, const bool hasAmmo, const uint8 ammoQuantity) -> uint8
{
    if (!hasAmmo)
    {
        return hitCount;
    }
    if (ammoQuantity < hitCount)
    {
        return ammoQuantity;
    }
    return hitCount;
}

// Multi-shot proc: roll0to99 < rate (xirand::GetRandomNumber(100) < mod).
inline auto MultiShotProcs(const int16 rate, const int roll0to99) -> bool
{
    return roll0to99 < rate;
}

// ShouldApplyCharBarrage mirrors isChar && !ammoThrowing && !rangedThrowing && isBarrage.
inline auto ShouldApplyCharBarrage(
    const bool isChar,
    const bool ammoThrowing,
    const bool rangedThrowing,
    const bool isBarrage) -> bool
{
    return isChar && !ammoThrowing && !rangedThrowing && isBarrage;
}

// ShouldApplyNonCharBarrage mirrors !isChar && isBarrage (mobs and trusts).
inline auto ShouldApplyNonCharBarrage(const bool isChar, const bool isBarrage) -> bool
{
    return !isChar && isBarrage;
}

// ShouldApplySange mirrors isChar && ammoThrowing && isSange
// (isSange already includes HasStatusEffect(Sange) && SANGE_MULTI_HIT > 0).
inline auto ShouldApplySange(const bool isChar, const bool ammoThrowing, const bool isSange) -> bool
{
    return isChar && ammoThrowing && isSange;
}

// IsSangeActive mirrors HasStatusEffect(Sange) && getMod(SANGE_MULTI_HIT) > 0.
inline auto IsSangeActive(const bool hasSangeEffect, const int16 sangeMultiHitMod) -> bool
{
    return hasSangeEffect && sangeMultiHitMod > 0;
}

// ShouldTryTripleShot mirrors (isChar || isTrust) && hasTripleShot.
// Host still rolls rate.
inline auto ShouldTryTripleShot(const bool isChar, const bool isTrust, const bool hasTripleShot) -> bool
{
    return (isChar || isTrust) && hasTripleShot;
}

// ShouldTryDoubleShot mirrors (isChar || isTrust) && hasDoubleShot.
inline auto ShouldTryDoubleShot(const bool isChar, const bool isTrust, const bool hasDoubleShot) -> bool
{
    return (isChar || isTrust) && hasDoubleShot;
}

// ResolveRangedHitCount is the exclusive if/else ladder for hitCount:
// 1) char non-throw barrage → base + barrage extras
// 2) non-char barrage → base + barrage extras
// 3) char ammo-throw sange → base + clamped shadows, cap by ammo qty
// 4) char/trust triple shot proc → 3
// 5) char/trust double shot proc → 2
// 6) else → base (1)
//
// barrageExtras is host-injected from getBarrageShotCount (level ladder).
// triple/double procs are host-injected after MultiShotProcs rolls.
inline auto ResolveRangedHitCount(
    const bool isChar,
    const bool isTrust,
    const bool ammoThrowing,
    const bool rangedThrowing,
    const bool isBarrage,
    const uint8 barrageExtras,
    const bool isSange,
    const int32 utsusemiMod,
    const bool hasAmmo,
    const uint8 ammoQuantity,
    const bool tripleShotProcs,
    const bool doubleShotProcs) -> uint8
{
    if (ShouldApplyCharBarrage(isChar, ammoThrowing, rangedThrowing, isBarrage) ||
        ShouldApplyNonCharBarrage(isChar, isBarrage))
    {
        return static_cast<uint8>(BaseRangedHitCount + barrageExtras);
    }
    if (ShouldApplySange(isChar, ammoThrowing, isSange))
    {
        auto hitCount = static_cast<uint8>(BaseRangedHitCount + ClampUtsusemiForSange(utsusemiMod));
        return CapHitCountByAmmoQuantity(hitCount, hasAmmo, ammoQuantity);
    }
    if (ShouldTryTripleShot(isChar, isTrust, true) && tripleShotProcs)
    {
        // hasTripleShot already folded into tripleShotProcs by host; pass true
        // only when effect present — tripleShotProcs is rate result.
        return TripleShotHitCount;
    }
    if (ShouldTryDoubleShot(isChar, isTrust, true) && doubleShotProcs)
    {
        return DoubleShotHitCount;
    }
    return BaseRangedHitCount;
}

// ResolveRangedHitCountWithEffects is the clearer host-facing form where
// triple/double require both effect present and rate proc.
inline auto ResolveRangedHitCountWithEffects(
    const bool isChar,
    const bool isTrust,
    const bool ammoThrowing,
    const bool rangedThrowing,
    const bool isBarrage,
    const uint8 barrageExtras,
    const bool isSange,
    const int32 utsusemiMod,
    const bool hasAmmo,
    const uint8 ammoQuantity,
    const bool hasTripleShot,
    const bool tripleRateProcs,
    const bool hasDoubleShot,
    const bool doubleRateProcs) -> uint8
{
    if (ShouldApplyCharBarrage(isChar, ammoThrowing, rangedThrowing, isBarrage) ||
        ShouldApplyNonCharBarrage(isChar, isBarrage))
    {
        return static_cast<uint8>(BaseRangedHitCount + barrageExtras);
    }
    if (ShouldApplySange(isChar, ammoThrowing, isSange))
    {
        auto hitCount = static_cast<uint8>(BaseRangedHitCount + ClampUtsusemiForSange(utsusemiMod));
        return CapHitCountByAmmoQuantity(hitCount, hasAmmo, ammoQuantity);
    }
    if (ShouldTryTripleShot(isChar, isTrust, hasTripleShot) && tripleRateProcs)
    {
        return TripleShotHitCount;
    }
    if (ShouldTryDoubleShot(isChar, isTrust, hasDoubleShot) && doubleRateProcs)
    {
        return DoubleShotHitCount;
    }
    return BaseRangedHitCount;
}

// ResolveThrowingSlot mirrors ammoThrowing → SLOT_AMMO else SLOT_RANGED.
// SLOT_RANGED = 2, SLOT_AMMO = 3 typically.
constexpr uint8 SlotRanged = 2;
constexpr uint8 SlotAmmo   = 3;

inline auto ResolveRangedWeaponSlot(const bool ammoThrowing) -> uint8
{
    return ammoThrowing ? SlotAmmo : SlotRanged;
}

// ShouldNullRangedWeaponOnAmmoThrow mirrors ammoThrowing → PItem = nullptr.
inline auto ShouldNullRangedWeaponOnAmmoThrow(const bool ammoThrowing) -> bool
{
    return ammoThrowing;
}

// ShouldNullAmmoOnRangedThrow mirrors rangedThrowing → PAmmo = nullptr.
inline auto ShouldNullAmmoOnRangedThrow(const bool rangedThrowing) -> bool
{
    return rangedThrowing;
}

} // namespace rangedhitcounthelpers
