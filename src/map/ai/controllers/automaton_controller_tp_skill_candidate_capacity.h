#pragma once

namespace automatoncontrollertpskillcandidate
{
inline auto CanUseTPSkillCandidate(bool skillFound, int automatonSkill, int requiredSkill, float distance, float radius) -> bool
{
    return skillFound && automatonSkill > requiredSkill && requiredSkill != -1 && distance < radius;
}
} // namespace automatoncontrollertpskillcandidate
