#pragma once

#include <cstdint>

namespace zonecharsyncrelationship
{

struct Relationship
{
    bool sameParty;
    bool sameAlliance;
};

// Determine mirrors getSignificanceScore's party/alliance relationship
// sampling. Alliance membership matters only when both characters have a
// party, and each of those parties has an alliance.
constexpr auto Determine(const bool     originHasParty,
                         const uint32_t originPartyID,
                         const bool     targetHasParty,
                         const uint32_t targetPartyID,
                         const bool     originHasAlliance,
                         const uint32_t originAllianceID,
                         const bool     targetHasAlliance,
                         const uint32_t targetAllianceID) -> Relationship
{
    if (!originHasParty || !targetHasParty)
    {
        return {};
    }

    return {
        .sameParty    = originPartyID == targetPartyID,
        .sameAlliance = originHasAlliance && targetHasAlliance && originAllianceID == targetAllianceID,
    };
}

} // namespace zonecharsyncrelationship
