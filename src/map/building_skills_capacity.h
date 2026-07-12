#pragma once

#include <cstdint>

// Pure BuildingCharSkillsTable policy from charutils.

namespace buildingskillshelpers
{

// Skill ID pins from battle_entity SKILLTYPE.
constexpr std::uint8_t SkillAutomatonMelee = 22;
constexpr std::uint8_t SkillAutomatonMagic = 24;
constexpr std::uint16_t SkillCappedBlueFlag = 0x8000;
constexpr std::uint16_t SkillUnusedFFFF     = 0xFFFF;

// IsUnusedCombatSkillSlot mirrors (13..21) || (46..47).
constexpr auto IsUnusedCombatSkillSlot(const std::int32_t skillID) -> bool
{
    return (skillID >= 13 && skillID <= 21) || (skillID >= 46 && skillID <= 47);
}

// IsAutomatonSkill mirrors 22..24.
constexpr auto IsAutomatonSkill(const std::int32_t skillID) -> bool
{
    return skillID >= SkillAutomatonMelee && skillID <= SkillAutomatonMagic;
}

// ResolveRealSkillRank prefers main rank when non-zero else sub.
constexpr auto ResolveRealSkillRank(const std::uint8_t mainSkillRank, const std::uint8_t subSkillRank) -> std::uint8_t
{
    return mainSkillRank != 0 ? mainSkillRank : subSkillRank;
}

// CapCurrentSkill mirrors current > max ? max : current.
constexpr auto CapCurrentSkill(const std::uint16_t currentSkill, const std::uint16_t maxSkill) -> std::uint16_t
{
    return currentSkill > maxSkill ? maxSkill : currentSkill;
}

// WorkingSkillFromCurrentAndBonus mirrors int16 newSkillValue = current + bonus
// (int16 assignment narrowing), then clamps negatives to 0.
constexpr auto WorkingSkillFromCurrentAndBonus(const std::uint16_t currentSkill, const std::int16_t skillBonus) -> std::uint16_t
{
    // Match C++: int16 newSkillValue = currentSkill + skillBonus;
    // sum promotes to int, then truncates into int16 on assignment.
    const auto sum           = static_cast<std::int32_t>(currentSkill) + static_cast<std::int32_t>(skillBonus);
    const auto newSkillValue = static_cast<std::int16_t>(sum);
    if (newSkillValue < 0)
    {
        return 0;
    }
    return static_cast<std::uint16_t>(newSkillValue);
}

// ShouldSetBlueCapFlag mirrors currentSkill >= maxSkill.
constexpr auto ShouldSetBlueCapFlag(const std::uint16_t currentSkill, const std::uint16_t maxSkill) -> bool
{
    return currentSkill >= maxSkill;
}

// WithBlueFlag ors 0x8000 onto a working skill value.
constexpr auto WithBlueFlag(const std::uint16_t workingSkill) -> std::uint16_t
{
    return static_cast<std::uint16_t>(workingSkill | SkillCappedBlueFlag);
}

// NonJobSkillWorkingValue mirrors max(0, skillBonus) | 0x8000.
constexpr auto NonJobSkillWorkingValue(const std::int16_t skillBonus) -> std::uint16_t
{
    auto bonus = skillBonus;
    if (bonus < 0)
    {
        bonus = 0;
    }
    return static_cast<std::uint16_t>(static_cast<std::uint16_t>(bonus) | SkillCappedBlueFlag);
}

// CraftWorkingSkill mirrors (real/10)*0x20 + rank.
constexpr auto CraftWorkingSkill(const std::uint16_t realSkillTenths, const std::uint8_t rank) -> std::uint16_t
{
    return static_cast<std::uint16_t>((realSkillTenths / 10) * 0x20 + rank);
}

// ShouldSetCraftBlueFlag mirrors (rank+1)*100 <= realSkillTenths.
constexpr auto ShouldSetCraftBlueFlag(const std::uint8_t rank, const std::uint16_t realSkillTenths) -> bool
{
    return static_cast<std::uint16_t>(rank + 1) * 100 <= realSkillTenths;
}

// IsCombatSkillLoopEnd mirrors skill ID loop 1..47 inclusive of continue ranges handled separately.
constexpr auto IsCombatSkillLoopID(const std::int32_t skillID) -> bool
{
    return skillID >= 1 && skillID < 48;
}

// IsCraftSkillLoopID mirrors 48..57.
constexpr auto IsCraftSkillLoopID(const std::int32_t skillID) -> bool
{
    return skillID >= 48 && skillID < 58;
}

// IsUnusedSkillLoopID mirrors 58..63.
constexpr auto IsUnusedSkillLoopID(const std::int32_t skillID) -> bool
{
    return skillID >= 58 && skillID < 64;
}

// MeritIndexForCombatSkill: skill IDs 1..47 skip unused slots still consume merit index only for used skills.
// Host advances meritIndex only on non-continue path; pure helper documents used-skill count path.
// IsMainJobSkillPath / IsSubJobSkillPath / IsNonJobSkillPath:
constexpr auto IsMainJobSkillPath(const std::uint16_t maxMainSkill) -> bool
{
    return maxMainSkill != 0;
}

constexpr auto IsSubJobSkillPath(const std::uint16_t maxMainSkill, const std::uint16_t maxSubSkill) -> bool
{
    return maxMainSkill == 0 && maxSubSkill != 0;
}

constexpr auto IsNonJobSkillPath(const std::uint16_t maxMainSkill, const std::uint16_t maxSubSkill) -> bool
{
    return maxMainSkill == 0 && maxSubSkill == 0;
}

// SkillModID mirrors skillID + 79.
constexpr auto SkillModID(const std::int32_t skillID) -> std::int32_t
{
    return skillID + 79;
}

// RealSkillLevels is RealSkills.skill / 10.
constexpr auto RealSkillLevels(const std::uint16_t realSkillTenths) -> std::uint16_t
{
    return static_cast<std::uint16_t>(realSkillTenths / 10);
}

} // namespace buildingskillshelpers
