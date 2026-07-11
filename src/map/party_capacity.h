#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>

// Pure CParty capacity / trust admission gates extracted so native tests can
// pin policy without DB, entity pointers, or packets.

namespace partyhelpers
{

// PCPartyLocalFullThreshold is the exclusive upper bound used by IsFull's
// local check: members.size() > 5 means full (capacity 6).
constexpr std::size_t PCPartyLocalFullThreshold = 5;

// PCPartyRemoteFullThreshold matches LoadPartySize() > 5 for cross-process full.
constexpr uint32 PCPartyRemoteFullThreshold = 5;

// IsLocalPartyFull mirrors members.size() > 5.
inline auto IsLocalPartyFull(const std::size_t localMemberCount) -> bool
{
    return localMemberCount > PCPartyLocalFullThreshold;
}

// IsRemotePartyFull mirrors LoadPartySize() > 5.
inline auto IsRemotePartyFull(const uint32 loadedPartySize) -> bool
{
    return loadedPartySize > PCPartyRemoteFullThreshold;
}

// IsPartyFull mirrors CParty::IsFull: local full short-circuits, else remote.
inline auto IsPartyFull(const std::size_t localMemberCount, const uint32 loadedPartySize) -> bool
{
    if (IsLocalPartyFull(localMemberCount))
    {
        return true;
    }
    return IsRemotePartyFull(loadedPartySize);
}

// HasOnlyOneLocalMember mirrors members.size() != 1 early false.
inline auto HasOnlyOneLocalMember(const std::size_t localMemberCount) -> bool
{
    return localMemberCount == 1;
}

// HasOnlyOnePartyMember mirrors CParty::HasOnlyOneMember:
// local size must be 1 and LoadPartySize must be 1.
inline auto HasOnlyOnePartyMember(const std::size_t localMemberCount, const uint32 loadedPartySize) -> bool
{
    if (!HasOnlyOneLocalMember(localMemberCount))
    {
        return false;
    }
    return loadedPartySize == 1;
}

// LoadPartySizeForType mirrors CParty::LoadPartySize when not PARTY_PCS:
// mob parties return local size without querying accounts_parties.
// isPCParty false → return localMemberCount as uint32; true → use DB result.
inline auto LoadPartySizeForType(const bool isPCParty, const std::size_t localMemberCount, const uint32 dbCount) -> uint32
{
    if (!isPCParty)
    {
        return static_cast<uint32>(localMemberCount);
    }
    return dbCount;
}

// ShouldRejectPCAddFull mirrors AddMember's IsFull gate for TYPE_PC + PARTY_PCS.
inline auto ShouldRejectPCAddFull(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// ShouldRejectPCAddTrusts mirrors AddMember's HasTrusts gate for TYPE_PC + PARTY_PCS.
inline auto ShouldRejectPCAddTrusts(const bool isPCEntity, const bool isPCParty, const bool partyHasTrusts) -> bool
{
    return isPCEntity && isPCParty && partyHasTrusts;
}

// PartyHasTrusts mirrors CParty::HasTrusts given host-scanned member trusts.
// anyMemberHasTrusts is true when any PC member has a non-empty PTrusts list.
inline auto PartyHasTrusts(const bool anyMemberHasTrusts) -> bool
{
    return anyMemberHasTrusts;
}

} // namespace partyhelpers
