#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <string>

// Pure CAlliance capacity gates extracted so native tests can pin policy
// without DB, party pointers, or packets.

namespace alliancehelpers
{

// AllianceLocalFullCount is partyList.size() at which isFull short-circuits.
// Production uses == 3 (not > 2), preserved for parity.
constexpr std::size_t AllianceLocalFullCount = 3;

// AllianceRemoteFullCount is loadPartyCount() at which isFull is true.
constexpr uint32 AllianceRemoteFullCount = 3;

// IsLocalAllianceFull mirrors partyList.size() == 3.
inline auto IsLocalAllianceFull(const std::size_t localPartyCount) -> bool
{
    return localPartyCount == AllianceLocalFullCount;
}

// IsRemoteAllianceFull mirrors loadPartyCount() == 3.
inline auto IsRemoteAllianceFull(const uint32 loadedPartyCount) -> bool
{
    return loadedPartyCount == AllianceRemoteFullCount;
}

// IsAllianceFull mirrors CAlliance::isFull: local full short-circuits, else remote.
inline auto IsAllianceFull(const std::size_t localPartyCount, const uint32 loadedPartyCount) -> bool
{
    if (IsLocalAllianceFull(localPartyCount))
    {
        return true;
    }
    return IsRemoteAllianceFull(loadedPartyCount);
}

// HasOnlyOneLocalParty mirrors partyList.size() != 1 early false.
inline auto HasOnlyOneLocalParty(const std::size_t localPartyCount) -> bool
{
    return localPartyCount == 1;
}

// HasOnlyOneAllianceParty mirrors CAlliance::hasOnlyOneParty:
// local size must be 1 and loadPartyCount must be 1.
inline auto HasOnlyOneAllianceParty(const std::size_t localPartyCount, const uint32 loadedPartyCount) -> bool
{
    if (!HasOnlyOneLocalParty(localPartyCount))
    {
        return false;
    }
    return loadedPartyCount == 1;
}

// LoadPartyCountFromQuery mirrors loadPartyCount given preparedStmt outcome:
// queryOk false → 0; else rowsCount.
inline auto LoadPartyCountFromQuery(const bool queryOk, const uint32 rowsCount) -> uint32
{
    if (!queryOk)
    {
        return 0;
    }
    return rowsCount;
}

// dissolve_alliance_path is which host branch dissolveAlliance takes.
enum class dissolve_alliance_path : uint8_t
{
    PLAYER_IPC, // playerInitiated: send AllianceDissolve IPC
    SERVER_DB,  // server-side: DB clear + delParty loop + delete this
};

// ClassifyDissolveAlliance mirrors dissolveAlliance's playerInitiated branch.
inline auto ClassifyDissolveAlliance(const bool playerInitiated) -> dissolve_alliance_path
{
    return playerInitiated ? dissolve_alliance_path::PLAYER_IPC : dissolve_alliance_path::SERVER_DB;
}

// DissolvePartyFlagClearMask is ALLIANCE_LEADER | PARTY_SECOND | PARTY_THIRD
// (0x0008 | 0x0001 | 0x0002 = 0x000B) used when clearing partyflag bits.
constexpr uint16 DissolvePartyFlagClearMask = 0x0008 | 0x0001 | 0x0002;

// ShouldApplyUnfilteredDissolveServerFilter mirrors
// IF(? = 0 AND ? = 0, true, server_addr = ? AND server_port = ?) when both
// extracted map IPP components are zero (no session found).
inline auto ShouldApplyUnfilteredDissolveServerFilter(const uint32 mapIP, const uint16 mapPort) -> bool
{
    return mapIP == 0 && mapPort == 0;
}

// remove_alliance_party_plan is the pure host action plan for removeParty.
enum class remove_alliance_party_plan : uint8_t
{
    NULL_PARTY,         // party == nullptr — warn and return
    DISSOLVE_STILL_MAIN, // was main and still main after promote attempt
    DEL_AND_NOTIFY,      // delParty + DB flag clear + AllianceReload/PartyReload IPC
};

// ClassifyRemoveAllianceParty mirrors removeParty control flow after null check
// and after any main-party assignAllianceLeader attempt.
// isMainParty: getMainParty() == party before promote.
// stillMainAfterPromote: getMainParty() == party after promote (or no promote).
// For non-main parties, stillMainAfterPromote is ignored.
inline auto ClassifyRemoveAllianceParty(
    const bool isNull,
    const bool isMainParty,
    const bool stillMainAfterPromote) -> remove_alliance_party_plan
{
    if (isNull)
    {
        return remove_alliance_party_plan::NULL_PARTY;
    }
    if (isMainParty && stillMainAfterPromote)
    {
        return remove_alliance_party_plan::DISSOLVE_STILL_MAIN;
    }
    return remove_alliance_party_plan::DEL_AND_NOTIFY;
}

// ShouldAttemptAllianceLeaderPromote mirrors isMainParty before the DB lookup
// for another party leader in the alliance.
inline auto ShouldAttemptAllianceLeaderPromote(const bool isMainParty) -> bool
{
    return isMainParty;
}

// FormatRemoveAlliancePartyNullWarning mirrors the ShowWarning text.
inline auto FormatRemoveAlliancePartyNullWarning() -> std::string
{
    return "CAlliance::removeParty - party is null!";
}

// FormatDelPartyNullWarning mirrors delParty's null warning.
inline auto FormatDelPartyNullWarning() -> std::string
{
    return "CAlliance::delParty - party is null!";
}

// ShouldSkipDelPartyWhenEmpty mirrors !party->m_PAlliance || partyList.size()==0.
inline auto ShouldSkipDelPartyWhenEmpty(const bool hasAlliance, const bool partyListEmpty) -> bool
{
    return !hasAlliance || partyListEmpty;
}

} // namespace alliancehelpers
