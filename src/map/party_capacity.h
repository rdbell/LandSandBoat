#pragma once

#include "common/cbasetypes.h"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure CParty capacity / trust admission gates extracted so native tests can
// pin policy without DB, entity pointers, or packets.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1327 / 1350: capacity thresholds, trust admission, AddMember classify
//   - 1330: level-sync refresh suite (prior pure for low-level remove)
//   - 2928: ShouldRejectPCAddFull residual dual-wire expand
//   - 3200: ShouldRejectPCAddFull prior dedicated dual-wire
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 / pure 1327 / 1350)
//   - 3498: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3200 / pure 1327 / 1350)
//   - 3555: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3498 / 3200 / pure 1327 / 1350)
//   - 3600: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3645: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3690: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3735: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3780: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3735 / 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3825: ShouldRejectPCAddFull prior dedicated dual-wire (retained)
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3780 / 3735 / 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 3870: ShouldRejectPCAddFull dedicated dual-wire
//           (TYPE_PC + PARTY_PCS + partyFull; residual expand 2928 /
//            prior dedicated 3825 / 3780 / 3735 / 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350)
//   - 2937: ShouldRejectPCAddTrusts residual dual-wire expand
//   - 3353: ShouldRejectPCAddTrusts dedicated dual-wire
//           (TYPE_PC + PARTY_PCS + partyHasTrusts; residual expand 2937 / pure 1327 / 1350)
//   - 2955: ShouldClearSeekingParty residual dual-wire expand
//           (also residual sibling pins for ShouldApplyPartyLevelSyncOnJoin)
//   - 3217: ShouldClearSeekingParty dedicated dual-wire
//           (isSeekingParty after join; residual expand 2955 / pure 1350)
//   - 3274: ShouldApplyPartyLevelSyncOnJoin prior dedicated dual-wire
//           (hasSyncTarget / m_PSyncTarget != nullptr after join;
//            residual expand 2955 / pure 1350)
//   - 3305: ShouldApplyPartyLevelSyncOnJoin dedicated dual-wire
//           (hasSyncTarget / m_PSyncTarget != nullptr after join;
//            residual expand 2955 / prior dedicated 3274 / pure 1350)
//   - 2974: ShouldRemoveSyncForLowLevel residual dual-wire expand
//   - 3378: ShouldRemoveSyncForLowLevel prior dedicated dual-wire
//           (RefreshSync syncLevel < 10; residual expand 2974 / pure 1330)
//   - 3430: ShouldRemoveSyncForLowLevel dedicated dual-wire
//           (RefreshSync syncLevel < 10; residual expand 2974 /
//            prior dedicated 3378 / pure 1330)
//   - 2991: ShouldStampLeaderCreatedPartyTime (TYPE_PC && members.size() > 1)
//   - 2999: ShouldApplySyncToMember (RefreshSync isPC && sameZoneAsSyncTarget)
//   - 3015: ShouldApplySyncEnableToMember (SetSyncTarget ENABLE
//           isPC && notDisappear && sameZoneAsDesignee)
//   - 3016: ShouldStartSyncDisableCountdown (LevelSync disable/remove
//           hasLevelSync && durationIsZero)
//   - 3025: ShouldApplySyncDisableToMember (SetSyncTarget DISABLE
//           isPC && notDisappear)
//   - 3031: ShouldPushPartyPacketToMember (PushPacket per-member filter:
//           isPC, not sender, notDisappear, !inPrison, zone filter)
//   - 3036: ShouldIncludeInGroupEffects (PushEffectsPacket partyInfo_t filter:
//           same party, not self, charFound, sameZone)
//   - 3041: ShouldPushEffectsPacket (PushEffectsPacket m_EffectsChanged gate:
//           effectsChanged identity)
//   - 3083: ShouldAttemptPCLeaderPromote (RemovePartyLeader PC promote gate:
//           !isMobParty)
//   - 3101: ShouldDetachAllianceOnDisband (DisbandParty alliance detach gate:
//           hasAlliance identity)
//   - 3114: ShouldNotifyPartyDisbandIPC (DisbandParty PC-path IPC notify gate:
//           playerInitiated identity)
//
// Production host: CParty::AddMember (party.cpp) injects
// isPCEntity / isPCParty / IsFull() into ShouldRejectPCAddFull via ClassifyAddMember,
// isPCEntity / isPCParty / HasTrusts() into ShouldRejectPCAddTrusts,
// PChar->isSeekingParty() into ShouldClearSeekingParty (PC post-process),
// m_PSyncTarget != nullptr into ShouldApplyPartyLevelSyncOnJoin (PC post-process),
// and (objtype == TYPE_PC, members.size()) into ShouldStampLeaderCreatedPartyTime
// after append before stamping PLeader->m_LeaderCreatedPartyTime.
// Production host: CParty::RefreshSync (party.cpp) injects syncLevel into
// ShouldRemoveSyncForLowLevel before SetSyncTarget clear, then gates non-PC
// entities before injecting the same-zone state, levels, and current main
// level into PlanRefreshSyncMember for status-effect power / SetMLevel work.
// Production host: CParty::SetSyncTarget ENABLE (party.cpp:~1222) injects
// isPC / notDisappear / sameZone into ShouldApplySyncEnableToMember before
// LevelSync message / DelStatusEffectsByFlag / AddStatusEffectSilent / CharSync.
// Production host: CParty::SetSyncTarget DISABLE (party.cpp:~1253) injects
// isPC / notDisappear into ShouldApplySyncDisableToMember before LevelSync
// disable handling (optional countdown via ShouldStartSyncDisableCountdown).
// Production host: CParty::PushPacket (party.cpp:~1317) injects isPC /
// member->id / senderID / status != DISAPPEAR / InPrison / ZoneID /
// getZone() into ShouldPushPartyPacketToMember before pushPacket(copy).
// Production host: CParty::PushEffectsPacket (party.cpp:~1333) injects
// m_EffectsChanged into ShouldPushEffectsPacket before GetPartyInfo body;
// (party.cpp:~1350) injects memberinfo.partyid / m_PartyID / memberinfo.id /
// PMemberChar->id / charFound / sameZone into ShouldIncludeInGroupEffects before
// sameZoneMembers.push_back(PPartyMember).
// Production host: CParty::RemovePartyLeader (party.cpp:~538) injects
// isMobParty (m_PartyType == PARTY_MOBS) into ShouldAttemptPCLeaderPromote
// before the accounts_sessions JOIN / SetLeader PC promote path.
// Production host: CParty::DisbandParty (party.cpp:~116) injects
// hasAlliance (m_PAlliance != nullptr) into ShouldDetachAllianceOnDisband
// before m_PAlliance->removeParty(this).
// Production host: CParty::DisbandParty (party.cpp:~169) injects
// playerInitiated into ShouldNotifyPartyDisbandIPC on the PC_FULL path after
// member cleanup / DB delete before message::send(ipc::PartyDisband).
// Go dual-wire: party.ShouldRejectPCAddFull (internal/party/reject_pc_add_full.go;
// residual dual-wire suite: 2928 / test_party_reject_full_2928;
// prior dedicated dual-wire suites: 3200 / test_party_reject_pc_add_full_3200,
// 3498 / test_party_reject_pc_add_full_3498,
// 3555 / test_party_reject_pc_add_full_3555,
// 3600 / test_party_reject_pc_add_full_3600,
// 3645 / test_party_reject_pc_add_full_3645 (retained),
// 3690 / test_party_reject_pc_add_full_3690 (retained),
// 3735 / test_party_reject_pc_add_full_3735 (retained),
// 3780 / test_party_reject_pc_add_full_3780 (retained),
// 3825 / test_party_reject_pc_add_full_3825 (retained);
// dedicated dual-wire suite: 3870 / test_party_reject_pc_add_full_3870),
// party.ShouldRejectPCAddTrusts (internal/party/reject_pc_add_trusts.go;
// residual dual-wire suite: 2937 / test_party_reject_trusts_2937;
// dedicated dual-wire suite: 3353 / test_party_reject_trusts_3353),
// party.ShouldClearSeekingParty (internal/party/clear_seeking.go;
// residual dual-wire suite: 2955 / test_party_clear_seeking_2955;
// dedicated dual-wire suite: 3217 / test_party_clear_seeking_party_3217),
// party.ShouldApplyPartyLevelSyncOnJoin
// (internal/party/apply_level_sync_on_join.go;
// residual dual-wire expand: 2955 / residual AddMember + clear-seeking sibling pins;
// prior dedicated dual-wire suite: 3274 / test_party_level_sync_on_join_3274;
// dedicated dual-wire suite: 3305 / test_party_level_sync_on_join_3305),
// party.ShouldRemoveSyncForLowLevel (internal/party/remove_sync_low.go;
// residual dual-wire suite: 2974 / test_party_remove_sync_low_2974;
// prior dedicated dual-wire suite: 3378 / test_party_remove_sync_low_3378;
// dedicated dual-wire suite: 3430 / test_party_remove_sync_low_3430),
// party.ShouldStampLeaderCreatedPartyTime (internal/party/stamp_leader_created.go),
// party.ShouldApplySyncToMember (internal/party/apply_sync_member.go),
// party.ShouldApplySyncEnableToMember (internal/party/apply_sync_enable.go),
// party.ShouldStartSyncDisableCountdown (internal/party/sync_disable_countdown.go),
// party.ShouldApplySyncDisableToMember (internal/party/apply_sync_disable.go),
// party.ShouldPushPartyPacketToMember (internal/party/push_packet_member.go),
// party.ShouldIncludeInGroupEffects (internal/party/include_group_effects.go),
// party.ShouldPushEffectsPacket (internal/party/push_effects_packet.go),
// party.ShouldAttemptPCLeaderPromote (internal/party/attempt_pc_leader_promote.go),
// party.ShouldDetachAllianceOnDisband (internal/party/detach_alliance_disband.go),
// party.ShouldNotifyPartyDisbandIPC (internal/party/notify_disband_ipc.go).

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
//
// Formula (slice 3870 dedicated dual-wire; residual expand 2928 /
// prior dedicated 3825 / 3780 / 3735 / 3690 / 3645 / 3600 / 3555 / 3498 / 3200 / pure 1327 / 1350 — formula unchanged):
//   isPCEntity && isPCParty && partyFull
//
// isPCEntity — host-evaluated objtype == TYPE_PC
// isPCParty  — host-evaluated m_PartyType == PARTY_PCS
// partyFull  — host-evaluated IsFull() (local/remote; see IsPartyFull)
// true  → reject AddMember (PC into full PC party)
// false → full gate passes (non-PC entity, mob party, or not full)
//
// Dual-wire of Go party.ShouldRejectPCAddFull.
// Call site: ClassifyAddMember / CParty::AddMember host inject.
// Residual dual-wire suite: 2928 / test_party_reject_full_2928.
// Prior dedicated dual-wire suites: 3200 / test_party_reject_pc_add_full_3200,
// 3498 / test_party_reject_pc_add_full_3498,
// 3555 / test_party_reject_pc_add_full_3555,
// 3600 / test_party_reject_pc_add_full_3600,
// 3645 / test_party_reject_pc_add_full_3645 (retained),
// 3690 / test_party_reject_pc_add_full_3690 (retained),
// 3735 / test_party_reject_pc_add_full_3735 (retained),
// 3780 / test_party_reject_pc_add_full_3780 (retained),
// 3825 / test_party_reject_pc_add_full_3825 (retained).
// Dedicated dual-wire suite is test_party_reject_pc_add_full_3870. Formula is
// unchanged; dedicated suite expands free==inline==pin==pin3825 poles + dense 2^3.
inline auto ShouldRejectPCAddFull(const bool isPCEntity, const bool isPCParty, const bool partyFull) -> bool
{
    return isPCEntity && isPCParty && partyFull;
}

// ShouldRejectPCAddTrusts mirrors AddMember's HasTrusts gate for TYPE_PC + PARTY_PCS.
//
// Formula (slice 3353 dedicated dual-wire; residual expand 2937 / pure 1327 /
// 1350 — formula unchanged):
//   isPCEntity && isPCParty && partyHasTrusts
//
// isPCEntity     — host-evaluated objtype == TYPE_PC
// isPCParty      — host-evaluated m_PartyType == PARTY_PCS
// partyHasTrusts — host-evaluated HasTrusts() (any PC member has non-empty trusts)
// true  → reject AddMember (PC into PC party that has summoned trusts)
// false → trusts gate passes (non-PC entity, mob party, or no trusts present)
//
// Dual-wire of Go party.ShouldRejectPCAddTrusts.
// Call site: ClassifyAddMember / CParty::AddMember host inject.
// Residual dual-wire suite: 2937 / test_party_reject_trusts_2937.
// Dedicated dual-wire suite is test_party_reject_trusts_3353. Formula is
// unchanged; dedicated suite expands free==inline==pin poles + dense 2^3.
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
// Dual-wire of Go party.LevelSyncMinLevel (internal/party/remove_sync_low.go).
// Prior pure port: slice 1330. Residual dual-wire expand: 2974.
// Prior dedicated dual-wire: 3378. Dedicated dual-wire: 3430 (formula unchanged).
constexpr uint8 LevelSyncMinLevel = 10;

// ShouldRemoveSyncForLowLevel mirrors syncLevel < 10 before SetSyncTarget clear.
//
// Formula (slice 3430 dedicated dual-wire; residual expand 2974 /
// prior dedicated 3378 / pure 1330 — formula unchanged):
//   syncLevel < LevelSyncMinLevel  // 10
//
// Host-injected scalars (no entity pointers):
//   syncLevel — sync target's main-job level (jobs.job[GetMJob()])
// true  → host calls SetSyncTarget("", MsgStd::LevelSyncRemoveLowLevel)
// false → keep sync; host continues RefreshSync per-member level apply
//
// Dual-wire of Go party.ShouldRemoveSyncForLowLevel
// (internal/party/remove_sync_low.go). Prior pure port: slice 1330.
// Residual dual-wire suite: 2974 / test_party_remove_sync_low_2974.
// Prior dedicated dual-wire suite: 3378 / test_party_remove_sync_low_3378.
// Dedicated dual-wire suite is test_party_remove_sync_low_3430. Formula is
// unchanged; dedicated suite expands free==inline==pin poles + dense edges.
// Call site: CParty::RefreshSync (party.cpp) host inject.
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
//
// Formula (slice 2999 dual-wire):
//   isPC && sameZoneAsSyncTarget
//
// isPC                 — host-evaluated objtype == TYPE_PC
// sameZoneAsSyncTarget — host-evaluated member->getZone() == sync->getZone()
// true  → host continues RefreshSync per-member level apply
//         (ResolveSyncMemberLevel / StatusEffect power / SetMLevel)
// false → continue (skip non-PC or different-zone members)
//
// Dual-wire of Go party.ShouldApplySyncToMember
// (internal/party/apply_sync_member.go). Prior pure port: slice 1330.
// Call site: CParty::RefreshSync (party.cpp) host inject.
// Edges: isPC × sameZone truth table (4 poles).
// Coverage: test_party_apply_sync_member_2999 (not in CMake/main).
inline auto ShouldApplySyncToMember(const bool isPC, const bool sameZoneAsSyncTarget) -> bool
{
    return isPC && sameZoneAsSyncTarget;
}

// refresh_sync_member_plan is the packet and character-refresh decision for
// one CParty::RefreshSync member after the target's level has been resolved.
struct refresh_sync_member_plan
{
    bool  apply        = false;
    uint8 newMainLevel = 0;
    bool  rebuild      = false;
};

// PlanRefreshSyncMember mirrors RefreshSync's per-member filter, main-level
// resolution, and expensive-rebuild gate.
inline auto PlanRefreshSyncMember(
    const bool  isPC,
    const bool  sameZoneAsSyncTarget,
    const uint8 syncLevel,
    const uint8 memberMainJobLevel,
    const uint8 currentMainLevel) -> refresh_sync_member_plan
{
    if (!ShouldApplySyncToMember(isPC, sameZoneAsSyncTarget))
    {
        return {};
    }

    const uint8 newMainLevel = ResolveSyncMemberLevel(syncLevel, memberMainJobLevel);
    return {
        .apply        = true,
        .newMainLevel = newMainLevel,
        .rebuild      = currentMainLevel != newMainLevel,
    };
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

// ShouldApplySyncEnableToMember mirrors the SetSyncTarget enable-path
// per-member filter: TYPE_PC, status != DISAPPEAR, same zone as designee.
//
// Formula (slice 3015 dual-wire):
//   isPC && notDisappear && sameZoneAsDesignee
//
// isPC               — host-evaluated objtype == TYPE_PC
// notDisappear       — host-evaluated member != nullptr && status != DISAPPEAR
// sameZoneAsDesignee — host-evaluated member != nullptr && getZone() == designee zone
// true  → host continues enable per-member LevelSync apply
//         (message / DelStatusEffectsByFlag / AddStatusEffectSilent / CharSync)
// false → continue (skip non-PC, disappear, or different-zone members)
//
// Dual-wire of Go party.ShouldApplySyncEnableToMember
// (internal/party/apply_sync_enable.go). Prior pure port: slice 1334.
// Call site: CParty::SetSyncTarget ENABLE (party.cpp:~1222) host inject.
// Edges: isPC × notDisappear × sameZone truth table (8 poles).
// Coverage: test_party_apply_sync_enable_3015 (not in CMake/main).
// Sibling dual-wire: 2999 ShouldApplySyncToMember (RefreshSync apply filter).
inline auto ShouldApplySyncEnableToMember(
    const bool isPC,
    const bool notDisappear,
    const bool sameZoneAsDesignee) -> bool
{
    return isPC && notDisappear && sameZoneAsDesignee;
}

// ShouldApplySyncDisableToMember mirrors the SetSyncTarget disable-path
// per-member filter: TYPE_PC and status != DISAPPEAR (zone is not checked
// on disable).
//
// Formula (slice 3025 dual-wire):
//   isPC && notDisappear
//
// isPC         — host-evaluated objtype == TYPE_PC
// notDisappear — host-evaluated member != nullptr && status != DISAPPEAR
// true  → host continues disable per-member LevelSync handling
//         (optional ShouldStartSyncDisableCountdown → battle message / SetDuration)
// false → continue (skip non-PC or disappear members)
//
// Dual-wire of Go party.ShouldApplySyncDisableToMember
// (internal/party/apply_sync_disable.go). Prior pure port: slice 1334.
// Call site: CParty::SetSyncTarget DISABLE (party.cpp:~1253) host inject.
// Edges: isPC × notDisappear truth table (4 poles).
// Coverage: test_party_apply_sync_disable_3025 (not in CMake/main).
// Sibling dual-wire: 3015 ShouldApplySyncEnableToMember (enable filter),
// 3016 ShouldStartSyncDisableCountdown (countdown gate).
// Note: unlike enable-path (3015), zone is not checked on disable.
inline auto ShouldApplySyncDisableToMember(const bool isPC, const bool notDisappear) -> bool
{
    return isPC && notDisappear;
}

// ShouldStartSyncDisableCountdown mirrors disable applying a 30s countdown only
// when the member has LevelSync with duration == 0 (infinite).
//
// Formula (slice 3016 dual-wire):
//   hasLevelSync && durationIsZero
//
// hasLevelSync   — host-evaluated StatusEffectContainer has LevelSync
// durationIsZero — host-evaluated sync effect GetDuration() == 0 (infinite)
// true  → host pushes battle message with LevelSyncDisableDurationSeconds,
//         SetStartTime(now), SetDuration(LevelSyncDisableDurationSeconds)
// false → leave existing timed LevelSync (or missing effect) unchanged
//
// Dual-wire of Go party.ShouldStartSyncDisableCountdown
// (internal/party/sync_disable_countdown.go). Prior pure port: slice 1334
// (SetSyncTarget residual suite; also reused by remove/disband hosts).
// Call sites: CParty::SetSyncTarget DISABLE path; CParty::RemoveMember /
// Disband countdown sites (party.cpp:~158 and siblings) host inject.
// Edges: hasLevelSync × durationIsZero truth table (4 poles).
// Sibling dual-wire: slice 3015 (ShouldApplySyncEnableToMember enable filter).
// Coverage: test_party_sync_disable_countdown_3016 (not in CMake/main).
inline auto ShouldStartSyncDisableCountdown(const bool hasLevelSync, const bool durationIsZero) -> bool
{
    return hasLevelSync && durationIsZero;
}

// ShouldPushPartyPacketToMember mirrors CParty::PushPacket's per-member filter.
// ZoneIDFilter 0 means all zones; otherwise the member must match ZoneIDFilter.
// Sender is skipped; DISAPPEAR and prison members are skipped.
//
// Formula (order of short-circuits; slice 3031 dual-wire):
//   !isPC                                              → false
//   memberID == senderID                               → false
//   !notDisappear                                      → false
//   inPrison                                           → false
//   zoneIDFilter != 0 && memberZoneID != zoneIDFilter  → false
//   else                                               → true
//
// Equivalent pin:
//   isPC && memberID != senderID && notDisappear && !inPrison &&
//     (zoneIDFilter == 0 || memberZoneID == zoneIDFilter)
//
// isPC          — host-evaluated objtype == TYPE_PC
// memberID      — host-evaluated member->id
// senderID      — PushPacket senderID arg (packet originator skipped)
// notDisappear  — host-evaluated member->status != DISAPPEAR
// inPrison      — host-evaluated jailutils::InPrison(member)
// zoneIDFilter  — PushPacket ZoneID arg (0 = all zones)
// memberZoneID  — host-evaluated member->getZone()
// true  → host pushes packet->copy() to member
// false → continue (skip member)
//
// Dual-wire of Go party.ShouldPushPartyPacketToMember
// (internal/party/push_packet_member.go). Prior pure port: slice 1335.
// Call site: CParty::PushPacket (party.cpp:~1317) host inject.
// Note: host null-member continue and TYPE_PC early continue sit outside
// this free function (isPC is still the first pure short-circuit).
// Coverage: test_party_push_packet_member_3031 (not in CMake/main).
// Residual suite: test_party_push_packet_1335.
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
//
// Formula (order of short-circuits; slice 3036 dual-wire):
//   infoPartyID != partyID   → false
//   infoCharID == selfCharID → false
//   !charFound               → false
//   !sameZone                → false
//   else                     → true
//
// Equivalent pin:
//   infoPartyID == partyID && infoCharID != selfCharID && charFound && sameZone
//
// infoPartyID — host-evaluated partyInfo_t.partyid
// partyID     — host-evaluated m_PartyID (recipient party)
// infoCharID  — host-evaluated partyInfo_t.id
// selfCharID  — host-evaluated PMemberChar->id (recipient; self skipped)
// charFound   — host-evaluated zoneutils::GetChar(memberinfo.id) != nullptr
// sameZone    — host-evaluated charFound && GetChar->getZone() == PMemberChar->getZone()
// true  → host pushes PPartyMember into sameZoneMembers
// false → continue (skip info row)
//
// Dual-wire of Go party.ShouldIncludeInGroupEffects
// (internal/party/include_group_effects.go). Prior pure port: slice 1336.
// Call site: CParty::PushEffectsPacket (party.cpp:~1350) host inject.
// Sibling dual-wire: 3031 ShouldPushPartyPacketToMember (PushPacket filter).
// Sibling dual-wire: 3041 ShouldPushEffectsPacket (m_EffectsChanged gate).
// Coverage: test_party_include_group_effects_3036 (not in CMake/main).
// Residual suite: test_party_group_effects_1336.
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
//
// Formula (slice 3041 dual-wire):
//   effectsChanged
//
// effectsChanged — host-evaluated m_EffectsChanged (set by EffectsChanged())
// true  → host continues PushEffectsPacket body (GetPartyInfo / sameZoneMembers /
//         GROUP_EFFECTS push / m_EffectsChanged = false)
// false → early return (no work)
//
// Dual-wire of Go party.ShouldPushEffectsPacket
// (internal/party/push_effects_packet.go). Prior pure port: slice 1336.
// Call site: CParty::PushEffectsPacket (party.cpp:~1333) host inject.
// Sibling dual-wire: 3036 ShouldIncludeInGroupEffects (partyInfo_t filter).
// Coverage: test_party_push_effects_packet_3041 (not in CMake/main).
// Residual suite: test_party_group_effects_1336.
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
//
// Formula (slice 3083 dual-wire):
//   !isMobParty
//
// isMobParty — host-evaluated m_PartyType == PARTY_MOBS
// true  → skip PC DB promote (mob path uses member-scan promote instead)
// false → attempt accounts_sessions JOIN for oldest non-leader SetLeader
//
// Dual-wire of Go party.ShouldAttemptPCLeaderPromote
// (internal/party/attempt_pc_leader_promote.go). Prior pure port: slice 1340.
// Call site: CParty::RemovePartyLeader (party.cpp:~538) host inject.
// Residual suite: test_party_remove_leader_1340 (classify / return / warning).
// Coverage: test_party_attempt_pc_leader_3083 (not in CMake/main).
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
//
// Formula (slice 3101 dual-wire):
//   hasAlliance
//
// hasAlliance — host-evaluated m_PAlliance != nullptr
// true  → host calls m_PAlliance->removeParty(this) then clears m_PAlliance
// false → skip alliance detach (solo party disband)
//
// Dual-wire of Go party.ShouldDetachAllianceOnDisband
// (internal/party/detach_alliance_disband.go). Prior pure port: slice 1345.
// Call site: CParty::DisbandParty (party.cpp:~116) host inject.
// Residual suite: test_party_disband_1345 (classify / IPC / treasure / MsgStd).
// Coverage: test_party_detach_alliance_3101 (not in CMake/main).
inline auto ShouldDetachAllianceOnDisband(const bool hasAlliance) -> bool
{
    return hasAlliance;
}

// ShouldNotifyPartyDisbandIPC mirrors playerInitiated inside the PC path only.
//
// Formula (slice 3114 dual-wire):
//   playerInitiated
//
// playerInitiated — host-evaluated DisbandParty(playerInitiated) argument
// true  → host sends ipc::PartyDisband { partyId } (message server notify)
// false → skip IPC (disband already came from message server)
//
// Dual-wire of Go party.ShouldNotifyPartyDisbandIPC
// (internal/party/notify_disband_ipc.go). Prior pure port: slice 1345.
// Call site: CParty::DisbandParty (party.cpp:~169) host inject on PC_FULL.
// Host should only call when ClassifyDisbandPartyMemberPath is PC_FULL.
// Residual suite: test_party_disband_1345 (classify / detach / treasure / MsgStd).
// Coverage: test_party_notify_disband_ipc_3114 (not in CMake/main).
inline auto ShouldNotifyPartyDisbandIPC(const bool playerInitiated) -> bool
{
    return playerInitiated;
}

// ShouldReplaceSoloTreasurePool mirrors treasure pool != Zone type when non-null.
//
// Formula (slice 3134 dual-wire):
//   hasTreasurePool && !isZonePool
//
// hasTreasurePool — host-evaluated PTreasurePool != nullptr
// isZonePool      — host-evaluated pool present and getPoolType() == Zone
// true  → host delMember, new Solo pool, addMember, updatePool
// false → keep existing pool (null or Zone type)
//
// Dual-wire of Go party.ShouldReplaceSoloTreasurePool
// (internal/party/replace_solo_treasure_pool.go). Prior pure port: slice 1345.
// Call site: CParty::DisbandParty (party.cpp:~148) host inject on PC_FULL
// member loop.
// Residual suite: test_party_disband_1345 (classify / detach / IPC / MsgStd).
// Coverage: test_party_replace_solo_pool_3134 (not in CMake/main).
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

// add_member_gate is the pure outcome of AddMember admission before mutation.
enum class add_member_gate : uint8_t
{
    REJECT_NULL_OR_HAS_PARTY, // PEntity null or already has PParty
    REJECT_ALREADY_MEMBER,    // already in members list
    REJECT_FULL,              // PC party full
    REJECT_TRUSTS,            // PC party has trusts
    PROCEED,                  // append member
};

// ClassifyAddMember mirrors AddMember's ordered rejection gates.
// alreadyHasParty is PEntity->PParty != nullptr; alreadyInList is find result.
// partyFull/hasTrusts are host-evaluated IsFull/HasTrusts results.
inline auto ClassifyAddMember(
    const bool entityNull,
    const bool alreadyHasParty,
    const bool alreadyInList,
    const bool isPCEntity,
    const bool isPCParty,
    const bool partyFull,
    const bool hasTrusts) -> add_member_gate
{
    if (entityNull || alreadyHasParty)
    {
        return add_member_gate::REJECT_NULL_OR_HAS_PARTY;
    }
    if (alreadyInList)
    {
        return add_member_gate::REJECT_ALREADY_MEMBER;
    }
    if (ShouldRejectPCAddFull(isPCEntity, isPCParty, partyFull))
    {
        return add_member_gate::REJECT_FULL;
    }
    if (ShouldRejectPCAddTrusts(isPCEntity, isPCParty, hasTrusts))
    {
        return add_member_gate::REJECT_TRUSTS;
    }
    return add_member_gate::PROCEED;
}

// FormatAddMemberNullWarning mirrors null/has-party warning.
inline auto FormatAddMemberNullWarning() -> std::string
{
    return "CParty::AddMember() - PEntity was null, or PParty not null.";
}

// FormatAddMemberAlreadyInListWarning mirrors duplicate member warning.
inline auto FormatAddMemberAlreadyInListWarning() -> std::string
{
    return "CParty::AddMember() - PEntity was already in the member list!";
}

// FormatAddMemberFullWarning mirrors full-party warning.
inline auto FormatAddMemberFullWarning() -> std::string
{
    return "CParty::AddMember() - Party was full when trying to add a member.";
}

// FormatAddMemberTrustsWarning mirrors trusts-present warning.
inline auto FormatAddMemberTrustsWarning() -> std::string
{
    return "CParty::AddMember() - Party had summoned trusts when trying to add a member.";
}

// FormatAddMemberNonPlayerWarning mirrors non-player into PC path.
inline auto FormatAddMemberNonPlayerWarning(const std::string& name) -> std::string
{
    return fmt::format("Non-Player passed into function ({}).", name);
}

// ShouldStampLeaderCreatedPartyTime mirrors TYPE_PC && members.size() > 1 after add.
//
// Formula (slice 2991 dual-wire):
//   isPCEntity && memberCountAfterAdd > 1
//
// isPCEntity          — host-evaluated objtype == TYPE_PC
// memberCountAfterAdd — host-evaluated members.size() after emplace_back
// true  → host stamps PLeader->m_LeaderCreatedPartyTime = timer::now()
// false → leave leader created-party time unchanged
//
// Dual-wire of Go party.ShouldStampLeaderCreatedPartyTime
// (internal/party/stamp_leader_created.go). Prior pure port: slice 1350.
// Call site: CParty::AddMember (party.cpp) host inject after members.emplace_back.
// Edges: memberCount 0, 1, 2; isPC true/false.
// Coverage: test_party_stamp_leader_created_2991 (not in CMake/main).
inline auto ShouldStampLeaderCreatedPartyTime(const bool isPCEntity, const std::size_t memberCountAfterAdd) -> bool
{
    return isPCEntity && memberCountAfterAdd > 1;
}

// ShouldRunPCAddPostProcess mirrors m_PartyType == PARTY_PCS after append.
inline auto ShouldRunPCAddPostProcess(const bool isPCParty) -> bool
{
    return isPCParty;
}

// ShouldClearSeekingParty mirrors isSeekingParty() after join.
//
// Formula (slice 3217 dedicated dual-wire; residual expand 2955 / pure 1350 —
// formula unchanged):
//   isSeekingParty
//
// isSeekingParty — host-evaluated PChar->isSeekingParty()
//                  (playerConfig.InviteFlg / LFP flag)
// true  → clear seeking (InviteFlg = false, UPDATE_HP, SaveCharStats)
// false → leave seeking flag unchanged
//
// Dual-wire of Go party.ShouldClearSeekingParty
// (internal/party/clear_seeking.go).
// Call site: CParty::AddMember PC post-process host inject.
// Residual dual-wire suite: 2955 / test_party_clear_seeking_2955.
// Dedicated dual-wire suite is test_party_clear_seeking_party_3217. Formula is
// unchanged; dedicated suite expands free==inline==pin poles + dense 2^1.
inline auto ShouldClearSeekingParty(const bool isSeekingParty) -> bool
{
    return isSeekingParty;
}

// ShouldApplyPartyLevelSyncOnJoin mirrors m_PSyncTarget != nullptr after join.
//
// Formula (slice 3305 dedicated dual-wire; residual expand 2955 / prior
// dedicated 3274 / pure 1350 — formula unchanged):
//   hasSyncTarget
//
// hasSyncTarget — host-evaluated m_PSyncTarget != nullptr
// true  → apply level sync on join (host still owns same-zone LevelSync
//         message / status / CharSync side effects)
// false → leave level-sync state unchanged for the joiner
//
// Dual-wire of Go party.ShouldApplyPartyLevelSyncOnJoin
// (internal/party/apply_level_sync_on_join.go).
// Call site: CParty::AddMember PC post-process host inject.
// Residual dual-wire expand: 2955 (sibling residual pins under
// test_party_clear_seeking_2955 / residual AddMember suite).
// Prior dedicated dual-wire suite: test_party_level_sync_on_join_3274.
// Dedicated dual-wire suite is test_party_level_sync_on_join_3305. Formula is
// unchanged; dedicated suite expands free==inline==pin poles + dense 2^1.
inline auto ShouldApplyPartyLevelSyncOnJoin(const bool hasSyncTarget) -> bool
{
    return hasSyncTarget;
}

// --- Out-of-zone AddMember(uint32) ---

// ShouldRunOutOfZoneAddMember mirrors m_PartyType == PARTY_PCS for AddMember(id).
inline auto ShouldRunOutOfZoneAddMember(const bool isPCParty) -> bool
{
    return isPCParty;
}

// FormatAddMemberOutOfZoneFullWarning mirrors the out-of-zone full-party warning.
inline auto FormatAddMemberOutOfZoneFullWarning() -> std::string
{
    return "CParty::AddMember() - Party was full when trying to add a member from out of zone.";
}

// PartySecondFlag is PARTY_SECOND (0x0001).
constexpr uint16 PartySecondFlag = 0x0001;

// PartyThirdFlag is PARTY_THIRD (0x0002).
constexpr uint16 PartyThirdFlag = 0x0002;

// OutOfZoneAddMemberFlags mirrors alliance party-number flags for remote add.
// Without alliance, flags stay 0 (new member is neither SECOND nor THIRD).
// partyNumber 1 → PARTY_SECOND; 2 → PARTY_THIRD; else 0.
inline auto OutOfZoneAddMemberFlags(const bool hasAlliance, const uint8 partyNumber) -> uint16
{
    if (!hasAlliance)
    {
        return 0;
    }
    if (partyNumber == 1)
    {
        return PartySecondFlag;
    }
    if (partyNumber == 2)
    {
        return PartyThirdFlag;
    }
    return 0;
}

// --- DelMember / PopMember / PushMember admission ---

// entity_party_gate is shared null/mismatch admission for DelMember and PopMember.
enum class entity_party_gate : uint8_t
{
    REJECT_NULL_OR_MISMATCH,
    PROCEED,
};

// ClassifyEntityPartyMatch mirrors DelMember/PopMember null or PParty != this.
inline auto ClassifyEntityPartyMatch(const bool entityNull, const bool partyMismatch) -> entity_party_gate
{
    if (entityNull || partyMismatch)
    {
        return entity_party_gate::REJECT_NULL_OR_MISMATCH;
    }
    return entity_party_gate::PROCEED;
}

// FormatDelMemberNullWarning mirrors DelMember null/mismatch warning.
inline auto FormatDelMemberNullWarning() -> std::string
{
    return "CParty::DelMember() - PEntity was null, or PParty mismatch.";
}

// FormatPopMemberNullWarning mirrors PopMember null/mismatch warning.
inline auto FormatPopMemberNullWarning() -> std::string
{
    return "CParty::PopMember() - PEntity was null, or PParty mismatch.";
}

// del_member_path is DelMember's leader vs non-leader branch after admission.
enum class del_member_path : uint8_t
{
    AS_LEADER,   // m_PLeader == PEntity → RemovePartyLeader
    NON_LEADER,  // find + PC cleanup / erase
};

// ClassifyDelMemberPath mirrors m_PLeader == PEntity after admission.
inline auto ClassifyDelMemberPath(const bool isLeader) -> del_member_path
{
    return isLeader ? del_member_path::AS_LEADER : del_member_path::NON_LEADER;
}

// ShouldReloadPartyAfterLeaderDel mirrors RemovePartyLeader return true.
inline auto ShouldReloadPartyAfterLeaderDel(const bool removePartyLeaderReturnedTrue) -> bool
{
    return removePartyLeaderReturnedTrue;
}

// ShouldDeleteEmptyPartyOnPop mirrors members.empty() after erase in PopMember.
inline auto ShouldDeleteEmptyPartyOnPop(const bool membersEmpty) -> bool
{
    return membersEmpty;
}

// ShouldClearAllianceMainOnPop mirrors getMainParty() == this when dissolving.
inline auto ShouldClearAllianceMainOnPop(const bool hasAlliance, const bool isMainParty) -> bool
{
    return hasAlliance && isMainParty;
}

// push_member_gate is PushMember admission before append.
enum class push_member_gate : uint8_t
{
    REJECT_NULL_OR_HAS_PARTY,
    PROCEED,
};

// ClassifyPushMember mirrors PushMember's null or already-has-party gate.
inline auto ClassifyPushMember(const bool entityNull, const bool alreadyHasParty) -> push_member_gate
{
    if (entityNull || alreadyHasParty)
    {
        return push_member_gate::REJECT_NULL_OR_HAS_PARTY;
    }
    return push_member_gate::PROCEED;
}

// FormatPushMemberNullWarning mirrors PushMember null/has-party warning.
inline auto FormatPushMemberNullWarning() -> std::string
{
    return "CParty::PushMember() - PEntity was null, or PParty not null.";
}

// PartySyncFlag is PARTY_SYNC (0x0100).
constexpr uint16 PartySyncFlag = 0x0100;

// ShouldAssignLeaderFromFlags mirrors memberinfo.flags & PARTY_LEADER.
inline auto ShouldAssignLeaderFromFlags(const uint16 flags) -> bool
{
    return (flags & PartyLeaderFlag) != 0;
}

// ShouldAssignQuarterMasterFromFlags mirrors memberinfo.flags & PARTY_QM.
inline auto ShouldAssignQuarterMasterFromFlags(const uint16 flags) -> bool
{
    return (flags & PartyQMFlag) != 0;
}

// ShouldAssignSyncTargetFromFlags mirrors memberinfo.flags & PARTY_SYNC.
inline auto ShouldAssignSyncTargetFromFlags(const uint16 flags) -> bool
{
    return (flags & PartySyncFlag) != 0;
}

// MemberInfoMatchesEntity mirrors memberinfo.id == PEntity->id for role restore.
inline auto MemberInfoMatchesEntity(const uint32 memberInfoID, const uint32 entityID) -> bool
{
    return memberInfoID == entityID;
}

// --- ReloadParty / ReloadPartyMembers / ReloadTreasurePool ---

// ShouldSkipMobReloadParty mirrors PARTY_MOBS early return in ReloadParty.
inline auto ShouldSkipMobReloadParty(const bool isMobParty) -> bool
{
    return isMobParty;
}

// reload_party_path is alliance vs solo-party packet fan-out.
enum class reload_party_path : uint8_t
{
    ALLIANCE, // m_PAlliance != nullptr
    PARTY,    // regular party
};

// ClassifyReloadPartyPath mirrors m_PAlliance != nullptr after mob skip.
inline auto ClassifyReloadPartyPath(const bool hasAlliance) -> reload_party_path
{
    return hasAlliance ? reload_party_path::ALLIANCE : reload_party_path::PARTY;
}

// FormatReloadPartyMembersNullWarning mirrors ReloadPartyMembers null gate.
inline auto FormatReloadPartyMembersNullWarning() -> std::string
{
    return "CParty::ReloadPartyMembers() - PChar was null.";
}

// ShouldRejectNullReloadPartyMembers mirrors PChar == nullptr.
inline auto ShouldRejectNullReloadPartyMembers(const bool charNull) -> bool
{
    return charNull;
}

// AlliancePartySlotMask is PARTY_SECOND | PARTY_THIRD for list index resets.
constexpr uint16 AlliancePartySlotMask = PartySecondFlag | PartyThirdFlag;

// ShouldResetAllianceListIndex mirrors
// (memberinfo.flags & (PARTY_SECOND|PARTY_THIRD)) != alliance cursor.
inline auto ShouldResetAllianceListIndex(const uint16 memberFlags, const uint16 allianceCursor) -> bool
{
    return (memberFlags & AlliancePartySlotMask) != allianceCursor;
}

// NextAllianceListCursor mirrors alliance = memberinfo.flags & mask.
inline auto NextAllianceListCursor(const uint16 memberFlags) -> uint16
{
    return memberFlags & AlliancePartySlotMask;
}

// reload_party_member_list_position is the client group-list position carried
// between rows. Alliance slot changes start a new client list at index zero.
struct reload_party_member_list_position
{
    uint16 allianceCursor = 0;
    uint8  listIndex      = 0;
};

// BeginReloadPartyMemberListRow applies the alliance-slot reset before a
// GROUP_LIST row is emitted.
inline auto BeginReloadPartyMemberListRow(
    reload_party_member_list_position position,
    const uint16                      memberFlags) -> reload_party_member_list_position
{
    if (ShouldResetAllianceListIndex(memberFlags, position.allianceCursor))
    {
        position.allianceCursor = NextAllianceListCursor(memberFlags);
        position.listIndex      = 0;
    }
    return position;
}

// AdvanceReloadPartyMemberListRow advances after a GROUP_LIST row is emitted.
inline auto AdvanceReloadPartyMemberListRow(reload_party_member_list_position position) -> reload_party_member_list_position
{
    position.listIndex++;
    return position;
}

// solo_reload_party_row_plan is the GROUP_LIST index range consumed by one
// regular-party ReloadParty roster row and its leader-trust injections.
struct solo_reload_party_row_plan
{
    uint8 memberIndex = 0;
    uint8 nextIndex   = 0;
};

// PlanSoloReloadPartyRow mirrors ReloadParty's index progression. Offline
// roster rows do not inject leader trusts; online rows inject every trust
// between their own row and the next roster row.
inline auto PlanSoloReloadPartyRow(
    const uint8       memberIndex,
    const bool        memberOnline,
    const std::size_t leaderTrustCount) -> solo_reload_party_row_plan
{
    uint8 nextIndex = memberIndex;
    if (memberOnline)
    {
        for (std::size_t i = 0; i < leaderTrustCount; ++i)
        {
            nextIndex++;
        }
    }
    nextIndex++;
    return {
        .memberIndex = memberIndex,
        .nextIndex   = nextIndex,
    };
}

// OfflineMemberZoneID mirrors zone == 0 ? prev_zone : zone for offline list rows.
inline auto OfflineMemberZoneID(const uint16 zone, const uint16 prevZone) -> uint16
{
    return zone == 0 ? prevZone : zone;
}

// FormatReloadTreasurePoolNullWarning mirrors ReloadTreasurePool null gate.
inline auto FormatReloadTreasurePoolNullWarning() -> std::string
{
    return "CParty::ReloadTreasurePool() - PChar was null.";
}

// ShouldRejectNullReloadTreasurePool mirrors PChar == nullptr.
inline auto ShouldRejectNullReloadTreasurePool(const bool charNull) -> bool
{
    return charNull;
}

// ShouldKeepZoneTreasurePool mirrors non-null zone-type pool early return.
inline auto ShouldKeepZoneTreasurePool(const bool hasPool, const bool isZonePool) -> bool
{
    return hasPool && isZonePool;
}

// reload_treasure_scan is which roster to scan for a joinable pool.
enum class reload_treasure_scan : uint8_t
{
    NONE,     // no party — fall through to solo create
    ALLIANCE, // has party and alliance
    PARTY,    // has party, no alliance
};

// ClassifyReloadTreasureScan mirrors party/alliance structure before member walk.
inline auto ClassifyReloadTreasureScan(const bool hasParty, const bool hasAlliance) -> reload_treasure_scan
{
    if (!hasParty)
    {
        return reload_treasure_scan::NONE;
    }
    if (hasAlliance)
    {
        return reload_treasure_scan::ALLIANCE;
    }
    return reload_treasure_scan::PARTY;
}

// ShouldJoinMemberTreasurePool mirrors candidate != self, has pool, same zone.
inline auto ShouldJoinMemberTreasurePool(const bool isSelf, const bool candidateHasPool, const bool sameZone) -> bool
{
    return !isSelf && candidateHasPool && sameZone;
}

// ShouldDelOwnPoolBeforeJoin mirrors PChar->PTreasurePool != nullptr before reassign.
inline auto ShouldDelOwnPoolBeforeJoin(const bool hasOwnPool) -> bool
{
    return hasOwnPool;
}

// ShouldCreateSoloTreasurePool mirrors PTreasurePool == nullptr at end of reload.
inline auto ShouldCreateSoloTreasurePool(const bool hasPool) -> bool
{
    return !hasPool;
}

// ShouldRefreshFlagsForParty mirrors memberinfo.partyid == m_PartyID.
inline auto ShouldRefreshFlagsForParty(const uint32 memberInfoPartyID, const uint32 partyID) -> bool
{
    return memberInfoPartyID == partyID;
}

// ShouldAssignAllianceLeaderFromFlags mirrors flags & ALLIANCE_LEADER when alliance set.
inline auto ShouldAssignAllianceLeaderFromFlags(const uint16 flags, const bool hasAlliance) -> bool
{
    return hasAlliance && (flags & AllianceLeaderFlag) != 0;
}

// --- GetMemberFlags / CParty constructor ---

// FormatGetMemberFlagsNullWarning mirrors GetMemberFlags null/mismatch warning.
inline auto FormatGetMemberFlagsNullWarning() -> std::string
{
    return "CParty::GetMemberFlags() - PEntity was null, or PParty mismatch.";
}

// ShouldRejectGetMemberFlags mirrors PEntity null or PParty != this.
inline auto ShouldRejectGetMemberFlags(const bool entityNull, const bool partyMismatch) -> bool
{
    return entityNull || partyMismatch;
}

// IsAllianceLeaderForFlags mirrors has alliance && entity is leader && this is main.
inline auto IsAllianceLeaderForFlags(const bool hasAlliance, const bool isLeader, const bool isMainParty) -> bool
{
    return hasAlliance && isLeader && isMainParty;
}

// MemberFlags assembles PARTYFLAG bits the same way as CParty::GetMemberFlags
// once role/alliance decisions are known:
//  1. ALLIANCE_LEADER when isAllianceLeader
//  2. PARTY_SECOND when partyNumber == 1, PARTY_THIRD when partyNumber == 2
//  3. PARTY_LEADER / PARTY_QM / PARTY_SYNC for matching roles
// LSB uses += for SECOND/THIRD and |= for the role bits.
inline auto MemberFlags(
    const uint8 partyNumber,
    const bool  isLeader,
    const bool  isQM,
    const bool  isSync,
    const bool  isAllianceLeader) -> uint16
{
    uint16 flags = 0;

    if (isAllianceLeader)
    {
        flags |= AllianceLeaderFlag;
    }

    if (partyNumber == 1)
    {
        flags += PartySecondFlag;
    }
    else if (partyNumber == 2)
    {
        flags += PartyThirdFlag;
    }

    if (isLeader)
    {
        flags |= PartyLeaderFlag;
    }
    if (isQM)
    {
        flags |= PartyQMFlag;
    }
    if (isSync)
    {
        flags |= PartySyncFlag;
    }

    return flags;
}

// ShouldInitPartyFromEntity mirrors PEntity != null && PParty == nullptr in CParty ctor.
inline auto ShouldInitPartyFromEntity(const bool entityNull, const bool alreadyHasParty) -> bool
{
    return !entityNull && !alreadyHasParty;
}

// FormatCPartyCtorNullWarning mirrors constructor null/has-party warning.
inline auto FormatCPartyCtorNullWarning() -> std::string
{
    return "CParty::CParty() - PEntity was null, or party was not null.";
}

// ResolvePartyTypeIsPC mirrors objtype == TYPE_PC ? PARTY_PCS : PARTY_MOBS
// (true → PC party type).
inline auto ResolvePartyTypeIsPC(const bool isPCEntity) -> bool
{
    return isPCEntity;
}

// PartyIDFromEntity mirrors m_PartyID = PEntity->id on construct.
inline auto PartyIDFromEntity(const uint32 entityID) -> uint32
{
    return entityID;
}

} // namespace partyhelpers
