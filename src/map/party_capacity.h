#pragma once

#include "common/cbasetypes.h"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

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

// MemberCountContribution is one member's contribution to CParty::MemberCount.
// zoneMatches is member->getZone() == ZoneID.
// isPC is member->objtype == TYPE_PC.
// trustCount is PTrusts.size() for PC members (ignored when !isPC).
// Trusts are counted regardless of trust zone (production does not filter them).
inline auto MemberCountContribution(const bool zoneMatches, const bool isPC, const uint8 trustCount) -> uint8
{
    uint8 count = 0;
    if (zoneMatches)
    {
        count++;
    }
    if (isPC)
    {
        count = static_cast<uint8>(count + trustCount);
    }
    return count;
}

// AccumulateMemberCount adds one contribution into a running total with uint8 wrap
// matching production's uint8 count increments.
inline auto AccumulateMemberCount(const uint8 running, const uint8 contribution) -> uint8
{
    return static_cast<uint8>(running + contribution);
}

// LevelSyncMinLevel is the exclusive lower bound for the sync target's main job
// level in CParty::RefreshSync (syncLevel < 10 removes sync).
constexpr uint8 LevelSyncMinLevel = 10;

// ShouldRemoveSyncForLowLevel mirrors syncLevel < 10 before SetSyncTarget clear.
inline auto ShouldRemoveSyncForLowLevel(const uint8 syncLevel) -> bool
{
    return syncLevel < LevelSyncMinLevel;
}

// ResolveSyncMemberLevel mirrors NewMLevel selection for a same-zone PC member:
// if syncLevel < memberMainJobLevel then syncLevel else memberMainJobLevel.
inline auto ResolveSyncMemberLevel(const uint8 syncLevel, const uint8 memberMainJobLevel) -> uint8
{
    if (syncLevel < memberMainJobLevel)
    {
        return syncLevel;
    }
    return memberMainJobLevel;
}

// ShouldApplySyncToMember mirrors the RefreshSync per-member filter:
// TYPE_PC and same zone as the sync target.
inline auto ShouldApplySyncToMember(const bool isPC, const bool sameZoneAsSyncTarget) -> bool
{
    return isPC && sameZoneAsSyncTarget;
}

// LevelSyncDisableDurationSeconds is the countdown applied when removing an
// infinite (duration == 0) LevelSync effect on disable.
constexpr uint32 LevelSyncDisableDurationSeconds = 30;

// MsgStd IDs used by SetSyncTarget rejection paths (msg_std.h).
constexpr uint16 MsgLevelSyncDesigneeBelowMin    = 541;
constexpr uint16 MsgLevelSyncDesigneeInOtherArea = 542;
constexpr uint16 MsgLevelSyncPreventedByStatus   = 543;

// set_sync_target_gate is the pure outcome of CParty::SetSyncTarget admission
// after LEVEL_SYNC_ENABLE and before packet/DB hosts.
enum class set_sync_target_gate : uint8_t
{
    DISABLED,           // map.LEVEL_SYNC_ENABLE is false — no-op
    ENABLE,             // set m_PSyncTarget and apply LevelSync to members
    DISABLE,            // clear m_PSyncTarget (empty/invalid designee)
    REJECT_BELOW_MIN,   // designee main level < LevelSyncMinLevel
    REJECT_OTHER_AREA,  // designee zone != leader zone
    REJECT_STATUS,      // a party member has a blocking status effect
};

// ClassifySetSyncTarget mirrors SetSyncTarget's ordered policy checks.
// designeeFound/designeeIsPC describe GetMemberByName + TYPE_PC.
// designeeLevel is GetMLevel(); sameZoneAsLeader compares designee vs leader zone.
// anyMemberHasBlockingStatus is true when any member has LevelRestriction,
// LevelSync, SjRestriction, Confrontation, or Battlefield.
inline auto ClassifySetSyncTarget(
    const bool  levelSyncEnabled,
    const bool  designeeFound,
    const bool  designeeIsPC,
    const uint8 designeeLevel,
    const bool  sameZoneAsLeader,
    const bool  anyMemberHasBlockingStatus) -> set_sync_target_gate
{
    if (!levelSyncEnabled)
    {
        return set_sync_target_gate::DISABLED;
    }
    if (!designeeFound || !designeeIsPC)
    {
        return set_sync_target_gate::DISABLE;
    }
    if (designeeLevel < LevelSyncMinLevel)
    {
        return set_sync_target_gate::REJECT_BELOW_MIN;
    }
    if (!sameZoneAsLeader)
    {
        return set_sync_target_gate::REJECT_OTHER_AREA;
    }
    if (anyMemberHasBlockingStatus)
    {
        return set_sync_target_gate::REJECT_STATUS;
    }
    return set_sync_target_gate::ENABLE;
}

// ShouldApplySyncEnableToMember mirrors the enable-path per-member filter:
// TYPE_PC, status != DISAPPEAR, same zone as designee.
inline auto ShouldApplySyncEnableToMember(
    const bool isPC,
    const bool notDisappear,
    const bool sameZoneAsDesignee) -> bool
{
    return isPC && notDisappear && sameZoneAsDesignee;
}

// ShouldApplySyncDisableToMember mirrors the disable-path per-member filter:
// TYPE_PC and status != DISAPPEAR (zone is not checked on disable).
inline auto ShouldApplySyncDisableToMember(const bool isPC, const bool notDisappear) -> bool
{
    return isPC && notDisappear;
}

// ShouldStartSyncDisableCountdown mirrors disable applying a 30s countdown only
// when the member has LevelSync with duration == 0 (infinite).
inline auto ShouldStartSyncDisableCountdown(const bool hasLevelSync, const bool durationIsZero) -> bool
{
    return hasLevelSync && durationIsZero;
}

// ShouldPushPartyPacketToMember mirrors CParty::PushPacket's per-member filter.
// ZoneID 0 means all zones; otherwise the member must match ZoneID.
// senderID is skipped; DISAPPEAR and prison members are skipped.
inline auto ShouldPushPartyPacketToMember(
    const bool   isPC,
    const uint32 memberID,
    const uint32 senderID,
    const bool   notDisappear,
    const bool   inPrison,
    const uint16 zoneIDFilter,
    const uint16 memberZoneID) -> bool
{
    if (!isPC)
    {
        return false;
    }
    if (memberID == senderID)
    {
        return false;
    }
    if (!notDisappear)
    {
        return false;
    }
    if (inPrison)
    {
        return false;
    }
    if (zoneIDFilter != 0 && memberZoneID != zoneIDFilter)
    {
        return false;
    }
    return true;
}

// ShouldIncludeInGroupEffects mirrors PushEffectsPacket's partyInfo_t filter for
// building sameZoneMembers: same party, not self, char resolved, same zone.
inline auto ShouldIncludeInGroupEffects(
    const uint32 infoPartyID,
    const uint32 partyID,
    const uint32 infoCharID,
    const uint32 selfCharID,
    const bool   charFound,
    const bool   sameZone) -> bool
{
    if (infoPartyID != partyID)
    {
        return false;
    }
    if (infoCharID == selfCharID)
    {
        return false;
    }
    if (!charFound)
    {
        return false;
    }
    if (!sameZone)
    {
        return false;
    }
    return true;
}

// ShouldPushEffectsPacket mirrors PushEffectsPacket's m_EffectsChanged gate.
inline auto ShouldPushEffectsPacket(const bool effectsChanged) -> bool
{
    return effectsChanged;
}

// remove_party_leader_plan is the pure host action plan for RemovePartyLeader
// after empty-list check and after any PC DB promote attempt / mob next-member scan.
enum class remove_party_leader_plan : uint8_t
{
    EMPTY_LIST,          // members.empty() — warn, return false
    MOB_PROMOTE_AND_DEL, // PARTY_MOBS with another member — assign leader, DelMember, return true
    DISBAND,             // still leader after promote attempt — DisbandParty, return false
    REMOVE_MEMBER,       // no longer leader — RemoveMember, return true
};

// ShouldAttemptPCLeaderPromote mirrors m_PartyType != PARTY_MOBS before the
// accounts_sessions JOIN lookup for a non-leader replacement.
inline auto ShouldAttemptPCLeaderPromote(const bool isMobParty) -> bool
{
    return !isMobParty;
}

// ClassifyRemovePartyLeader mirrors RemovePartyLeader after the empty check.
// isMobParty: m_PartyType == PARTY_MOBS.
// hasOtherMember: any member != removing entity (used for mob promote path).
// stillLeader: m_PLeader == PEntity after any PC SetLeader / before mob promote.
// For mob parties with hasOtherMember, MOB_PROMOTE_AND_DEL wins before stillLeader.
inline auto ClassifyRemovePartyLeader(
    const bool isEmpty,
    const bool isMobParty,
    const bool hasOtherMember,
    const bool stillLeader) -> remove_party_leader_plan
{
    if (isEmpty)
    {
        return remove_party_leader_plan::EMPTY_LIST;
    }
    if (isMobParty && hasOtherMember)
    {
        return remove_party_leader_plan::MOB_PROMOTE_AND_DEL;
    }
    if (stillLeader)
    {
        return remove_party_leader_plan::DISBAND;
    }
    return remove_party_leader_plan::REMOVE_MEMBER;
}

// RemovePartyLeaderReturnValue mirrors the bool return for each plan.
inline auto RemovePartyLeaderReturnValue(const remove_party_leader_plan plan) -> bool
{
    switch (plan)
    {
        case remove_party_leader_plan::MOB_PROMOTE_AND_DEL:
        case remove_party_leader_plan::REMOVE_MEMBER:
            return true;
        case remove_party_leader_plan::EMPTY_LIST:
        case remove_party_leader_plan::DISBAND:
        default:
            return false;
    }
}

// FormatRemovePartyLeaderEmptyWarning mirrors the ShowWarning text.
inline auto FormatRemovePartyLeaderEmptyWarning() -> std::string
{
    return "CParty::RemovePartyLeader - called when \"member\" list was empty";
}

// disband_party_member_path is the pure type branch inside DisbandParty after
// alliance detach and pointer clears.
enum class disband_party_member_path : uint8_t
{
    PC_FULL,   // PARTY_PCS: packets, trusts, latents, treasure, sync, DB delete, optional IPC
    MOB_CLEAR, // PARTY_MOBS: clear PParty only
    NONE,      // neither PC nor MOB type (should not occur)
};

// ClassifyDisbandPartyMemberPath mirrors m_PartyType == PARTY_PCS / PARTY_MOBS.
inline auto ClassifyDisbandPartyMemberPath(const bool isPCParty, const bool isMobParty) -> disband_party_member_path
{
    if (isPCParty)
    {
        return disband_party_member_path::PC_FULL;
    }
    if (isMobParty)
    {
        return disband_party_member_path::MOB_CLEAR;
    }
    return disband_party_member_path::NONE;
}

// ShouldDetachAllianceOnDisband mirrors if (m_PAlliance) removeParty(this).
inline auto ShouldDetachAllianceOnDisband(const bool hasAlliance) -> bool
{
    return hasAlliance;
}

// ShouldNotifyPartyDisbandIPC mirrors playerInitiated inside the PC path only.
// Host should only call when ClassifyDisbandPartyMemberPath is PC_FULL.
inline auto ShouldNotifyPartyDisbandIPC(const bool playerInitiated) -> bool
{
    return playerInitiated;
}

// ShouldReplaceSoloTreasurePool mirrors treasure pool != Zone type when non-null.
inline auto ShouldReplaceSoloTreasurePool(const bool hasTreasurePool, const bool isZonePool) -> bool
{
    return hasTreasurePool && !isZonePool;
}

// MsgLevelSyncRemoveLeftParty is MsgStd::LevelSyncRemoveLeftParty (553).
constexpr uint16 MsgLevelSyncRemoveLeftParty = 553;

// MsgLevelSyncSet is MsgStd::LevelSyncSet (238).
constexpr uint16 MsgLevelSyncSet = 238;

// assign_party_role_action is the pure host action for AssignPartyRole after
// PC-type and membership checks.
enum class assign_party_role_action : uint8_t
{
    REJECT_MOB_PARTY,      // not PARTY_PCS
    REJECT_NOT_MEMBER,     // DB membership miss
    SET_LEADER,            // ChangeKind 0
    SET_QUARTERMASTER,     // ChangeKind 4
    CLEAR_QUARTERMASTER,   // ChangeKind 5 (lottery)
    SET_LEVEL_SYNC,        // ChangeKind 6
    DISABLE_LEVEL_SYNC,    // ChangeKind 7
    REJECT_UNKNOWN_ROLE,   // default
};

// ClassifyAssignPartyRole mirrors AssignPartyRole ordered gates + switch.
// role is the raw GP_CLI_COMMAND_GROUP_CHANGE2_CHANGEKIND value.
inline auto ClassifyAssignPartyRole(
    const bool   isPCParty,
    const bool   membershipQueryOk,
    const bool   membershipRowFound,
    const uint8  role) -> assign_party_role_action
{
    if (!isPCParty)
    {
        return assign_party_role_action::REJECT_MOB_PARTY;
    }
    if (!membershipQueryOk || !membershipRowFound)
    {
        return assign_party_role_action::REJECT_NOT_MEMBER;
    }
    switch (role)
    {
        case 0: // SetPartyLeader
            return assign_party_role_action::SET_LEADER;
        case 4: // SetQuartermaster
            return assign_party_role_action::SET_QUARTERMASTER;
        case 5: // SetLottery
            return assign_party_role_action::CLEAR_QUARTERMASTER;
        case 6: // SetLevelSync
            return assign_party_role_action::SET_LEVEL_SYNC;
        case 7: // DisableLevelSync
            return assign_party_role_action::DISABLE_LEVEL_SYNC;
        default:
            return assign_party_role_action::REJECT_UNKNOWN_ROLE;
    }
}

// ShouldNotifyAllianceReloadOnRole mirrors if (m_PAlliance) AllianceReload else PartyReload.
// Only for successful role actions (not rejects).
inline auto ShouldNotifyAllianceReloadOnRole(const bool hasAlliance) -> bool
{
    return hasAlliance;
}

// FormatAssignRoleMobPartyWarning mirrors ShowWarningFmt for mob parties.
inline auto FormatAssignRoleMobPartyWarning(const uint8 role, const std::string& memberName) -> std::string
{
    return fmt::format("Attempting to assign role ({}) to {} in Mob Party.", role, memberName);
}

// get_member_by_name_gate is the pure outcome of GetMemberByName before the
// member-list scan.
enum class get_member_by_name_gate : uint8_t
{
    REJECT_MOB_PARTY, // not PARTY_PCS — warn, return nullptr
    REJECT_EMPTY,     // memberName == ""
    SEARCH,           // case-insensitive scan of members
};

// ClassifyGetMemberByName mirrors GetMemberByName's ordered pre-scan gates.
inline auto ClassifyGetMemberByName(const bool isPCParty, const bool nameEmpty) -> get_member_by_name_gate
{
    if (!isPCParty)
    {
        return get_member_by_name_gate::REJECT_MOB_PARTY;
    }
    if (nameEmpty)
    {
        return get_member_by_name_gate::REJECT_EMPTY;
    }
    return get_member_by_name_gate::SEARCH;
}

// FormatGetMemberMobPartyWarning mirrors ShowWarning for mob GetMemberByName.
inline auto FormatGetMemberMobPartyWarning(const std::string& memberName) -> std::string
{
    return fmt::format("Attempting to get Member data for {} in Mob Party.", memberName);
}

// MemberNameMatches mirrors strcmpi(a, b) == 0 (case-insensitive equality).
inline auto MemberNameMatches(const std::string& a, const std::string& b) -> bool
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
        {
            return false;
        }
    }
    return true;
}

// set_leader_lookup_gate is the pure outcome of the PC SetLeader charname query.
enum class set_leader_lookup_gate : uint8_t
{
    NOT_FOUND, // query fail or no row — return early
    FOUND,     // rewrite partyid/flags and set m_PLeader
};

// ClassifySetLeaderLookup mirrors rset success with a row in SetLeader(PC).
inline auto ClassifySetLeaderLookup(const bool queryOk, const bool rowFound) -> set_leader_lookup_gate
{
    if (queryOk && rowFound)
    {
        return set_leader_lookup_gate::FOUND;
    }
    return set_leader_lookup_gate::NOT_FOUND;
}

// PartyLeaderFlag is PARTY_LEADER (0x0004).
constexpr uint16 PartyLeaderFlag = 0x0004;

// AllianceLeaderFlag is ALLIANCE_LEADER (0x0008); dual-hosted with alliancehelpers.
constexpr uint16 AllianceLeaderFlag = 0x0008;

// ClearLeaderFlagsMask is ALLIANCE_LEADER | PARTY_LEADER cleared on old leaders.
constexpr uint16 ClearLeaderFlagsMask = AllianceLeaderFlag | PartyLeaderFlag;

// LeaderPartyFlags mirrors IF(allianceid = partyid, ALLIANCE_LEADER|PARTY_LEADER, PARTY_LEADER).
inline auto LeaderPartyFlags(const bool allianceIdEqualsPartyId) -> uint16
{
    return allianceIdEqualsPartyId ? static_cast<uint16>(AllianceLeaderFlag | PartyLeaderFlag) : PartyLeaderFlag;
}

// ShouldRewriteAllianceIDOnLeaderChange mirrors
// m_PAlliance && m_PAlliance->m_AllianceID == m_PartyID.
inline auto ShouldRewriteAllianceIDOnLeaderChange(const bool hasAlliance, const bool allianceIdEqualsPartyId) -> bool
{
    return hasAlliance && allianceIdEqualsPartyId;
}

// NewPartyIDFromLeaderChar mirrors m_PartyID = newId after PC SetLeader.
inline auto NewPartyIDFromLeaderChar(const uint32 leaderCharID) -> uint32
{
    return leaderCharID;
}

// ShouldUseMobPartyFirstMemberAsLeader mirrors the PARTY_MOBS SetLeader branch
// (members.at(0)); host must ensure the list is non-empty.
inline auto ShouldUseMobPartyFirstMemberAsLeader(const bool isMobParty) -> bool
{
    return isMobParty;
}

// FormatGetPartyInfoMobWarning mirrors GetPartyInfo's mob-party warning.
inline auto FormatGetPartyInfoMobWarning() -> std::string
{
    return "Attempting to get Party data for Mob Party.";
}

// ShouldQueryPartyInfo mirrors m_PartyType == PARTY_PCS before the SQL load.
inline auto ShouldQueryPartyInfo(const bool isPCParty) -> bool
{
    return isPCParty;
}

// GetPartyInfoAllianceIDInject mirrors m_PAlliance ? m_AllianceID : 0 for the
// accounts_parties WHERE clause.
inline auto GetPartyInfoAllianceIDInject(const bool hasAlliance, const uint32 allianceID) -> uint32
{
    return hasAlliance ? allianceID : 0;
}

// GetPartyInfoOrderFlags is PARTY_SECOND | PARTY_THIRD used in ORDER BY partyflag & ?.
constexpr uint16 GetPartyInfoOrderFlags = 0x0001 | 0x0002;

// PartyQMFlag is PARTY_QM (0x0010).
constexpr uint16 PartyQMFlag = 0x0010;

// ShouldSetQuarterMasterDBFlag mirrors if (PEntity != nullptr) after GetMemberByName.
// Always clears QM flags first; sets only when a member was resolved.
inline auto ShouldSetQuarterMasterDBFlag(const bool memberFound) -> bool
{
    return memberFound;
}

// remove_member_gate is the pure outcome of RemoveMember's first checks.
enum class remove_member_gate : uint8_t
{
    REJECT_NULL_OR_MISMATCH, // PEntity null or PParty != this
    REMOVE_AS_LEADER,        // m_PLeader == PEntity → RemovePartyLeader
    REMOVE_NON_LEADER,       // find + PC cleanup path
};

// ClassifyRemoveMember mirrors RemoveMember's leader vs non-leader branch.
inline auto ClassifyRemoveMember(
    const bool entityNull,
    const bool partyMismatch,
    const bool isLeader) -> remove_member_gate
{
    if (entityNull || partyMismatch)
    {
        return remove_member_gate::REJECT_NULL_OR_MISMATCH;
    }
    if (isLeader)
    {
        return remove_member_gate::REMOVE_AS_LEADER;
    }
    return remove_member_gate::REMOVE_NON_LEADER;
}

// FormatRemoveMemberNullWarning mirrors ShowWarning for null/mismatch.
inline auto FormatRemoveMemberNullWarning() -> std::string
{
    return "CParty::RemoveMember() - PEntity was null, or PParty mismatch.";
}

// ShouldClearQuarterMasterOnRemove mirrors m_PQuarterMaster == PChar.
inline auto ShouldClearQuarterMasterOnRemove(const bool isQuarterMaster) -> bool
{
    return isQuarterMaster;
}

// ShouldDisableSyncOnRemove mirrors m_PSyncTarget == PChar for the leaving PC.
inline auto ShouldDisableSyncOnRemove(const bool isSyncTarget) -> bool
{
    return isSyncTarget;
}

// ShouldApplyLeavingSyncCountdown mirrors non-leader leave when the leaver is
// not the sync target but has infinite LevelSync (other members keep sync).
// Original: m_PSyncTarget != nullptr && m_PSyncTarget != PChar && not disappear && has infinite sync.
inline auto ShouldApplyLeavingSyncCountdown(
    const bool hasSyncTarget,
    const bool leaverIsSyncTarget,
    const bool notDisappear,
    const bool hasInfiniteLevelSync) -> bool
{
    return hasSyncTarget && !leaverIsSyncTarget && notDisappear && hasInfiniteLevelSync;
}

// ShouldRunPCRemoveCleanup mirrors m_PartyType == PARTY_PCS && objtype == TYPE_PC.
inline auto ShouldRunPCRemoveCleanup(const bool isPCParty, const bool isPCEntity) -> bool
{
    return isPCParty && isPCEntity;
}

} // namespace partyhelpers
