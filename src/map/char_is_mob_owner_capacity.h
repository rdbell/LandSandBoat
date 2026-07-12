#pragma once

#include <cstdint>
#include <functional>

// Pure claim-ownership policy from CCharEntity::IsMobOwner.
// Host injects null rejection (with warning), claim-type resolution, and
// alliance membership scanning.

namespace charismobownerhelpers
{

// Evaluate returns true when the character may act on the claimed target:
//   unclaimed (ownerId==0), self-owned, PC target, non-exclusive claim mob,
//   or alliance member owns the claim.
template <typename AllianceHasOwner>
inline auto Evaluate(const std::uint32_t ownerId,
                     const std::uint32_t selfId,
                     const bool targetIsPC,
                     const bool nonExclusiveClaim,
                     AllianceHasOwner&& allianceHasOwner) -> bool
{
    if (ownerId == 0 || ownerId == selfId || targetIsPC)
    {
        return true;
    }
    if (nonExclusiveClaim)
    {
        return true;
    }
    return std::invoke(allianceHasOwner);
}

} // namespace charismobownerhelpers
