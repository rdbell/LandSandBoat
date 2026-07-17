#pragma once

#include "common/cbasetypes.h"
#include "items.h"
#include "items/item_linkshell.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure CLinkshell admission / rank / push policy extracted so native tests can
// pin behavior without DB, packets, or entity pointers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1354: AddMember / rank / push / DelMember capacity suite
//   - 2929: ShouldRejectNullAddMember (charNull identity)
//   - 2958: ShouldRejectDuplicateAddMember (alreadyInList identity)
//   - 2977: ShouldSendLinkshellMessageIPC (messageNonEmpty identity)
//   - 2993: ShouldPushStoredLinkshellMessage (messageNonEmpty identity)
//
// Production host: CLinkshell::AddMember (linkshell.cpp) injects
// PChar == nullptr into ShouldRejectNullAddMember before duplicate / slot work,
// then injects find-hit into ShouldRejectDuplicateAddMember.
// CLinkshell::setMessage injects message.size() != 0 into
// ShouldSendLinkshellMessageIPC after DB update before IPC send.
// CLinkshell::PushLinkshellMessage injects !message.empty() into
// ShouldPushStoredLinkshellMessage after DB load before packet push.
// Go dual-wire: linkshell.ShouldRejectNullAddMember
// (internal/linkshell/reject_null_add_member.go),
// linkshell.ShouldRejectDuplicateAddMember
// (internal/linkshell/reject_duplicate_add_member.go),
// linkshell.ShouldSendLinkshellMessageIPC
// (internal/linkshell/send_message_ipc.go),
// linkshell.ShouldPushStoredLinkshellMessage
// (internal/linkshell/push_stored_message.go).

namespace linkshellhelpers
{

// --- AddMember ---

// ShouldRejectNullAddMember mirrors PChar == nullptr.
//
// Formula (slice 2929 dual-wire):
//   charNull
//
// charNull — host-evaluated PChar == nullptr
// true  → reject AddMember (early return; no roster/DB work)
// false → null gate passes; host continues to duplicate-member checks
//
// Dual-wire of Go linkshell.ShouldRejectNullAddMember.
// Call site: CLinkshell::AddMember host inject (PChar == nullptr).
inline auto ShouldRejectNullAddMember(const bool charNull) -> bool
{
    return charNull;
}

// ShouldRejectDuplicateAddMember mirrors find hit in online members.
//
// Formula (slice 2958 dual-wire):
//   alreadyInList
//
// alreadyInList — host-evaluated
//   std::find(members.begin(), members.end(), PChar) != members.end()
// true  → reject AddMember (warn + early return; no roster/DB work)
// false → duplicate gate passes; host continues to slot attach / DB update
//
// Dual-wire of Go linkshell.ShouldRejectDuplicateAddMember.
// Call site: CLinkshell::AddMember host inject (find hit in members).
// Evaluated only after ShouldRejectNullAddMember passes (slice 2929).
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
//
// Formula (slice 2977 dual-wire):
//   messageNonEmpty
//
// messageNonEmpty — host-evaluated message.size() != 0
// true  → host may send ipc::LinkshellSetMessage after DB update
// false → skip IPC (empty motd still updates poster/time in DB; no fan-out)
//
// Dual-wire of Go linkshell.ShouldSendLinkshellMessageIPC.
// Call site: CLinkshell::setMessage host inject (message.size() != 0).
// Prior pure port: slices 1354 / 2171 (capacity suite; setMessage value model).
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
// Host uses this for RejectInvalidNewRank / RejectInvalidRequester dispositions.
inline auto FormatChangeMemberRankError(const std::string& memberName, const uint32 linkshellId) -> std::string
{
    return fmt::format("CLinkshell::ChangeMemberRank: Invalid rank change request for member '{}' in linkshell {}.", memberName, linkshellId);
}

// ChangeMemberRankPreflight is the pure early-gate disposition of
// CLinkshell::ChangeMemberRank before the online-member inventory loop.
enum class ChangeMemberRankPreflight : uint8
{
    RejectInvalidNewRank   = 0, // newRank not in [2, 3]; host logs error
    RejectInvalidRequester = 1, // requesterRank != LSTYPE_LINKSHELL; host logs error
    SkipInvalidItemID      = 2, // resolved item id not PEARLSACK/LINKPEARL; silent skip
    Proceed                = 3, // enter member loop with newItemID
};

// ChangeMemberRankPlan is the pure preflight outcome for ChangeMemberRank.
// newItemID is set when disposition == Proceed.
struct ChangeMemberRankPlan
{
    ChangeMemberRankPreflight disposition{};
    uint16                    newItemID{};
};

// PlanChangeMemberRank short-circuits in production ChangeMemberRank order:
// 1) !IsValidRankChangeNewRank(newRank) -> RejectInvalidNewRank
// 2) !IsValidRankChangeRequester(requesterRank) -> RejectInvalidRequester
// 3) ResolveRankChangeItemID; !IsValidRankChangeItemID -> SkipInvalidItemID
// 4) Proceed with newItemID
// Composes IsValidRankChangeNewRank / IsValidRankChangeRequester /
// ResolveRankChangeItemID / IsValidRankChangeItemID. Logging stays host-side.
inline auto PlanChangeMemberRank(const uint8 newRank, const uint8 requesterRank) -> ChangeMemberRankPlan
{
    if (!IsValidRankChangeNewRank(newRank))
    {
        return ChangeMemberRankPlan{ ChangeMemberRankPreflight::RejectInvalidNewRank, 0 };
    }
    if (!IsValidRankChangeRequester(requesterRank))
    {
        return ChangeMemberRankPlan{ ChangeMemberRankPreflight::RejectInvalidRequester, 0 };
    }
    const auto newId = ResolveRankChangeItemID(newRank);
    if (!IsValidRankChangeItemID(newId))
    {
        return ChangeMemberRankPlan{ ChangeMemberRankPreflight::SkipInvalidItemID, 0 };
    }
    return ChangeMemberRankPlan{ ChangeMemberRankPreflight::Proceed, newId };
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
//
// Formula (slice 2993 dual-wire):
//   messageNonEmpty
//
// messageNonEmpty — host-evaluated !message.empty()
// true  → host may push GP_SERV_COMMAND_LINKSHELL_MESSAGE after DB load
// false → skip push (empty stored motd; TODO residual "No linkshell message set.")
//
// Dual-wire of Go linkshell.ShouldPushStoredLinkshellMessage.
// Call site: CLinkshell::PushLinkshellMessage host inject (!message.empty()).
// Prior pure port: slice 1354 (capacity suite push gate).
// Sibling setMessage IPC gate: slice 2977 (same identity formula; different host).
inline auto ShouldPushStoredLinkshellMessage(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

// DelMemberRemaining mirrors return !members.empty() after erase attempt.
inline auto DelMemberRemaining(const std::size_t memberCountAfter) -> bool
{
    return memberCountAfter != 0;
}

// --- DelMember session clear ---

// LinkshellClearAttachment is the pure DelMember session-clear branch after a
// matching online member is found. DB clear and PLinkshell nulling stay host-side.
enum class LinkshellClearAttachment : uint8
{
    None     = 0, // neither PLinkshell1 nor PLinkshell2 matches this shell
    ClearLS1 = 1, // clear LS1 session fields and PLinkshell1
    ClearLS2 = 2, // clear LS2 session fields and PLinkshell2
};

// PlanLinkshellDelMemberClear mirrors production DelMember if/else-if order:
// 1) isLS1Attachment (PLinkshell1 == this) -> ClearLS1
// 2) else if isLS2Attachment (PLinkshell2 == this) -> ClearLS2
// 3) else None
// Note: LS1 wins when both are somehow true (if / else if).
inline auto PlanLinkshellDelMemberClear(const bool isLS1Attachment, const bool isLS2Attachment) -> LinkshellClearAttachment
{
    if (isLS1Attachment)
    {
        return LinkshellClearAttachment::ClearLS1;
    }
    if (isLS2Attachment)
    {
        return LinkshellClearAttachment::ClearLS2;
    }
    return LinkshellClearAttachment::None;
}

// --- Registry: Load / Unload / Online / Register ---

// load_linkshell_gate is pure outcome of LoadLinkshell DB lookup.
enum class load_linkshell_gate : uint8_t
{
    NOT_FOUND, // query fail, no rows, or next fail
    FOUND,     // construct and insert into LinkshellList
};

// ClassifyLoadLinkshell mirrors rset && rowsCount && next.
inline auto ClassifyLoadLinkshell(const bool queryOk, const bool hasRow) -> load_linkshell_gate
{
    if (queryOk && hasRow)
    {
        return load_linkshell_gate::FOUND;
    }
    return load_linkshell_gate::NOT_FOUND;
}

// ShouldUnloadLinkshell mirrors find hit in LinkshellList.
inline auto ShouldUnloadLinkshell(const bool foundInList) -> bool
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

// ShouldProcessLinkshellItem mirrors item != null && isType(ITEM_LINKSHELL).
inline auto ShouldProcessLinkshellItem(const bool itemNonNull, const bool isLinkshellType) -> bool
{
    return itemNonNull && isLinkshellType;
}

// ShouldLoadLinkshellOnOnlineAdd mirrors cache miss before LoadLinkshell.
inline auto ShouldLoadLinkshellOnOnlineAdd(const bool foundInCache) -> bool
{
    return !foundInCache;
}

// ShouldAddMemberAfterOnlineLookup mirrors PLinkshell != nullptr.
inline auto ShouldAddMemberAfterOnlineLookup(const bool linkshellLoaded) -> bool
{
    return linkshellLoaded;
}

// OnlineMemberAlwaysReturnsFalse documents AddOnlineMember/DelOnlineMember
// always return false regardless of work done (LSB quirk).
inline auto OnlineMemberAlwaysReturnsFalse() -> bool
{
    return false;
}

// ShouldEraseLinkshellAfterDelOnline mirrors !DelMember (roster empty).
inline auto ShouldEraseLinkshellAfterDelOnline(const bool delMemberRemaining) -> bool
{
    return !delMemberRemaining;
}

// IsValidLinkshellNameFromQuery mirrors !rset || rowsCount == 0 (name free).
inline auto IsValidLinkshellNameFromQuery(const bool queryOk, const uint32 rowCount) -> bool
{
    return !queryOk || rowCount == 0;
}

// RegisterNewLinkshellPostRights is the INSERT postrights value LSTYPE_PEARLSACK.
// Production stores LSTYPE (rank) enum into the postrights column for new shells.
constexpr uint8 RegisterNewLinkshellPostRights = static_cast<uint8>(LSTYPE_PEARLSACK);

// ShouldAttemptRegisterInsert mirrors IsValidLinkshellName true.
inline auto ShouldAttemptRegisterInsert(const bool nameValid) -> bool
{
    return nameValid;
}

// ClassifyRegisterNewLinkshell is ordered outcome after name validation.
enum class register_linkshell_gate : uint8_t
{
    REJECT_NAME,   // name taken / invalid query path
    REJECT_INSERT, // insert failed
    REJECT_SELECT, // post-insert select failed
    REJECT_LOAD,   // LoadLinkshell returned null
    SUCCESS,       // return loaded id
};

// ClassifyRegisterNewLinkshell mirrors RegisterNewLinkshell control flow.
// nameValid is IsValidLinkshellName; insertOk/selectOk/hasRow/loadOk are host results.
inline auto ClassifyRegisterNewLinkshell(
    const bool nameValid,
    const bool insertOk,
    const bool selectOk,
    const bool hasRow,
    const bool loadOk) -> register_linkshell_gate
{
    if (!nameValid)
    {
        return register_linkshell_gate::REJECT_NAME;
    }
    if (!insertOk)
    {
        return register_linkshell_gate::REJECT_INSERT;
    }
    if (!selectOk || !hasRow)
    {
        return register_linkshell_gate::REJECT_SELECT;
    }
    if (!loadOk)
    {
        return register_linkshell_gate::REJECT_LOAD;
    }
    return register_linkshell_gate::SUCCESS;
}

// ShouldReturnCachedLinkshell mirrors GetLinkshell find hit.
inline auto ShouldReturnCachedLinkshell(const bool foundInList) -> bool
{
    return foundInList;
}

} // namespace linkshellhelpers
