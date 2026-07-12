#pragma once

#include <cstdint>

// Pure xi.trust.checkBattlefieldTrustCount inject form.
// Parity: internal/trusthelpers CheckBattlefieldTrustCount (slice ~1145+).
// Production wire: slice 1584.

namespace trustbattlefieldhelpers
{

constexpr int RoVKIMaxParticipants = 6;

// rovKIBattlefieldIDs (LB5; GEO/RUN TODO upstream).
constexpr bool IsRoVKIBattlefield(const std::uint16_t id)
{
    switch (id)
    {
        case 5:
        case 6:
        case 7:
        case 70:
        case 71:
        case 72:
        case 101:
        case 102:
        case 103:
        case 163:
        case 194:
        case 195:
        case 196:
        case 517:
        case 518:
        case 519:
        case 530:
        case 1091:
        case 1123:
        case 1154:
            return true;
        default:
            return false;
    }
}

// checkBattlefieldTrustCount pure.
constexpr auto CheckBattlefieldTrustCount(const bool inBattlefield,
                                          int        maxParticipants,
                                          const int  numPlayers,
                                          const int  numTrusts,
                                          const bool isRoVKI,
                                          const bool hasRhapsodyInUmber) -> bool
{
    if (!inBattlefield)
    {
        return true;
    }
    if (isRoVKI && hasRhapsodyInUmber)
    {
        maxParticipants = RoVKIMaxParticipants;
    }
    return (numPlayers + numTrusts) < maxParticipants;
}

} // namespace trustbattlefieldhelpers
