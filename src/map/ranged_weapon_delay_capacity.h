#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

namespace rangedweapondelayhelpers
{

// ResolveRangedBaseDelay mirrors GetRangedWeaponDelay's base delay selection.
// hasRangeWeapon with damage, hasAmmo with damage, forTPCalc controls ammo add.
// rangeBaseDelay / ammoBaseDelay are CItemWeapon::getBaseDelay injects.
inline auto ResolveRangedBaseDelay(
    const bool hasRangeWeapon,
    const bool rangeHasDamage,
    const int16 rangeBaseDelay,
    const bool hasAmmo,
    const bool ammoHasDamage,
    const int16 ammoBaseDelay,
    const bool forTPCalc) -> int16
{
    if (hasRangeWeapon && rangeHasDamage)
    {
        int16 delay = rangeBaseDelay;
        if (hasAmmo && forTPCalc)
        {
            delay = static_cast<int16>(delay + ammoBaseDelay);
        }
        return delay;
    }
    if (hasAmmo && ammoHasDamage)
    {
        return ammoBaseDelay;
    }
    return 0;
}

// ApplyRangedDelayFlatMod mirrors (delay - getMod(RANGED_DELAY)) * 1000 / 120.
// Integer division matches C++ int16 arithmetic.
inline auto ApplyRangedDelayFlatMod(const int16 baseDelay, const int16 rangedDelayMod) -> int16
{
    return static_cast<int16>((baseDelay - rangedDelayMod) * 1000 / 120);
}

// ApplyRangedDelayP mirrors delay * ((100.0f + getMod(RANGED_DELAYP)) / 100.0f)
// for non-TP (animation) path. Truncates toward zero like C++ float→int16.
inline auto ApplyRangedDelayP(const int16 delayMs, const int16 rangedDelayP) -> int16
{
    const float scale = (100.0f + static_cast<float>(rangedDelayP)) / 100.0f;
    return static_cast<int16>(static_cast<float>(delayMs) * scale);
}

// ResolveRangedWeaponDelay is the full GetRangedWeaponDelay pure formula.
// forTPCalc skips RANGED_DELAYP (TP calc uses flat path only).
inline auto ResolveRangedWeaponDelay(
    const bool hasRangeWeapon,
    const bool rangeHasDamage,
    const int16 rangeBaseDelay,
    const bool hasAmmo,
    const bool ammoHasDamage,
    const int16 ammoBaseDelay,
    const int16 rangedDelayMod,
    const int16 rangedDelayP,
    const bool forTPCalc) -> int16
{
    const auto base = ResolveRangedBaseDelay(
        hasRangeWeapon,
        rangeHasDamage,
        rangeBaseDelay,
        hasAmmo,
        ammoHasDamage,
        ammoBaseDelay,
        forTPCalc);
    auto delay = ApplyRangedDelayFlatMod(base, rangedDelayMod);
    if (!forTPCalc)
    {
        delay = ApplyRangedDelayP(delay, rangedDelayP);
    }
    return delay;
}

// ResolveAmmoDelay mirrors GetAmmoDelay: ammo delay / 2 when ammo has damage.
inline auto ResolveAmmoDelay(const bool hasAmmo, const bool ammoHasDamage, const int16 ammoDelay) -> int16
{
    if (hasAmmo && ammoHasDamage)
    {
        return static_cast<int16>(ammoDelay / 2);
    }
    return 0;
}

} // namespace rangedweapondelayhelpers
