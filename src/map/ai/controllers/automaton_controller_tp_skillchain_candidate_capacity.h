#pragma once

namespace automatoncontrollertpskillchaincandidate
{
inline auto ShouldSelectTPSkillchainCandidate(int candidateSkill, int currentSkill, bool formsSkillchain) -> bool
{
    return candidateSkill > currentSkill && formsSkillchain;
}
} // namespace automatoncontrollertpskillchaincandidate
