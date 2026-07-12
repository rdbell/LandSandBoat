#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CalculateEnspellDamage tails after Tier 1/2 base.

namespace enspelldamagetailshelpers
{

constexpr auto EnspellBonusFromExclude(const std::int32_t totalMod, const std::int32_t exclude) -> std::int32_t
{
    return totalMod - exclude;
}

// Returns damage; sets decayMod / removeStatus.
constexpr auto CalculateEnspellTier3Damage(const std::int32_t enspellDMG, bool& decayMod, bool& removeStatus) -> std::int32_t
{
    decayMod      = false;
    removeStatus  = false;
    if (enspellDMG > 1)
    {
        decayMod = true;
        return enspellDMG;
    }
    removeStatus = true;
    return enspellDMG;
}

inline auto EnspellRuneDPS(const double weaponDPS, const bool hasWeapon, const bool dualWield) -> double
{
    auto dps = hasWeapon ? weaponDPS : (3.0 / 240.0);
    if (dualWield)
    {
        dps /= 2.0;
    }
    return std::fmin(21.0, dps);
}

inline auto EnspellRuneMinMax(const double runeDPS, const int runeBonus, double& minOut, double& maxOut) -> void
{
    minOut = 0.0;
    maxOut = 0.0;
    switch (runeBonus)
    {
        case 1:
            minOut = std::floor(runeDPS * 0.97);
            maxOut = std::floor(runeDPS * 1.30);
            break;
        case 2:
            minOut = std::floor(runeDPS * 1.40);
            maxOut = std::floor(runeDPS * 1.70);
            break;
        case 3:
            minOut = std::floor(runeDPS * 1.90);
            maxOut = std::floor(runeDPS * 2.20);
            break;
        default:
            break;
    }
}

constexpr auto EnspellPctApplicable(const std::int32_t totalPct, const std::int32_t excludePct) -> std::int32_t
{
    return totalPct - excludePct;
}

constexpr auto EnspellNonWeaponPct(const std::int32_t pctApplicable, const std::int32_t weaponPct) -> std::int32_t
{
    const auto n = pctApplicable - weaponPct;
    return n < 0 ? 0 : n;
}

constexpr auto ShouldApplyComposureEnspellBonus(const bool isPC,
                                                const bool isRDMMain,
                                                const bool hasComposure,
                                                const std::uint8_t tier,
                                                const std::uint8_t element) -> bool
{
    return isPC && isRDMMain && hasComposure && (tier == 1 || tier == 2) && element >= 1 && element <= 6;
}

inline auto EnspellDamageMultiplier(const std::int32_t nonWeaponPct, const std::int32_t weaponPct, const bool composure) -> float
{
    auto mult = 1.0f + static_cast<float>(nonWeaponPct) / 100.0f;
    if (composure)
    {
        mult += 2.0f;
    }
    mult += static_cast<float>(weaponPct) / 100.0f;
    return mult;
}

inline auto ApplyEnspellDamageMultiplier(const std::int32_t damage, const float mult) -> std::int32_t
{
    return static_cast<std::int32_t>(std::floor(static_cast<float>(damage) * mult));
}

inline auto EnspellResistLadder(const std::int16_t meva, const double resvar) -> float
{
    const double half      = static_cast<double>(meva) / 100.0;
    const double quart     = std::pow(half, 2);
    const double eighth    = std::pow(half, 3);
    const double sixteenth = std::pow(half, 4);
    if (resvar <= sixteenth)
    {
        return 0.0625f;
    }
    if (resvar <= eighth)
    {
        return 0.125f;
    }
    if (resvar <= quart)
    {
        return 0.25f;
    }
    if (resvar <= half)
    {
        return 0.5f;
    }
    return 1.0f;
}

inline auto EnspellDayWeatherBonus(const bool  isPC,
                                   const float mobExtra,
                                   const std::int8_t dayArm,
                                   const std::int8_t weatherArm,
                                   const bool  dayChanceOK,
                                   const bool  weatherChanceOK) -> float
{
    auto d = 1.0f;
    if (!isPC)
    {
        d += mobExtra;
    }
    if (dayChanceOK)
    {
        if (dayArm == 1)
        {
            d += 0.1f;
        }
        else if (dayArm == -1)
        {
            d -= 0.1f;
        }
    }
    if (weatherChanceOK)
    {
        switch (weatherArm)
        {
            case 1:
                d += 0.1f;
                break;
            case 2:
                d += 0.25f;
                break;
            case -1:
                d -= 0.1f;
                break;
            case -2:
                d -= 0.25f;
                break;
            default:
                break;
        }
    }
    return d;
}

inline auto ApplyEnspellResistAndDayBonus(std::int32_t damage, const float resist, const float dBonus) -> std::int32_t
{
    damage = static_cast<std::int32_t>(static_cast<float>(damage) * resist);
    damage = static_cast<std::int32_t>(static_cast<float>(damage) * dBonus);
    return damage;
}

constexpr auto ClampEnspellFinalDamage(const std::int32_t damage) -> std::int32_t
{
    if (damage < -99999)
    {
        return -99999;
    }
    if (damage > 99999)
    {
        return 99999;
    }
    return damage;
}

} // namespace enspelldamagetailshelpers
