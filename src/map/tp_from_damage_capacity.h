#pragma once

#include "tp_return_capacity.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure TP-from-damage paths after entity injects.
// Parity: internal/attackutils modified_delay_zanshin, store_tp, tp_return;
// internal/tpgain Physical (with Lua dAGI operator-precedence parity).

namespace tpfromdamagehelpers
{

constexpr int   H2HSingleSwingMinDelay = 96;
constexpr int   H2HFistMinDelay        = 48;
constexpr double DelayPMinMultiplier   = 0.85;
constexpr int   PhysicalMobBaseBonus   = 30;
constexpr double PhysicalNonMobShare   = 1.0 / 3.0;
constexpr int   SubtleBlowICap         = 50;
constexpr double SubtleBlowModMin      = 0.25;
constexpr double DAGIModMin            = 0.5;
constexpr double DAGIModMax            = 1.0;

enum class ModifiedDelayActor : std::uint8_t
{
    PC    = 0,
    Mob   = 1,
    Other = 2,
};

struct ModifiedDelayParams
{
    std::int32_t       delay{};
    bool               dualWield{};
    std::int32_t       dualWieldMod{};
    bool               usingH2H{};
    ModifiedDelayActor actor{ ModifiedDelayActor::PC };
    bool               subEquipped{};
    bool               h2hSkillRankZero{};
    std::int32_t       martialArtsMod{};
    std::int32_t       delayP{};
};

struct ModifiedDelayResult
{
    std::int32_t delay{};
    bool         canZanshin{};
};

inline auto GetModifiedDelayAndCanZanshin(const ModifiedDelayParams& p) -> ModifiedDelayResult
{
    double modifiedDelay = static_cast<double>(p.delay);
    bool   canZanshin    = false;

    if (p.dualWield)
    {
        modifiedDelay = (static_cast<double>(p.delay) * static_cast<double>(100 - p.dualWieldMod) / 100.0) / 2.0;
    }
    else if (p.usingH2H)
    {
        switch (p.actor)
        {
            case ModifiedDelayActor::PC:
                if (p.subEquipped || p.h2hSkillRankZero)
                {
                    modifiedDelay = std::max(static_cast<double>(p.delay - p.martialArtsMod),
                                             static_cast<double>(H2HSingleSwingMinDelay));
                    canZanshin    = true;
                }
                else
                {
                    modifiedDelay = std::max(static_cast<double>(p.delay - p.martialArtsMod) / 2.0,
                                             static_cast<double>(H2HFistMinDelay));
                }
                break;
            case ModifiedDelayActor::Mob:
                modifiedDelay = std::max(static_cast<double>(p.delay) / 2.0, static_cast<double>(H2HFistMinDelay));
                break;
            default:
                modifiedDelay = std::max(static_cast<double>(p.delay - p.martialArtsMod) / 2.0,
                                         static_cast<double>(H2HFistMinDelay));
                break;
        }
    }
    else
    {
        canZanshin = true;
    }

    const double delayPMult = std::max((100.0 + static_cast<double>(p.delayP)) / 100.0, DelayPMinMultiplier);
    modifiedDelay *= delayPMult;

    return ModifiedDelayResult{ static_cast<std::int32_t>(std::floor(modifiedDelay)), canZanshin };
}

// ApplyStoreTP: floor(baseTP * (100+storeTP)/100)
inline auto ApplyStoreTP(const std::int32_t baseTP, const std::int32_t storeTPMod) -> std::int32_t
{
    const double mod = (100.0 + static_cast<double>(storeTPMod)) / 100.0;
    return static_cast<std::int32_t>(std::floor(static_cast<double>(baseTP) * mod));
}

// Single melee hit TP (after Meikyo gate by caller or via hasMeikyo flag).
inline auto SingleMeleeHitTPReturn(const bool         hasMeikyo,
                                   const bool         isZanshin,
                                   const bool         usePCOrPetFormula,
                                   const std::int32_t modifiedDelay,
                                   const bool         canZanshin,
                                   const std::int32_t ikishotenMerit,
                                   const std::int32_t storeTPMod) -> std::int32_t
{
    if (hasMeikyo)
    {
        return 0;
    }
    auto tp = static_cast<std::int32_t>(tpreturnhelpers::CalculateTPReturn(usePCOrPetFormula, modifiedDelay));
    if (isZanshin && canZanshin)
    {
        tp += ikishotenMerit;
    }
    return ApplyStoreTP(tp, storeTPMod);
}

// Single ranged hit TP.
inline auto SingleRangedHitTPReturn(const bool         hasMeikyo,
                                    const bool         usePCOrPetFormula,
                                    const std::int32_t baseRangedDelay,
                                    const std::int32_t storeTPMod) -> std::int32_t
{
    if (hasMeikyo)
    {
        return 0;
    }
    if (baseRangedDelay <= 0)
    {
        return 0;
    }
    const auto tp = static_cast<std::int32_t>(tpreturnhelpers::CalculateTPReturn(usePCOrPetFormula, baseRangedDelay));
    return ApplyStoreTP(tp, storeTPMod);
}

// --- Physical TP gain on damage taken ---

// Lua-parity dAGI: clamp(200 - (dAGI+30)/200, 0.5, 1) — operator precedence
// matches tp.lua (almost always 1 for practical dAGI).
inline auto DAGIModifierLua(const std::int32_t dAGI) -> double
{
    auto v = 200.0 - (static_cast<double>(dAGI) + 30.0) / 200.0;
    if (v < DAGIModMin)
    {
        v = DAGIModMin;
    }
    if (v > DAGIModMax)
    {
        v = DAGIModMax;
    }
    return v;
}

constexpr auto InhibitTPModifier(const std::int32_t inhibitTP) -> double
{
    return (100.0 - static_cast<double>(inhibitTP)) / 100.0;
}

constexpr auto StoreTPModifier(const std::int32_t storeTP) -> double
{
    return 1.0 + static_cast<double>(storeTP) / 100.0;
}

constexpr auto SubtleBlowI(const std::int32_t subtleBlow, const std::int32_t merits) -> std::int32_t
{
    auto v = subtleBlow + merits;
    if (v > SubtleBlowICap)
    {
        v = SubtleBlowICap;
    }
    return v;
}

inline auto SubtleBlowModifier(const std::int32_t subtleBlowI, const std::int32_t subtleBlowII) -> double
{
    return std::max((100.0 - static_cast<double>(subtleBlowI) + static_cast<double>(subtleBlowII)) / 100.0,
                    SubtleBlowModMin);
}

struct PhysicalTPGainParams
{
    std::int32_t baseTPGain{};
    bool         targetIsMob{};
    bool         actorIsMob{};
    std::int32_t dAGI{};
    std::int32_t inhibitTP{};
    std::int32_t storeTP{}; // target
    std::int32_t subtleBlow{};
    std::int32_t subtleBlowMerit{};
    std::int32_t subtleBlowII{}; // includes tandem already or use tandem separate
    std::int32_t tandemBlowBonus{};
};

inline auto PhysicalTPGain(const PhysicalTPGainParams& p) -> std::int32_t
{
    const auto inhibit = InhibitTPModifier(p.inhibitTP);
    const auto sbi     = SubtleBlowI(p.subtleBlow, p.subtleBlowMerit);
    const auto sbii    = p.subtleBlowII + p.tandemBlowBonus;
    const auto subtle  = SubtleBlowModifier(sbi, sbii);
    const auto store   = StoreTPModifier(p.storeTP);

    if (p.targetIsMob && !p.actorIsMob)
    {
        const auto dagi = DAGIModifierLua(p.dAGI);
        return static_cast<std::int32_t>(std::floor(
            static_cast<double>(p.baseTPGain + PhysicalMobBaseBonus) * inhibit * dagi * subtle * store));
    }
    return static_cast<std::int32_t>(std::floor(
        static_cast<double>(p.baseTPGain) * inhibit * subtle * store * PhysicalNonMobShare));
}

// Early gates for physical TP gain: null/damage/meikyo handled by host.
constexpr auto ShouldZeroPhysicalTPGain(const bool nullEntity, const std::int32_t damage, const bool hasMeikyo) -> bool
{
    return nullEntity || damage <= 0 || hasMeikyo;
}

constexpr auto TandemBlowBonus(const bool tandemActive, const bool hasMasterPC, const std::int32_t masterPower, const std::int32_t selfPower)
    -> std::int32_t
{
    if (!tandemActive)
    {
        return 0;
    }
    if (hasMasterPC)
    {
        return masterPower;
    }
    return selfPower;
}

} // namespace tpfromdamagehelpers
