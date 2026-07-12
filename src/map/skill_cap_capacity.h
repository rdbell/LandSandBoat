#pragma once

#include <algorithm>
#include <cstdint>

// Pure GetMaxSkill level clamps after skill_caps table inject.
// Parity: internal/skillcap Max / MaxByRank level policy.
// Host still owns g_SkillTable / g_SkillRanks data loaded from DB.

namespace skillcaphelpers
{

// Soft research cap: levels above 99 use the 99 row (LSB TODO on 99+).
constexpr std::uint8_t SoftMaxSkillLevel = 99;

// CapLevelForSkillTable applies the hard 99 soft-cap used by GetMaxSkill(skill,job,level).
constexpr auto CapLevelForSkillTable(std::uint8_t level) -> std::uint8_t
{
    if (level > SoftMaxSkillLevel)
    {
        return SoftMaxSkillLevel;
    }
    return level;
}

// ClampSkillTableLevel indexes skill_caps: clamp to [0, maxLevel] where
// maxLevel = tableSize-1 (0-indexed table).
constexpr auto ClampSkillTableLevel(const std::uint8_t level, const std::uint8_t maxLevel) -> std::uint8_t
{
    if (level > maxLevel)
    {
        return maxLevel;
    }
    return level;
}

// ResolveSkillTableLevel: soft-cap then clamp to table size.
// applySoftCap mirrors GetMaxSkill(skill,job,level) only (not the rank overload).
constexpr auto ResolveSkillTableLevel(const std::uint8_t level,
                                      const std::uint8_t maxLevel,
                                      const bool         applySoftCap99) -> std::uint8_t
{
    auto lvl = level;
    if (applySoftCap99)
    {
        lvl = CapLevelForSkillTable(lvl);
    }
    return ClampSkillTableLevel(lvl, maxLevel);
}

// ExceedsTable warns when original level exceeds maxLevel (after soft-cap for skill overload).
constexpr auto LevelExceedsSkillTable(const std::uint8_t levelAfterSoftCap, const std::uint8_t maxLevel) -> bool
{
    return levelAfterSoftCap > maxLevel;
}

} // namespace skillcaphelpers
