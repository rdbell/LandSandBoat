#pragma once

namespace automatoncontrollerstormwakerelementalfallback
{
inline auto ShouldUseElementalFallback(bool lowHP, bool iceManeuver) -> bool
{
    return !lowHP && !iceManeuver;
}
} // namespace automatoncontrollerstormwakerelementalfallback
