#pragma once

namespace automatoncontrollerspiritreaverabsorbintcandidate
{
inline auto CanSelectCandidate(bool darkManeuver, bool hasIntBoost) -> bool
{
    return darkManeuver && !hasIntBoost;
}
} // namespace automatoncontrollerspiritreaverabsorbintcandidate
