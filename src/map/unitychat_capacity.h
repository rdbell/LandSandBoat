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
//
// Production host: CUnityChat::PushPacket (unitychat.cpp) injects
// member->id == senderID / STATUS_TYPE::DISAPPEAR / jailutils::InPrison(member)
// into ShouldReceiveUnityPacket.
// Go dual-wire: unitychat.ShouldReceiveUnityPacket
// (internal/unitychat/receive_packet.go).

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

// ShouldRejectNullOnlineMember mirrors PChar == nullptr.
inline auto ShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

// ShouldLoadUnityChatOnOnlineAdd mirrors cache miss AND leader != 0.
inline auto ShouldLoadUnityChatOnOnlineAdd(const bool foundInCache, const uint32 leader) -> bool
{
    return !foundInCache && leader != 0;
}

// ShouldAddMemberAfterOnlineLookup mirrors PUnity != nullptr.
inline auto ShouldAddMemberAfterOnlineLookup(const bool unityLoaded) -> bool
{
    return unityLoaded;
}

// OnlineMemberAlwaysReturnsFalse documents AddOnlineMember/DelOnlineMember always false.
inline auto OnlineMemberAlwaysReturnsFalse() -> bool
{
    return false;
}

// ShouldEraseUnityChatAfterDelOnline mirrors !DelMember (roster empty).
inline auto ShouldEraseUnityChatAfterDelOnline(const bool delMemberRemaining) -> bool
{
    return !delMemberRemaining;
}

// ShouldReturnCachedUnityChat mirrors GetUnityChat find hit.
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
