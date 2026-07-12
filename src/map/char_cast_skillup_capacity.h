#pragma once

#include <cstdint>

// Pure ranged-instrument skill-up eligibility from CCharEntity::OnCastFinished.

namespace charcastskilluphelpers
{

constexpr std::uint8_t SkillSinging          = 40;
constexpr std::uint8_t SkillStringInstrument = 41;
constexpr std::uint8_t SkillWindInstrument   = 42;
constexpr std::uint8_t SkillGeomancy         = 44;
constexpr std::uint8_t SkillHandbell         = 45;

// ShouldSkillUpRanged reports whether the equipped ranged skill should TrySkillUP
// alongside the spell skill, given spell skill type and ranged item skill type.
constexpr auto ShouldSkillUpRanged(const std::uint8_t spellSkill, const std::uint8_t rangedSkill) -> bool
{
    switch (spellSkill)
    {
        case SkillGeomancy:
            return rangedSkill == SkillHandbell;
        case SkillSinging:
            return rangedSkill == SkillStringInstrument || rangedSkill == SkillWindInstrument || rangedSkill == SkillSinging;
        default:
            return false;
    }
}

} // namespace charcastskilluphelpers
