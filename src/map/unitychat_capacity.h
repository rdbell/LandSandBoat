#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <string>

// Pure CUnityChat / unitychat namespace policy extracted for native tests.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1356: DelMemberRemaining, registry / online-member gates, residual capacity
//   - 2933: ShouldReceiveUnityPacket (!isSender && !isDisappear && !inPrison)
//   - 3050: ShouldLoadUnityChatOnOnlineAdd (!foundInCache && leader != 0)
//   - 3075: ShouldRejectNullOnlineMember (charNull identity null-PChar gate)
//   - 3096: ShouldAddMemberAfterOnlineLookup (unityLoaded identity post-lookup gate)
//   - 3116: ShouldEraseUnityChatAfterDelOnline (!delMemberRemaining roster-empty erase)
//   - 3130: ShouldReturnCachedUnityChat (foundInList identity)
//
// Production host: CUnityChat::PushPacket (unitychat.cpp) injects
// member->id == senderID / STATUS_TYPE::DISAPPEAR / jailutils::InPrison(member)
// into ShouldReceiveUnityPacket.
// Go dual-wire: unitychat.ShouldReceiveUnityPacket
// (internal/unitychat/receive_packet.go).
// Production host: unitychat::AddOnlineMember (unitychat.cpp) injects
// UnityChatList.find(leader) != end and leader into
// ShouldLoadUnityChatOnOnlineAdd; on true LoadUnityChat(leader).
// Go dual-wire: unitychat.ShouldLoadUnityChatOnOnlineAdd
// (internal/unitychat/load_on_online_add.go).
// Production host: unitychat::AddOnlineMember / DelOnlineMember inject
// (PChar == nullptr) into ShouldRejectNullOnlineMember; on true ShowWarning +
// return OnlineMemberAlwaysReturnsFalse.
// Go dual-wire: unitychat.ShouldRejectNullOnlineMember
// (internal/unitychat/reject_null_online_member.go).
// Production host: unitychat::AddOnlineMember injects (PUnity != nullptr) into
// ShouldAddMemberAfterOnlineLookup; on true PUnity->AddMember(PChar).
// Go dual-wire: unitychat.ShouldAddMemberAfterOnlineLookup
// (internal/unitychat/add_member_after_lookup.go).
// Production host: unitychat::DelOnlineMember injects DelMember(PChar) into
// ShouldEraseUnityChatAfterDelOnline; on true UnityChatList.erase(leader).
// Go dual-wire: unitychat.ShouldEraseUnityChatAfterDelOnline
// (internal/unitychat/erase_after_del_online.go).
// Production host: unitychat::GetUnityChat injects
// UnityChatList.find(leader) != end into ShouldReturnCachedUnityChat; on true
// returns UnityChatList.find(leader)->second.get(); on false returns nullptr.
// Go dual-wire: unitychat.ShouldReturnCachedUnityChat
// (internal/unitychat/return_cached_unitychat.go).

namespace unitychathelpers
{

// DelMemberRemaining mirrors return !members.empty() after erase attempt.
inline auto DelMemberRemaining(const std::size_t memberCountAfter) -> bool
{
    return memberCountAfter != 0;
}

// ShouldReceiveUnityPacket mirrors CUnityChat::PushPacket member filter:
// id != sender && not disappear && not prison.
//
// Formula (slice 2933 dual-wire):
//   !isSender && !isDisappear && !inPrison
//
// isSender    — host-evaluated member->id == senderID
// isDisappear — host-evaluated member->status == STATUS_TYPE::DISAPPEAR
// inPrison    — host-evaluated jailutils::InPrison(member)
// true  → push a packet copy to this online member
// false → skip member (sender, disappeared, or jailed)
//
// Dual-wire of Go unitychat.ShouldReceiveUnityPacket.
// Call site: CUnityChat::PushPacket host inject.
inline auto ShouldReceiveUnityPacket(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

// ShouldUnloadUnityChat mirrors find hit in UnityChatList.
inline auto ShouldUnloadUnityChat(const bool foundInList) -> bool
{
    return foundInList;
}

// FormatOnlineMemberNullWarning mirrors AddOnlineMember/DelOnlineMember null warn.
inline auto FormatOnlineMemberNullWarning() -> std::string
{
    return "PChar is null.";
}

// ShouldRejectNullOnlineMember mirrors PChar == nullptr on AddOnlineMember /
// DelOnlineMember before load / roster work.
//
// Formula (slice 3075 dual-wire):
//   charNull
//
// charNull — host-evaluated (PChar == nullptr)
// true  → host logs FormatOnlineMemberNullWarning and returns
//         OnlineMemberAlwaysReturnsFalse before load / roster work
// false → proceed past the null-char early gate
//
// Dual-wire of Go unitychat.ShouldRejectNullOnlineMember.
// Call sites: unitychat::AddOnlineMember / DelOnlineMember — host injects
// (PChar == nullptr); on true ShowWarning + return OnlineMemberAlwaysReturnsFalse.
// Prior pure port: slice 1356 (unitychat capacity residual). Residual pins
// remain in test_unitychat_capacity_1356; dedicated dual-wire suite is
// test_unity_reject_null_online_3075. Sibling dual-wire:
// ShouldLoadUnityChatOnOnlineAdd (3050). Residual siblings: add-after-lookup,
// always-false return, erase-after-del, null warning string (still 1356).
inline auto ShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

// ShouldLoadUnityChatOnOnlineAdd mirrors cache miss AND leader != 0 on
// AddOnlineMember before LoadUnityChat / create.
//
// Formula (slice 3050 dual-wire):
//   !foundInCache && leader != 0
//
// foundInCache — host: UnityChatList cache hit for leader
// leader       — unity leader id (0 means invalid/no load)
// true  → host LoadUnityChat / create for online add
// false → skip load (cache hit or leader 0)
//
// Dual-wire of Go unitychat.ShouldLoadUnityChatOnOnlineAdd.
// Call site: unitychat::AddOnlineMember — host injects
// UnityChatList.find(leader) != UnityChatList.end() and leader; on true
// LoadUnityChat(leader); on false reuses cache entry when found.
// Prior pure port: slice 1356 (unitychat capacity residual). Residual pins
// remain in test_unitychat_capacity_1356; dedicated dual-wire suite is
// test_unity_load_online_add_3050. Sibling dual-wire: null reject (3075).
// Residual siblings: add-after-lookup, always-false return (still 1356 residual).
inline auto ShouldLoadUnityChatOnOnlineAdd(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

// ShouldAddMemberAfterOnlineLookup mirrors PUnity != nullptr after load/cache
// lookup on AddOnlineMember before AddMember.
//
// Formula (slice 3096 dual-wire):
//   unityLoaded
//
// unityLoaded — host-evaluated (PUnity != nullptr) after cache reuse or
//               LoadUnityChat(leader)
// true  → host calls PUnity->AddMember(PChar)
// false → skip AddMember (null unity after miss + leader 0 or load failure)
//
// Dual-wire of Go unitychat.ShouldAddMemberAfterOnlineLookup.
// Call site: unitychat::AddOnlineMember — host injects (PUnity != nullptr);
// on true AddMember(PChar). Prior pure port: slice 1356 (unitychat capacity
// residual). Residual pins remain in test_unitychat_capacity_1356; dedicated
// dual-wire suite is test_unity_add_member_lookup_3096. Sibling dual-wires
// (leave alone): ShouldLoadUnityChatOnOnlineAdd (3050),
// ShouldRejectNullOnlineMember (3075). Residual siblings: always-false return,
// erase-after-del, null warning string (still 1356).
inline auto ShouldAddMemberAfterOnlineLookup(const bool unityLoaded) -> bool
{
    return unityLoaded;
}

// OnlineMemberAlwaysReturnsFalse documents AddOnlineMember/DelOnlineMember always false.
inline auto OnlineMemberAlwaysReturnsFalse() -> bool
{
    return false;
}

// ShouldEraseUnityChatAfterDelOnline mirrors !DelMember (roster empty) after
// DelOnlineMember removes a character from the online list.
//
// Formula (slice 3116 dual-wire):
//   !delMemberRemaining
//
// delMemberRemaining — host: result of PUnityChat->DelMember(PChar)
//                      (LSB DelMember returns !members.empty() after erase)
// true  → host erases UnityChatList entry for leader (roster empty)
// false → keep UnityChatList entry (online members remain)
//
// Dual-wire of Go unitychat.ShouldEraseUnityChatAfterDelOnline.
// Call site: unitychat::DelOnlineMember — host injects DelMember(PChar);
// on true UnityChatList.erase(leader). Prior pure port: slice 1356
// (unitychat capacity residual). Residual pins remain in
// test_unitychat_capacity_1356; dedicated dual-wire suite is
// test_unity_erase_after_del_3116. Sibling dual-wires (leave alone):
// ShouldLoadUnityChatOnOnlineAdd (3050), ShouldRejectNullOnlineMember (3075),
// ShouldAddMemberAfterOnlineLookup (3096). Residual siblings: always-false
// return, null warning string, exception format (still 1356).
inline auto ShouldEraseUnityChatAfterDelOnline(const bool delMemberRemaining) -> bool
{
    return !delMemberRemaining;
}

// ShouldReturnCachedUnityChat mirrors GetUnityChat find hit.
//
// Formula (slice 3130 dual-wire):
//   foundInList
//
// foundInList — host: UnityChatList.find(leader) != end
// true  → host returns UnityChatList.find(leader)->second.get()
// false → host returns nullptr (not loaded)
//
// Dual-wire of Go unitychat.ShouldReturnCachedUnityChat.
// Call site: unitychat::GetUnityChat — host injects
// UnityChatList.find(leader) != UnityChatList.end(); on true returns
// cached CUnityChat*; on false returns nullptr. Prior pure port: slice 1356
// (unitychat capacity residual). Residual pins remain in
// test_unitychat_capacity_1356; dedicated dual-wire suite is
// test_unity_return_cached_3130. Sibling dual-wires (leave alone):
// ShouldLoadUnityChatOnOnlineAdd (3050), ShouldRejectNullOnlineMember (3075),
// ShouldAddMemberAfterOnlineLookup (3096),
// ShouldEraseUnityChatAfterDelOnline (3116). Residual siblings: always-false
// return, null warning string, exception format, unload (still 1356).
inline auto ShouldReturnCachedUnityChat(const bool foundInList) -> bool
{
    return foundInList;
}

// FormatDelOnlineMemberException mirrors ShowError exception text shape.
inline auto FormatDelOnlineMemberException(const std::string& what) -> std::string
{
    return std::string("unitychat::DelOnlineMember caught exception: ") + what;
}

} // namespace unitychathelpers
