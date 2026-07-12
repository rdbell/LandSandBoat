#pragma once

#include <cstdint>

// Pure getBarrageShotCount level ladder and PC skill/ammo clamps.

namespace barragehelpers
{

constexpr std::uint8_t JobRNG            = 11;
constexpr std::uint8_t SkillArchery      = 25;
constexpr std::uint8_t SkillMarksmanship = 26;

constexpr auto BarrageShotCount(const std::uint16_t level) -> std::uint8_t
{
    if (level < 30)
    {
        return 0;
    }
    if (level < 50)
    {
        return 3;
    }
    if (level < 75)
    {
        return 4;
    }
    if (level < 90)
    {
        return 5;
    }
    if (level < 99)
    {
        return 6;
    }
    return 7;
}

constexpr auto BarrageLevelForJob(const std::uint8_t mainJob, const std::uint8_t mLevel, const std::uint8_t sLevel) -> std::uint16_t
{
    return mainJob == JobRNG ? static_cast<std::uint16_t>(mLevel) : static_cast<std::uint16_t>(sLevel);
}

constexpr auto BarrageSkillOK(const bool hasRangedWeapon, const std::uint8_t skillType) -> bool
{
    if (!hasRangedWeapon)
    {
        return true;
    }
    return skillType == SkillArchery || skillType == SkillMarksmanship;
}

constexpr auto BarrageAmmoClamp(const std::uint8_t shotCount, const bool hasAmmo, const std::uint8_t ammoQuantity) -> std::uint8_t
{
    if (!hasAmmo)
    {
        return shotCount;
    }
    const auto need = static_cast<std::uint16_t>(shotCount) + 1;
    if (static_cast<std::uint16_t>(ammoQuantity) < need)
    {
        return ammoQuantity == 0 ? static_cast<std::uint8_t>(0) : static_cast<std::uint8_t>(ammoQuantity - 1);
    }
    return shotCount;
}

} // namespace barragehelpers
