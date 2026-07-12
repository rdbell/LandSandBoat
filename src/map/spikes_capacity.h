#pragma once

#include <algorithm>
#include <cstdint>

// Pure HandleSpikesDamage / Equip / Parry path helpers.

namespace spikeshelpers
{

enum class SpikesPath : std::uint8_t
{
    Retaliation = 0,
    SpellOrAuto,
    ItemGear,
    ClearNone,
};

constexpr auto ClassifySpikesPath(const bool retaliationProc, const bool hasSpikesEffect, const bool hasItemSubEffect) -> SpikesPath
{
    if (retaliationProc)
    {
        return SpikesPath::Retaliation;
    }
    if (hasSpikesEffect)
    {
        return SpikesPath::SpellOrAuto;
    }
    if (hasItemSubEffect)
    {
        return SpikesPath::ItemGear;
    }
    return SpikesPath::ClearNone;
}

constexpr auto RetaliationProc(const bool hasRetaliation, const bool engaged, const std::uint8_t hitRate, const int roll, const bool facing) -> bool
{
    return hasRetaliation && engaged && static_cast<int>(hitRate) / 2 > roll && facing;
}

constexpr auto RetaliationDamage(const std::uint16_t baseDmg, const std::int16_t retaliationMod) -> std::uint16_t
{
    const auto bonus = static_cast<std::uint16_t>(static_cast<float>(baseDmg) * (static_cast<float>(retaliationMod) / 100.0f));
    return static_cast<std::uint16_t>(baseDmg + bonus);
}

inline auto SpikesPacketParam(const std::int32_t spikesDamage, const std::int32_t maxHP, const std::int32_t currentHP) -> std::uint16_t
{
    if (spikesDamage < 0)
    {
        auto healed = -spikesDamage;
        auto room   = maxHP - currentHP;
        if (healed < 0)
        {
            healed = 0;
        }
        if (room < 0)
        {
            room = 0;
        }
        if (healed > room)
        {
            healed = room;
        }
        return static_cast<std::uint16_t>(healed);
    }
    return static_cast<std::uint16_t>(spikesDamage);
}

constexpr auto ShouldRunSpellSpikeSwitch(const bool defenderIsMob, const std::int16_t autoSpikesMod) -> bool
{
    return !defenderIsMob || autoSpikesMod == 0;
}

constexpr auto ShouldCallOnSpikesDamage(const bool defenderIsMob, const std::int16_t autoSpikesMod) -> bool
{
    return defenderIsMob && autoSpikesMod > 0;
}

constexpr auto DreadSpikesOnUndeadNull(const bool isDread, const bool attackerIsUndead) -> bool
{
    return isDread && attackerIsUndead;
}

constexpr auto ReprisalApplies(const bool isReprisal, const bool resolutionIsBlock) -> bool
{
    if (!isReprisal)
    {
        return true;
    }
    return resolutionIsBlock;
}

constexpr auto SpikesEquipLevelDiff(const std::uint8_t defenderLevel, const std::uint8_t attackerLevel) -> int
{
    int diff = static_cast<int>(defenderLevel) - static_cast<int>(attackerLevel);
    if (diff < -5)
    {
        diff = -5;
    }
    if (diff > 5)
    {
        diff = 5;
    }
    return diff * 2;
}

constexpr auto SpikesEquipProcs(const std::uint8_t chance, const int lvlDiff, const int roll) -> bool
{
    return roll < static_cast<int>(chance) + lvlDiff;
}

constexpr auto SpikesEquipRatio(const std::uint8_t damage) -> std::uint8_t
{
    int r = static_cast<int>(damage) / 4;
    if (r < 1)
    {
        return 1;
    }
    if (r > 255)
    {
        return 255;
    }
    return static_cast<std::uint8_t>(r);
}

constexpr auto ItemSubEffectIsSpikeType(const std::int16_t spikesType) -> bool
{
    return spikesType > 0 && spikesType < 7;
}

constexpr auto ShouldClaimOnSpikeKill(const bool attackerDead) -> bool
{
    return attackerDead;
}

constexpr auto ParrySpikesActive(const bool spikesEffectNone) -> bool
{
    return !spikesEffectNone;
}

} // namespace spikeshelpers
