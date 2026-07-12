#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure GetFSTR / melee fSTR / ranged fSTR2 after entity injects.
// Parity: internal/attackutils fstr.go
// Lua reference: xi.combat.physical.calculateMeleeStatFactor / calculateRangedStatFactor

namespace fstrhelpers
{

// SLOTTYPE pins (battle_entity.h)
constexpr std::uint8_t SlotMain   = 0x00;
constexpr std::uint8_t SlotSub    = 0x01;
constexpr std::uint8_t SlotRanged = 0x02;
constexpr std::uint8_t SlotAmmo   = 0x03;

enum class StatFactorActor : std::uint8_t
{
    PC  = 0, // players and trusts
    Mob = 1,
    Pet = 2,
};

constexpr auto ClampI32(const std::int32_t v, const std::int32_t lo, const std::int32_t hi) -> std::int32_t
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// llround half away from zero (sol2 integral conversion of Lua number).
inline auto LlroundI32(const double x) -> std::int32_t
{
    if (x >= 0.0)
    {
        return static_cast<std::int32_t>(std::floor(x + 0.5));
    }
    return static_cast<std::int32_t>(std::ceil(x - 0.5));
}

inline auto ClampF64(const double v, const double lo, const double hi) -> double
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// Player/trust piecewise addend before /4 (melee) or /2 (ranged).
constexpr auto PlayerStatDiffRaw(const std::int32_t statDiff) -> std::int32_t
{
    if (statDiff >= 12)
    {
        return statDiff + 4;
    }
    if (statDiff >= 6)
    {
        return statDiff + 6;
    }
    if (statDiff >= 1)
    {
        return statDiff + 7;
    }
    if (statDiff >= -2)
    {
        return statDiff + 8;
    }
    if (statDiff >= -7)
    {
        return statDiff + 9;
    }
    if (statDiff >= -15)
    {
        return statDiff + 10;
    }
    if (statDiff >= -21)
    {
        return statDiff + 12;
    }
    return statDiff + 13;
}

// Mob/pet melee ladder then floor (/4).
inline auto MobMeleeStatDiff(const std::int32_t statDiff) -> std::int32_t
{
    double f = 0.0;
    if (statDiff >= 36)
    {
        f = static_cast<double>(statDiff - 4) / 4.0;
    }
    else if (statDiff >= 26)
    {
        f = static_cast<double>(statDiff - 3) / 4.0;
    }
    else if (statDiff >= 17)
    {
        f = static_cast<double>(statDiff - 2) / 4.0;
    }
    else if (statDiff >= 4)
    {
        f = static_cast<double>(statDiff - 1) / 4.0;
    }
    else if (statDiff >= -8)
    {
        f = static_cast<double>(statDiff) / 4.0;
    }
    else if (statDiff >= -13)
    {
        f = static_cast<double>(statDiff + 1) / 4.0;
    }
    else if (statDiff >= -19)
    {
        f = static_cast<double>(statDiff + 3) / 4.0;
    }
    else if (statDiff >= -32)
    {
        f = static_cast<double>(statDiff + 4) / 4.0;
    }
    else if (statDiff >= -42)
    {
        f = static_cast<double>(statDiff + 5) / 4.0;
    }
    else if (statDiff >= -54)
    {
        f = static_cast<double>(statDiff + 6) / 4.0;
    }
    else if (statDiff >= -67)
    {
        f = static_cast<double>(statDiff + 7) / 4.0;
    }
    else if (statDiff >= -76)
    {
        f = static_cast<double>(statDiff + 8) / 4.0;
    }
    else
    {
        f = static_cast<double>(statDiff + 9) / 4.0;
    }
    return static_cast<std::int32_t>(std::floor(f));
}

// Mob/pet ranged ladder then floor (/2).
inline auto MobRangedStatDiff(const std::int32_t statDiff) -> std::int32_t
{
    double f = 0.0;
    if (statDiff >= 36)
    {
        f = static_cast<double>(statDiff - 4) / 2.0;
    }
    else if (statDiff >= 26)
    {
        f = static_cast<double>(statDiff - 3) / 2.0;
    }
    else if (statDiff >= 15)
    {
        f = static_cast<double>(statDiff - 2) / 2.0;
    }
    else if (statDiff >= 4)
    {
        f = static_cast<double>(statDiff - 1) / 2.0;
    }
    else if (statDiff >= -8)
    {
        f = static_cast<double>(statDiff) / 2.0;
    }
    else if (statDiff >= -16)
    {
        f = static_cast<double>(statDiff + 1) / 2.0;
    }
    else if (statDiff >= -31)
    {
        f = static_cast<double>(statDiff + 1) / 2.0;
    }
    else if (statDiff >= -42)
    {
        f = static_cast<double>(statDiff + 3) / 2.0;
    }
    else if (statDiff >= -53)
    {
        f = static_cast<double>(statDiff + 3) / 2.0;
    }
    else if (statDiff >= -64)
    {
        f = static_cast<double>(statDiff + 5) / 2.0;
    }
    else if (statDiff >= -76)
    {
        f = static_cast<double>(statDiff + 6) / 2.0;
    }
    else
    {
        f = static_cast<double>(statDiff + 7) / 2.0;
    }
    return static_cast<std::int32_t>(std::floor(f));
}

inline auto MeleeStatFactor(const StatFactorActor actor,
                            const std::int32_t    mainLvl,
                            const std::int32_t    attackerSTR,
                            const std::int32_t    defenderVIT,
                            const std::int32_t    weaponRank) -> std::int32_t
{
    if (actor == StatFactorActor::Mob && mainLvl <= 1)
    {
        return 1;
    }

    auto statDiff = attackerSTR - defenderVIT;

    if (actor == StatFactorActor::Mob || actor == StatFactorActor::Pet)
    {
        const auto fSTR  = MobMeleeStatDiff(statDiff);
        const auto lower = mainLvl / 5 - 1;
        const auto upper = mainLvl / 5 + 5;
        return ClampI32(fSTR, lower, upper);
    }

    // Players and trusts.
    const auto statLowerCap = (7 + weaponRank * 2) * -2;
    const auto statUpperCap = (14 + weaponRank * 2) * 2;
    statDiff                = ClampI32(statDiff, statLowerCap, statUpperCap);

    const auto raw   = PlayerStatDiffRaw(statDiff);
    const auto upper = weaponRank + 8;
    auto       lower = weaponRank * -1;
    if (weaponRank == 0)
    {
        lower = -1;
    }
    return LlroundI32(ClampF64(static_cast<double>(raw) / 4.0, static_cast<double>(lower), static_cast<double>(upper)));
}

inline auto RangedStatFactor(const StatFactorActor actor,
                             const std::int32_t    mainLvl,
                             const std::int32_t    attackerSTR,
                             const std::int32_t    defenderVIT,
                             const std::int32_t    weaponRank) -> std::int32_t
{
    if (actor == StatFactorActor::Mob && mainLvl <= 1)
    {
        return 1;
    }

    auto statDiff = attackerSTR - defenderVIT;

    if (actor == StatFactorActor::Mob || actor == StatFactorActor::Pet)
    {
        const auto fSTR = MobRangedStatDiff(statDiff);
        const auto ml   = static_cast<double>(mainLvl) / 5.0;
        const auto lower = static_cast<std::int32_t>(std::floor((ml - 1.0) * 2.0));
        const auto upper = static_cast<std::int32_t>(std::floor((ml + 5.0) * 2.0));
        return ClampI32(fSTR, lower, upper);
    }

    // Players and trusts.
    const auto statLowerCap = (7 + weaponRank * 2) * -2;
    const auto statUpperCap = (14 + weaponRank * 2) * 2;
    statDiff                = ClampI32(statDiff, statLowerCap, statUpperCap);

    const auto raw   = PlayerStatDiffRaw(statDiff);
    const auto upper = (weaponRank + 8) * 2;
    auto       lower = weaponRank * -2;
    if (weaponRank == 0)
    {
        lower = -2;
    }
    else if (weaponRank == 1)
    {
        lower = -3;
    }
    return LlroundI32(ClampF64(static_cast<double>(raw) / 2.0, static_cast<double>(lower), static_cast<double>(upper)));
}

// Slot dispatch matching battleutils::GetFSTR.
inline auto GetFSTR(const std::uint8_t    slot,
                    const StatFactorActor actor,
                    const std::int32_t    mainLvl,
                    const std::int32_t    attackerSTR,
                    const std::int32_t    defenderVIT,
                    const std::int32_t    weaponRank) -> std::int32_t
{
    switch (slot)
    {
        case SlotRanged:
        case SlotAmmo:
            return RangedStatFactor(actor, mainLvl, attackerSTR, defenderVIT, weaponRank);
        case SlotMain:
        case SlotSub:
            return MeleeStatFactor(actor, mainLvl, attackerSTR, defenderVIT, weaponRank);
        default:
            return 0;
    }
}

// Classify attacker for pure path selection.
constexpr auto ClassifyActor(const bool isMob, const bool isPet) -> StatFactorActor
{
    if (isMob)
    {
        return StatFactorActor::Mob;
    }
    if (isPet)
    {
        return StatFactorActor::Pet;
    }
    return StatFactorActor::PC;
}

} // namespace fstrhelpers
