#pragma once

namespace automatoncontrollertpskillpriority
{
inline auto ShouldSelectTPSkill(int candidateManeuvers, int currentManeuvers, int candidateSkill, int currentSkill) -> bool
{
    return candidateManeuvers > -1 && (candidateManeuvers > currentManeuvers || (candidateManeuvers == currentManeuvers && candidateSkill > currentSkill));
}
} // namespace automatoncontrollertpskillpriority
