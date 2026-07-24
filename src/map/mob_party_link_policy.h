#pragma once

#include <cstdint>

namespace mobpartylinkhelpers
{

// ShouldAttemptPartyLink mirrors FindPartyForMob's source admission gate.
inline auto ShouldAttemptPartyLink(const bool forceLink, const bool familyLink, const int16_t sublink, const bool hasParty) -> bool
{
    return (forceLink || familyLink || sublink != 0) && !hasParty;
}

// ShouldConsiderPartyLinkCandidate mirrors FindPartyForMob's first candidate
// filter. Forced and sublink sources consider all candidates; ordinary sources
// consider only candidates with family-linking enabled.
inline auto ShouldConsiderPartyLinkCandidate(const bool forceLink, const int16_t sourceSublink, const bool candidateFamilyLink) -> bool
{
    return forceLink || sourceSublink != 0 || candidateFamilyLink;
}

// MatchesPartyLink mirrors FindPartyForMob's ordered match selection:
// SUPERLINK, then force-link, then linked-family or matching sublink.
inline auto MatchesPartyLink(
    const int16_t sourceSuperlink,
    const bool    sourceForceLink,
    const int16_t sourceSublink,
    const uint16_t sourceFamily,
    const int16_t candidateSuperlink,
    const bool    candidateForceLink,
    const bool    candidateFamilyLink,
    const uint16_t candidateFamily,
    const int16_t candidateSublink) -> bool
{
    if (sourceSuperlink != 0)
    {
        return candidateSuperlink == sourceSuperlink;
    }
    if (sourceForceLink)
    {
        return candidateForceLink;
    }
    return (candidateFamilyLink && candidateFamily == sourceFamily) ||
           (sourceSublink != 0 && sourceSublink == candidateSublink);
}

} // namespace mobpartylinkhelpers
