#pragma once

#include <cstdint>

namespace trustcontrollercastcoordination
{

inline auto CanCast(bool candidateBuff, bool candidateCure, bool candidateDebuff, bool candidateNa,
                    uint16_t candidateFamily, uint16_t candidateID, uint16_t peerFamily, uint16_t peerID,
                    bool sameTarget, uint8_t targetHPP) -> bool
{
    const bool sameFamily = candidateFamily == peerFamily;
    if ((candidateBuff || candidateDebuff) && sameFamily && candidateID <= peerID)
    {
        return false;
    }
    if (candidateCure && sameTarget && targetHPP > 50)
    {
        return false;
    }
    return !(candidateNa && sameFamily && candidateID == peerID);
}

} // namespace trustcontrollercastcoordination
