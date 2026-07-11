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

} // namespace partyhelpers
