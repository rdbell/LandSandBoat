#pragma once

namespace automatoncontrollersoulsootherpartystatusremovalgate
{
inline auto CanConsiderPartyStatusRemoval(bool waterManeuver, bool soulsootherHead, bool masterHasParty) -> bool
{
    return waterManeuver && soulsootherHead && masterHasParty;
}
} // namespace automatoncontrollersoulsootherpartystatusremovalgate
