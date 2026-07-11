#pragma once

#include "common/cbasetypes.h"
#include "items.h"
#include "items/item_linkshell.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure CLinkshell admission / rank / push policy extracted so native tests can
// pin behavior without DB, packets, or entity pointers.

namespace linkshellhelpers
{

// --- AddMember ---

// ShouldRejectNullAddMember mirrors PChar == nullptr.
inline auto ShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

// ShouldRejectDuplicateAddMember mirrors find hit in online members.
inline auto ShouldRejectDuplicateAddMember(const bool alreadyInList) -> bool
{
    return alreadyInList;
}

// FormatAddMemberAlreadyWarning mirrors duplicate online-member warning.
inline auto FormatAddMemberAlreadyWarning(const std::string& name) -> std::string
{
    return fmt::format("CLinkshell::AddMember attempted to add member '{}' who is already in the online member list.", name);
}

// IsLinkshellSlot1 mirrors lsNum == 1 (else branch is LS2).
inline auto IsLinkshellSlot1(const uint8 lsNum) -> bool
{
    return lsNum == 1;
}

// --- setMessage ---

// ShouldSendLinkshellMessageIPC mirrors message.size() != 0 after DB update.
inline auto ShouldSendLinkshellMessageIPC(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

// --- ChangeMemberRank ---

// RankChangePearlToSack is newRank 2 (pearl → sack).
constexpr uint8 RankChangePearlToSack = 2;

// RankChangeSackToPearl is newRank 3 (sack → pearl).
constexpr uint8 RankChangeSackToPearl = 3;

// IsValidRankChangeNewRank mirrors newRank < 2 || newRank > 3 rejection.
inline auto IsValidRankChangeNewRank(const uint8 newRank) -> bool
{
    return newRank >= RankChangePearlToSack && newRank <= RankChangeSackToPearl;
}

// IsValidRankChangeRequester mirrors requesterRank == LSTYPE_LINKSHELL.
inline auto IsValidRankChangeRequester(const uint8 requesterRank) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL;
}

// ResolveRankChangeItemID mirrors (ITEMID::LINKSHELL + newRank) - 1.
inline auto ResolveRankChangeItemID(const uint8 newRank) -> uint16
{
    return static_cast<uint16>((ITEMID::LINKSHELL + newRank) - 1);
}

// IsValidRankChangeItemID mirrors newId == PEARLSACK || LINKPEARL.
inline auto IsValidRankChangeItemID(const uint16 newId) -> bool
{
    return newId == ITEMID::PEARLSACK || newId == ITEMID::LINKPEARL;
}

// ResolveLSTypeFromRankItemID maps PEARLSACK/LINKPEARL item ids to LSTYPE.
inline auto ResolveLSTypeFromRankItemID(const uint16 newId) -> uint8
{
    return newId == ITEMID::PEARLSACK ? static_cast<uint8>(LSTYPE_PEARLSACK) : static_cast<uint8>(LSTYPE_LINKPEARL);
}

// FormatChangeMemberRankError mirrors invalid rank/requester error text shape.
inline auto FormatChangeMemberRankError(const std::string& memberName, const uint32 linkshellId) -> std::string
{
    return fmt::format("CLinkshell::ChangeMemberRank: Invalid rank change request for member '{}' in linkshell {}.", memberName, linkshellId);
}

// IsLinkshell2Attachment mirrors PMember->PLinkshell2 == this.
inline auto IsLinkshell2Attachment(const bool isLS2Pointer) -> bool
{
    return isLS2Pointer;
}

// --- RemoveMemberByName inventory break policy ---

// ShouldBreakInventoryPearl mirrors requester is shell holder OR item is equipped one.
inline auto ShouldBreakInventoryPearl(const uint8 requesterRank, const bool isEquippedItem) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL || isEquippedItem;
}

// ShouldMarkPearlBroken mirrors GetLSType() != LSTYPE_LINKSHELL before break.
inline auto ShouldMarkPearlBroken(const uint8 lsType) -> bool
{
    return lsType != LSTYPE_LINKSHELL;
}

// ShouldSendBreakMessage mirrors breakLinkshell true → NoLongerExists else Kicked.
inline auto ShouldSendBreakMessage(const bool breakLinkshell) -> bool
{
    return breakLinkshell;
}

// --- PushPacket ---

// ShouldReceiveLinkshellPacket mirrors id != sender && not disappear && not prison.
inline auto ShouldReceiveLinkshellPacket(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

// ShouldRewritePacketAsLinkshell2 mirrors member->PLinkshell2 == this.
inline auto ShouldRewritePacketAsLinkshell2(const bool memberIsLS2) -> bool
{
    return memberIsLS2;
}

// MessageLinkshell2 is MESSAGE_LINKSHELL2 for chat_std rewrite at offset 0x04.
constexpr uint8 MessageLinkshell2 = 27;

// LinkshellMessageLS2Flag is the 0x40 bit OR'd into LS message packets at 0x05.
constexpr uint8 LinkshellMessageLS2Flag = 0x40;

// IsChatStdPacketType mirrors getType() == GP_SERV_COMMAND_CHAT_STD (0x017).
// Host supplies the type equality; this documents the MESSAGE rewrite value.
inline auto ChatStdMessageTypeForLS2() -> uint8
{
    return MessageLinkshell2;
}

// ApplyLinkshellMessageLS2Flag mirrors ref<uint8>(0x05) |= 0x40.
inline auto ApplyLinkshellMessageLS2Flag(const uint8 existingByte) -> uint8
{
    return static_cast<uint8>(existingByte | LinkshellMessageLS2Flag);
}

// ShouldPushStoredLinkshellMessage mirrors !message.empty() after DB load.
inline auto ShouldPushStoredLinkshellMessage(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

// DelMemberRemaining mirrors return !members.empty() after erase attempt.
inline auto DelMemberRemaining(const std::size_t memberCountAfter) -> bool
{
    return memberCountAfter != 0;
}

} // namespace linkshellhelpers
