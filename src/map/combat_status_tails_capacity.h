#pragma once

#include <cstdint>

// Pure Afflatus Misery / Tactical TP / CalculateSpikeDamage pre-MDT helpers.

namespace combatstatustailshelpers
{

constexpr std::uint16_t AfflatusMiseryAccCap  = 30;
constexpr std::uint16_t AfflatusMiseryAccStep = 10;
constexpr std::uint8_t  SpikesEffectDread     = 3; // SPIKES::SPIKE_DREAD
constexpr std::uint8_t  ElementDark           = 8;

struct AfflatusMiseryAccResult
{
    bool          applied{ false };
    std::uint16_t newSubPower{ 0 };
    std::int16_t  accDelta{ 0 };
};

constexpr auto AfflatusMiseryAccuracyBonus(const bool          hasMisery,
                                           const bool          hasAuspice,
                                           const std::uint16_t subPower) -> AfflatusMiseryAccResult
{
    if (!hasMisery || !hasAuspice || subPower >= AfflatusMiseryAccCap)
    {
        return {};
    }
    return AfflatusMiseryAccResult{
        true,
        static_cast<std::uint16_t>(subPower + AfflatusMiseryAccStep),
        static_cast<std::int16_t>(AfflatusMiseryAccStep),
    };
}

constexpr auto ShouldSetAfflatusMiseryDamage(const bool hasMisery, const std::int32_t damage) -> bool
{
    return hasMisery && damage > 0;
}

constexpr auto TacticalTPBonus(const bool isPC, const bool hasTrait, const std::int16_t mod, std::int16_t& outTP) -> bool
{
    if (!isPC || !hasTrait)
    {
        outTP = 0;
        return false;
    }
    outTP = mod;
    return true;
}

struct SpikeDamageResult
{
    std::int32_t damage{ 0 };
    bool         clampElementToFire{ false };
};

inline auto CalculateSpikeDamagePreMDT(const std::int32_t spikesParam,
                                       const std::int16_t spikesDmgBonus,
                                       const bool         isDread,
                                       const std::uint16_t damageTaken,
                                       const bool         elementOutOfRange) -> SpikeDamageResult
{
    std::int32_t damage = spikesParam;
    if (spikesDmgBonus > 0)
    {
        damage = static_cast<std::int32_t>(static_cast<float>(damage) * (1.0f + static_cast<float>(spikesDmgBonus) / 100.0f));
    }
    if (isDread)
    {
        damage = static_cast<std::int32_t>(damageTaken);
    }
    return SpikeDamageResult{ damage, elementOutOfRange };
}

constexpr auto SpikesParamFromMod(const std::int16_t spikesDmg) -> std::int16_t
{
    return spikesDmg < 0 ? static_cast<std::int16_t>(0) : spikesDmg;
}

constexpr auto ShouldApplySpikeHealMessage(const std::int32_t damageAfterMDT) -> bool
{
    return damageAfterMDT < 0;
}

} // namespace combatstatustailshelpers
