#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <string>

// Pure CAlliance capacity gates extracted so native tests can pin policy
// without DB, party pointers, or packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1329: local/remote isFull, hasOnlyOneParty, loadPartyCount query
//   - 1339: dissolveAlliance path / dissolve server filter / flag clear mask
//   - 1341: removeParty plan, delParty null warning, ShouldSkipDelPartyWhenEmpty
//   - 1346: assignAllianceLeader gate / leader flag / local main party
//   - 2941: ShouldSkipDelPartyWhenEmpty prior dual-wire (!hasAlliance || partyListEmpty)
//   - 2979: ShouldAttemptAllianceLeaderPromote prior dual-wire (isMainParty)
//   - 2988: ShouldSetLocalMainParty prior dual-wire (memberFoundOnThisServer)
//   - 3077: ShouldSetLocalMainParty (memberFoundOnThisServer) dual-wire expansion
//   - 3121: ShouldSkipDelPartyWhenEmpty (!hasAlliance || partyListEmpty) dual-wire expansion
//   - 3144: ShouldAttemptAllianceLeaderPromote (isMainParty) dual-wire expansion
//   - 3228: ShouldSkipDelPartyWhenEmpty dedicated dual-wire expand residual 2941
//   - 3355: ShouldSetLocalMainParty dedicated dual-wire expand residual 2988
//   - 3393: ShouldAttemptAllianceLeaderPromote dedicated dual-wire expand residual 2979
//   - 3447: ShouldAttemptAllianceLeaderPromote dedicated dual-wire expand residual 2979
//   - 3490: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3547: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3591: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3636: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3681: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3726: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3771: ShouldAttemptAllianceLeaderPromote prior dedicated dual-wire expand residual 2979
//   - 3816: ShouldAttemptAllianceLeaderPromote dedicated dual-wire expand residual 2979
//
// Production host: CAlliance::delParty (alliance.cpp) injects
// party->m_PAlliance != nullptr and partyList.empty() into
// ShouldSkipDelPartyWhenEmpty before erasing from partyList.
// Go dual-wire: alliance.ShouldSkipDelPartyWhenEmpty
// (internal/alliance/skip_del_party_empty.go). Prior pure port: 1341;
// prior dual-wire: 2941; prior dual-wire expansion: 3121;
// dedicated dual-wire expand residual 2941: 3228.
//
// Production host: CAlliance::removeParty (alliance.cpp) injects isMainParty
// (getMainParty() == party after null check) into
// ShouldAttemptAllianceLeaderPromote before DB lookup for another party leader.
// Go dual-wire: alliance.ShouldAttemptAllianceLeaderPromote
// (internal/alliance/attempt_leader_promote.go). Prior pure port: 1341 / 1346;
// prior dual-wire: 2979; prior dual-wire expansion: 3144;
// prior dedicated dual-wire expand residual 2979: 3393;
// prior dedicated dual-wire expand residual 2979: 3447;
// prior dedicated dual-wire expand residual 2979: 3490;
// prior dedicated dual-wire expand residual 2979: 3547;
// prior dedicated dual-wire expand residual 2979: 3591;
// prior dedicated dual-wire expand residual 2979: 3636;
// prior dedicated dual-wire expand residual 2979: 3681;
// prior dedicated dual-wire expand residual 2979: 3726;
// prior dedicated dual-wire expand residual 2979: 3771;
// dedicated dual-wire expand residual 2979: 3816.
//
// Production host: CAlliance::assignAllianceLeader (alliance.cpp) injects
// PParty->GetMemberByName(name) != nullptr into ShouldSetLocalMainParty while
// scanning partyList after clearing aLeader.
// Go dual-wire: alliance.ShouldSetLocalMainParty
// (internal/alliance/set_local_main_party.go). Prior pure port: 1346;
// prior dual-wire: 2988; prior dual-wire expansion: 3077;
// dedicated dual-wire expand residual 2988: 3355.

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
// for another party leader in the alliance (CAlliance::removeParty promote gate).
//
// Formula (slice 3816 dedicated dual-wire expand residual 2979; prior 3771
// dedicated dual-wire expand residual 2979; prior 3726 dedicated dual-wire
// expand residual 2979; prior 3681 dedicated dual-wire expand residual 2979;
// prior 3636 dedicated dual-wire expand residual 2979; prior 3591 dedicated
// dual-wire expand residual 2979; prior 3547 dedicated dual-wire expand
// residual 2979; prior 3490 dedicated dual-wire expand residual 2979; prior
// 3447 dedicated dual-wire expand residual 2979; prior 3393 dedicated
// dual-wire expand residual 2979; prior 3144 dual-wire expansion; prior 2979
// dual-wire; residual 1341 / 1346):
//   isMainParty
//
// isMainParty — host-evaluated getMainParty() == party (after null check)
// true  → attempt DB lookup for another party leader and assignAllianceLeader
// false → skip promote; re-check still-main and classify remove plan only
//
// Dual-wire of Go alliance.ShouldAttemptAllianceLeaderPromote
// (internal/alliance/attempt_leader_promote.go).
// Call site: CAlliance::removeParty after null-party check.
// Residual pure port: slice 1341 (removeParty plan) / 1346 (assignAllianceLeader).
// Prior dual-wire packaging: slice 2979.
// Prior dual-wire expansion: slice 3144.
// Prior dedicated dual-wire expand residual 2979: slice 3393.
// Prior dedicated dual-wire expand residual 2979: slice 3447.
// Prior dedicated dual-wire expand residual 2979: slice 3490.
// Prior dedicated dual-wire expand residual 2979: slice 3547.
// Prior dedicated dual-wire expand residual 2979: slice 3591.
// Prior dedicated dual-wire expand residual 2979: slice 3636.
// Prior dedicated dual-wire expand residual 2979: slice 3681.
// Prior dedicated dual-wire expand residual 2979: slice 3726.
// Prior dedicated dual-wire expand residual 2979: slice 3771.
// Dedicated dual-wire expand residual 2979: slice 3816.
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

// ShouldSkipDelPartyWhenEmpty mirrors delParty's empty-alliance early return:
// !party->m_PAlliance || partyList.size()==0.
//
// Formula (slice 3228 dedicated dual-wire expand residual 2941;
// prior 3121 dual-wire expansion; prior 2941 dual-wire; residual 1341):
//   !hasAlliance || partyListEmpty
//
// hasAlliance     — host-evaluated party->m_PAlliance != nullptr
// partyListEmpty  — host-evaluated partyList.size()==0 (when hasAlliance;
//                   production injects hasAlliance && size==0 so false when
//                   m_PAlliance is null)
// true  → skip delParty body (no alliance, or alliance has no parties)
// false → proceed (erase from partyList, clear m_PAlliance, treasure pool, …)
//
// Dual-wire of Go alliance.ShouldSkipDelPartyWhenEmpty
// (internal/alliance/skip_del_party_empty.go).
// Call site: CAlliance::delParty after null-party check.
// Residual pure port: slice 1341 (removeParty / delParty plan suite).
// Prior dual-wire packaging: slice 2941.
// Prior dual-wire expansion: slice 3121.
// Dedicated dual-wire expand residual 2941: slice 3228.
inline auto ShouldSkipDelPartyWhenEmpty(const bool hasAlliance, const bool partyListEmpty) -> bool
{
    return !hasAlliance || partyListEmpty;
}

// assign_alliance_leader_gate is the pure outcome of the charname+alliance
// leader lookup in assignAllianceLeader.
enum class assign_alliance_leader_gate : uint8_t
{
    NOT_FOUND, // query fail or no row — no-op
    FOUND,     // rewrite alliance id to charid and set ALLIANCE_LEADER flag
};

// ClassifyAssignAllianceLeader mirrors rset success with a row.
inline auto ClassifyAssignAllianceLeader(const bool queryOk, const bool rowFound) -> assign_alliance_leader_gate
{
    if (queryOk && rowFound)
    {
        return assign_alliance_leader_gate::FOUND;
    }
    return assign_alliance_leader_gate::NOT_FOUND;
}

// AllianceLeaderFlag is PARTYFLAG ALLIANCE_LEADER (0x0008).
constexpr uint16 AllianceLeaderFlag = 0x0008;

// ShouldSetLocalMainParty mirrors finding GetMemberByName on this process
// during assignAllianceLeader (local main-party gate).
//
// Formula (slice 3355 dedicated dual-wire expand residual 2988; prior 3077
// dual-wire expansion; prior 2988 dual-wire; residual 1346):
//   memberFoundOnThisServer
//
// memberFoundOnThisServer — host-evaluated PParty->GetMemberByName(name) != nullptr
// true  → set aLeader = PParty (leader present on this process)
// false → leave aLeader nullptr for this party (leader on another server / not in party)
//
// Dual-wire of Go alliance.ShouldSetLocalMainParty
// (internal/alliance/set_local_main_party.go).
// Call site: CAlliance::assignAllianceLeader after clearing aLeader — host
// injects GetMemberByName result per partyList entry.
// Residual pure port: slice 1346 (assignAllianceLeader gate suite).
// Prior dual-wire packaging: slice 2988.
// Prior dual-wire expansion: slice 3077.
// Dedicated dual-wire expand residual 2988: slice 3355.
inline auto ShouldSetLocalMainParty(const bool memberFoundOnThisServer) -> bool
{
    return memberFoundOnThisServer;
}

// NewAllianceIDFromLeaderChar mirrors m_AllianceID = charid after promote.
inline auto NewAllianceIDFromLeaderChar(const uint32 leaderCharID) -> uint32
{
    return leaderCharID;
}

} // namespace alliancehelpers
