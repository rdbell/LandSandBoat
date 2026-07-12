#pragma once

#include <cstdint>

// Pure BaitLoss / RodBreak / CanFishMob gates from fishingutils.
// Parity: internal/fishingutils/bait_rod.go (slice 1629).

namespace fishingbaitrodhelpers
{

// ITEM_WEAPON (items/item.h).
constexpr std::uint8_t ItemTypeWeapon = 0x10;

// SKILL_FISHING (battle_entity.h).
constexpr std::uint8_t SkillFishing = 48;

// FISHINGSUCCESSTYPE_CATCHITEM.
constexpr std::uint8_t FishingSuccessCatchItem = 1;

// STATUS_TYPE::DISAPPEAR.
constexpr std::uint8_t StatusDisappear = 2;

enum class BaitLossReject : std::uint8_t
{
    OK               = 0,
    NotWeapon        = 1,
    NotFishingSkill  = 2,
    KeepFly          = 3, // !removeFly && stackSize == 1
};

// Pure BaitLoss validation when bait equipment fields are injected.
// hasBait false → OK (LSB returns true with no mutation).
inline auto ClassifyBaitLoss(const bool hasBait, const bool isWeaponType, const std::uint8_t skillType, const bool removeFly, const std::uint8_t stackSize)
    -> BaitLossReject
{
    if (!hasBait)
    {
        return BaitLossReject::OK;
    }
    if (!isWeaponType)
    {
        return BaitLossReject::NotWeapon;
    }
    if (skillType != SkillFishing)
    {
        return BaitLossReject::NotFishingSkill;
    }
    if (!removeFly && stackSize == 1)
    {
        return BaitLossReject::KeepFly;
    }
    return BaitLossReject::OK;
}

// Consume bait when hookedFish present and success is not CATCHITEM.
inline auto BaitLossShouldConsume(const bool hasHookedFish, const std::uint8_t successType) -> bool
{
    return hasHookedFish && successType != FishingSuccessCatchItem;
}

inline auto BaitLossShouldUnequip(const std::uint8_t quantity) -> bool
{
    return quantity == 1;
}

// RodBreak catalog gate: breakable && brokenRodId > 0.
inline auto ShouldBreakRod(const bool breakable, const std::uint16_t brokenRodId) -> bool
{
    return breakable && brokenRodId > 0;
}

// Pure CanFishMob with entity injects.
inline auto CanFishMob(const bool hasMob, const bool isAlive, const std::uint8_t status, const std::int32_t hookedVar) -> bool
{
    if (!hasMob)
    {
        return false;
    }
    if (isAlive)
    {
        return false;
    }
    if (status != StatusDisappear)
    {
        return false;
    }
    if (hookedVar != 1)
    {
        return false;
    }
    return true;
}

} // namespace fishingbaitrodhelpers
