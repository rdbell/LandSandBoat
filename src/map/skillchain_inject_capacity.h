#pragma once

#include "modifier.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

// Pure skillchain multiplier injects (nullify/absorb/staff/affinity/magic-taken/day-weather).
// Parity: internal/absorbnull, internal/spellelem, internal/dmgmultiplier, internal/dayweather

namespace skillchaininjecthelpers
{

constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementDark    = 8;
constexpr std::uint8_t ElementWater   = 6;

// Staff: 1 + mod * 5/100
constexpr double StaffBonusStep = 0.05;

// Staff mod IDs FIRE..DARK
constexpr Mod kStaffMods[8] = {
    Mod::FIRE_STAFF_BONUS, Mod::ICE_STAFF_BONUS, Mod::WIND_STAFF_BONUS, Mod::EARTH_STAFF_BONUS,
    Mod::THUNDER_STAFF_BONUS, Mod::WATER_STAFF_BONUS, Mod::LIGHT_STAFF_BONUS, Mod::DARK_STAFF_BONUS,
};

// Affinity MAB mod IDs FIRE..DARK
constexpr Mod kAffinityMods[8] = {
    Mod::FIRE_MAB, Mod::ICE_MAB, Mod::WIND_MAB, Mod::EARTH_MAB,
    Mod::THUNDER_MAB, Mod::WATER_MAB, Mod::LIGHT_MAB, Mod::DARK_MAB,
};

// Elemental absorb / null mods FIRE..DARK
constexpr Mod kAbsorbMods[8] = {
    Mod::FIRE_ABSORB, Mod::ICE_ABSORB, Mod::WIND_ABSORB, Mod::EARTH_ABSORB,
    Mod::LTNG_ABSORB, Mod::WATER_ABSORB, Mod::LIGHT_ABSORB, Mod::DARK_ABSORB,
};
constexpr Mod kNullMods[8] = {
    Mod::FIRE_NULL, Mod::ICE_NULL, Mod::WIND_NULL, Mod::EARTH_NULL,
    Mod::LTNG_NULL, Mod::WATER_NULL, Mod::LIGHT_NULL, Mod::DARK_NULL,
};

// Force DW bonus mods FIRE..DARK
constexpr Mod kForceDWBonusMods[8] = {
    Mod::FORCE_FIRE_DWBONUS, Mod::FORCE_ICE_DWBONUS, Mod::FORCE_WIND_DWBONUS, Mod::FORCE_EARTH_DWBONUS,
    Mod::FORCE_LIGHTNING_DWBONUS, Mod::FORCE_WATER_DWBONUS, Mod::FORCE_LIGHT_DWBONUS, Mod::FORCE_DARK_DWBONUS,
};

inline auto StaffBonus(const std::uint8_t element, const std::int16_t staffMod) -> double
{
    if (element == ElementNone)
    {
        return 1.0;
    }
    return 1.0 + static_cast<double>(staffMod) * StaffBonusStep;
}

inline auto AffinityBonus(const std::uint8_t element, const std::int16_t affinityMAB) -> double
{
    if (element == ElementNone)
    {
        return 1.0;
    }
    return 1.0 + static_cast<double>(affinityMAB) / 100.0;
}

// NullificationFactor after proc injects (skillchain: isMagic=true, isBreath=false).
inline auto NullificationFactor(const bool nullAll, const bool nullMagic, const bool nullElement) -> double
{
    if (nullAll || nullMagic || nullElement)
    {
        return 0.0;
    }
    return 1.0;
}

// AbsorptionFactor after Liement + proc injects.
inline auto AbsorptionFactor(const double liementFactor, const bool absorbAll, const bool absorbMagic, const bool absorbElement) -> double
{
    if (liementFactor < 0.0)
    {
        return liementFactor;
    }
    if (absorbAll || absorbMagic || absorbElement)
    {
        return -1.0;
    }
    return 1.0;
}

// Magic damage adjustment (isMagical only) for skillchain.
inline auto MagicalDamageAdjustment(const std::int32_t dmg, const std::int32_t dmgMagic, const std::int32_t dmgMagicII, const std::int32_t udmgMagic) -> double
{
    const double global = static_cast<double>(dmg) / 10000.0;
    const double magicI = static_cast<double>(dmgMagic) / 10000.0;
    const double magicII = static_cast<double>(dmgMagicII) / 10000.0;
    const double magicU  = static_cast<double>(udmgMagic) / 10000.0;

    auto clamp = [](double v, double lo, double hi) {
        return std::max(lo, std::min(hi, v));
    };
    const double combined = clamp(global + magicI, -0.5, 0.5);
    double       mult     = clamp(1.0 + combined + magicII, 0.125, 1.875);
    mult                  = clamp(mult + magicU, 0.0, 2.0);
    return mult;
}

// --- Day/weather (subset of dayweather.Multiplier) ---

constexpr double SingleWeatherStep = 0.10;
constexpr double DoubleWeatherStep = 0.25;
constexpr double DayStep           = 0.10;
constexpr double IridescenceStep   = 0.05;
constexpr double DayWeatherMultMin = 0.0;
constexpr double DayWeatherMultMax = 1.4;

// Weakness, single weather, double weather per element 1..8
struct ElementWeatherRow
{
    std::uint8_t weakness{};
    std::uint8_t single{};
    std::uint8_t doubleW{};
};

// Mirrors dayweather elementTable (weather ids match xi.weather).
constexpr ElementWeatherRow kElementWeather[9] = {
    {},
    { 6, 4, 5 },   // Fire → Water, HotSpell, HeatWave
    { 1, 12, 13 }, // Ice
    { 2, 10, 11 }, // Wind
    { 3, 8, 9 },   // Earth
    { 4, 14, 15 }, // Thunder
    { 5, 6, 7 },   // Water
    { 8, 16, 17 }, // Light
    { 7, 18, 19 }, // Dark
};

struct DayWeatherParams
{
    std::uint8_t spellElement{};
    std::uint8_t weather{};
    std::uint8_t dayElement{};
    bool         alwaysApply{};
    bool         randomProc{};
    bool         forceDWBonusPenalty{};
    bool         forceElementBonus{};
    int          iridescence{};
    int          dayWeatherProcBonus{};
    int          dayNukeBonus{};
};

inline auto DayWeatherMultiplier(const DayWeatherParams& p) -> double
{
    double mult = 1.0;
    if (p.spellElement == ElementNone || p.spellElement > ElementDark)
    {
        return mult;
    }

    bool applyBonuses   = false;
    bool applyPenalties = false;
    if (p.alwaysApply || p.randomProc || p.forceDWBonusPenalty)
    {
        applyBonuses   = true;
        applyPenalties = true;
    }
    else if (p.forceElementBonus)
    {
        applyBonuses = true;
    }

    const auto& row = kElementWeather[p.spellElement];
    const double iri = static_cast<double>(p.iridescence) * IridescenceStep;
    const int weather = static_cast<int>(p.weather);

    if (applyBonuses)
    {
        if (weather == static_cast<int>(row.single))
        {
            mult += SingleWeatherStep + iri;
        }
        else if (weather == static_cast<int>(row.doubleW))
        {
            mult += DoubleWeatherStep + iri;
        }
        if (p.dayElement == p.spellElement)
        {
            mult += DayStep;
        }
        if (weather == static_cast<int>(row.single) || weather == static_cast<int>(row.doubleW) || p.dayElement == p.spellElement)
        {
            mult += static_cast<double>(p.dayWeatherProcBonus) / 100.0;
        }
    }
    if (applyPenalties)
    {
        // Opposite weathers: look up weakness element's weather.
        const auto& weak = kElementWeather[row.weakness];
        if (weather == static_cast<int>(weak.single))
        {
            mult -= SingleWeatherStep + iri;
        }
        else if (weather == static_cast<int>(weak.doubleW))
        {
            mult -= DoubleWeatherStep + iri;
        }
        if (p.dayElement == row.weakness)
        {
            mult -= DayStep;
        }
    }
    if (p.spellElement <= ElementWater && p.spellElement == p.dayElement)
    {
        mult += static_cast<double>(p.dayNukeBonus) / 100.0;
    }
    return std::max(DayWeatherMultMin, std::min(DayWeatherMultMax, mult));
}

inline auto StaffModForElement(const std::uint8_t element) -> Mod
{
    if (element < ElementFire || element > ElementDark)
    {
        return Mod::NONE;
    }
    return kStaffMods[element - ElementFire];
}

inline auto AffinityModForElement(const std::uint8_t element) -> Mod
{
    if (element < ElementFire || element > ElementDark)
    {
        return Mod::NONE;
    }
    return kAffinityMods[element - ElementFire];
}

inline auto AbsorbModForElement(const std::uint8_t element) -> Mod
{
    if (element < ElementFire || element > ElementDark)
    {
        return Mod::NONE;
    }
    return kAbsorbMods[element - ElementFire];
}

inline auto NullModForElement(const std::uint8_t element) -> Mod
{
    if (element < ElementFire || element > ElementDark)
    {
        return Mod::NONE;
    }
    return kNullMods[element - ElementFire];
}

inline auto ForceDWBonusModForElement(const std::uint8_t element) -> Mod
{
    if (element < ElementFire || element > ElementDark)
    {
        return Mod::NONE;
    }
    return kForceDWBonusMods[element - ElementFire];
}

// Lua math.random(1,100) <= chance  (chance from getMod)
inline auto RollProc1to100(const int roll1to100, const std::int16_t chance) -> bool
{
    if (chance <= 0)
    {
        return false;
    }
    return roll1to100 <= static_cast<int>(chance);
}

} // namespace skillchaininjecthelpers
