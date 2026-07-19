#pragma once

namespace automatoncontrollerstormwakericeelementalpriority
{
inline auto ShouldPrioritizeElemental(bool iceManeuver, bool lowHP) -> bool
{
    return iceManeuver && !lowHP;
}
} // namespace automatoncontrollerstormwakericeelementalpriority
