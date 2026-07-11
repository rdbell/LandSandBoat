#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <string>

// Pure CUnityChat / unitychat namespace policy extracted for native tests.

namespace unitychathelpers
{

// DelMemberRemaining mirrors return !members.empty() after erase attempt.
inline auto DelMemberRemaining(const std::size_t memberCountAfter) -> bool
{
    return memberCountAfter != 0;
}

// ShouldReceiveUnityPacket mirrors id != sender && not disappear && not prison.
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
