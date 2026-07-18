#pragma once

#include <cstdint>

namespace trustcontrollerengage
{

constexpr uint16_t CorneliaModelID = 3119;

inline auto ShouldEngage(bool masterEngaged, bool masterHasTarget, bool masterMeleeSwing, int32_t engageType, uint16_t modelID) -> bool
{
    if (!masterEngaged || !masterHasTarget || modelID == CorneliaModelID)
    {
        return false;
    }

    if (engageType == 1)
    {
        return true;
    }

    return masterMeleeSwing;
}

} // namespace trustcontrollerengage
