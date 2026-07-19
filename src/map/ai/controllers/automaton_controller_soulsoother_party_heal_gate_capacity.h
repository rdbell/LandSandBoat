#pragma once

namespace automatoncontrollersoulsootherpartyhealgate
{
inline auto CanConsiderPartyHealing(bool lightManeuver, bool noCastTarget, bool soulsootherHead, bool masterHasParty) -> bool
{
    return lightManeuver && noCastTarget && soulsootherHead && masterHasParty;
}
} // namespace automatoncontrollersoulsootherpartyhealgate
