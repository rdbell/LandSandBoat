#pragma once

#include <algorithm>
#include <cstdint>

// Pure battleutils::CalculateWeaponSkillTP after equip/mod injects.
// Parity: internal/weaponskill.CalculateWeaponSkillTP

namespace wstphelpers
{

constexpr std::uint16_t RangedWeaponskillIDMin = 192;
constexpr std::uint16_t RangedWeaponskillIDMax = 221;
constexpr std::int16_t  MaxWeaponskillTP       = 3000;

constexpr auto IsRangedWeaponskillID(const std::uint16_t id) -> bool
{
    return id >= RangedWeaponskillIDMin && id <= RangedWeaponskillIDMax;
}

constexpr auto FencerEligible(const bool mainPresent,
                              const bool mainTwoHanded,
                              const bool mainHandToHand,
                              const bool subPresent,
                              const bool subIsWeapon,
                              const std::uint8_t subSkillType,
                              const bool subIsShield) -> bool
{
    if (!mainPresent || mainTwoHanded || mainHandToHand)
    {
        return false;
    }
    if (!subPresent)
    {
        return true;
    }
    if (subIsShield)
    {
        return true;
    }
    if (subIsWeapon && subSkillType == 0)
    {
        return true;
    }
    return false;
}

constexpr auto CalculateWeaponSkillTP(const std::int16_t spentTP,
                                      const std::int16_t tpBonus,
                                      const bool         isPC,
                                      const bool         isRangedWS,
                                      const std::int16_t subTPBonus,
                                      const std::int16_t mainTPBonus,
                                      const std::int16_t rangedTPBonus,
                                      const bool         fencerEligible,
                                      const std::int16_t fencerTPBonus) -> std::int16_t
{
    auto tp = static_cast<std::int16_t>(spentTP + tpBonus);
    if (isPC)
    {
        tp = static_cast<std::int16_t>(tp - subTPBonus);
        if (isRangedWS)
        {
            tp = static_cast<std::int16_t>(tp - mainTPBonus);
        }
        else
        {
            tp = static_cast<std::int16_t>(tp - rangedTPBonus);
            if (fencerEligible)
            {
                tp = static_cast<std::int16_t>(tp + fencerTPBonus);
            }
        }
    }
    return std::min(tp, MaxWeaponskillTP);
}

} // namespace wstphelpers
