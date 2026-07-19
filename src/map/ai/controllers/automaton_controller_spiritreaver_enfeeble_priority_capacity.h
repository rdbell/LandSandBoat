#pragma once

namespace automatoncontrollerspiritreaverenfeeblepriority
{
inline auto ShouldPrioritizeEnfeeble(bool darkManeuver, int hpp, int mpp) -> bool
{
    return darkManeuver || hpp < 75 || mpp < 75;
}
} // namespace automatoncontrollerspiritreaverenfeeblepriority
