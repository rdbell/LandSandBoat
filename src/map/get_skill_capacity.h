#pragma once

#include <cstdint>

// Pure CBattleEntity::GetSkill with fully injected inputs.
// Parity: internal/getskill (slice 1654).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::GetSkill (~2269–2278)
//
// Host retains WorkingSkills storage; helpers take SkillID and the pre-indexed
// WorkingSkills.skill[SkillID] entry (ignored when SkillID is out of range).
// The blue capped display bit (0x8000) is stripped via 0x7FFF.

namespace getskillhelpers
{

// MAX_SKILLTYPE from battle_entity.h.
constexpr std::uint16_t MaxSkillType = 64;

// Clears blue-capped text flag on WorkingSkills.skill entries.
// LSB: WorkingSkills.skill[SkillID] & 0x7FFF
constexpr std::uint16_t SkillValueMask = 0x7FFF;

// High bit set on capped WorkingSkills entries (blue skill text).
constexpr std::uint16_t SkillCappedBlueFlag = 0x8000;

// IsSkillIDInRange mirrors SkillID < MAX_SKILLTYPE.
constexpr auto IsSkillIDInRange(const std::uint16_t skillID) -> bool
{
    return skillID < MaxSkillType;
}

// MaskSkillValue mirrors WorkingSkills.skill[SkillID] & 0x7FFF.
constexpr auto MaskSkillValue(const std::uint16_t workingSkill) -> std::uint16_t
{
    return static_cast<std::uint16_t>(workingSkill & SkillValueMask);
}

// GetSkill mirrors CBattleEntity::GetSkill.
// workingSkill is the host-resolved WorkingSkills.skill[SkillID] when in range.
// Out-of-range skillID returns 0 without using workingSkill.
constexpr auto GetSkill(const std::uint16_t skillID, const std::uint16_t workingSkill) -> std::uint16_t
{
    if (!IsSkillIDInRange(skillID))
    {
        return 0;
    }
    return MaskSkillValue(workingSkill);
}

} // namespace getskillhelpers
