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
//   - 1355: registry load / online / register residual capacity
//   - 2929: ShouldRejectNullAddMember (charNull identity)
//   - 2958: ShouldRejectDuplicateAddMember (alreadyInList identity)
//   - 2977: ShouldSendLinkshellMessageIPC (messageNonEmpty identity)
//   - 2993: ShouldPushStoredLinkshellMessage (messageNonEmpty identity)
//   - 3001: ShouldBreakInventoryPearl (shell holder OR equipped item)
//   - 3008: ShouldMarkPearlBroken (lsType != LSTYPE_LINKSHELL)
//   - 3009: ShouldSendBreakMessage (breakLinkshell identity)
//   - 3017: ShouldReceiveLinkshellPacket (!isSender && !isDisappear && !inPrison)
//   - 3026: ShouldRewritePacketAsLinkshell2 (memberIsLS2 identity)
//   - 3055: ShouldLoadLinkshellOnOnlineAdd (!foundInCache)
//   - 3079: ShouldRejectNullOnlineMember (charNull identity null-PChar gate)
//   - 3099: ShouldProcessLinkshellItem (itemNonNull && isLinkshellType)
//
// Production host: CLinkshell::AddMember (linkshell.cpp) injects
// PChar == nullptr into ShouldRejectNullAddMember before duplicate / slot work,
// then injects find-hit into ShouldRejectDuplicateAddMember.
// CLinkshell::setMessage injects message.size() != 0 into
// ShouldSendLinkshellMessageIPC after DB update before IPC send.
// CLinkshell::PushLinkshellMessage injects !message.empty() into
// ShouldPushStoredLinkshellMessage after DB load before packet push.
// CLinkshell::RemoveMemberByName injects requesterRank and
// (newPItemLinkshell == PItemLinkshell) into ShouldBreakInventoryPearl before
// ShouldMarkPearlBroken / inventory break; then injects
// GetLSType() into ShouldMarkPearlBroken before SetLSType(BROKEN);
// then injects breakLinkshell into ShouldSendBreakMessage to select
// LinkshellNoLongerExists vs LinkshellKicked after ITEM_SAME / CharStatus.
// CLinkshell::PushPacket injects member->id == senderID,
// member->status == DISAPPEAR, and jailutils::InPrison(member) into
// ShouldReceiveLinkshellPacket before packet copy / optional LS2 rewrite / push;
// then injects member->PLinkshell2 == this into ShouldRewritePacketAsLinkshell2
// after receive filter, before chat_std / LS message rewrite and pushPacket.
// linkshell::AddOnlineMember / DelOnlineMember (linkshell.cpp) inject
// (PChar == nullptr) into ShouldRejectNullOnlineMember; on true ShowWarning +
// return OnlineMemberAlwaysReturnsFalse before process item / load / roster.
// After null gate, inject
// (PItemLinkshell != nullptr, PItemLinkshell != nullptr && isType(ITEM_LINKSHELL))
// into ShouldProcessLinkshellItem; on true proceed to load/add or del/erase.
// linkshell::AddOnlineMember injects
// LinkshellList.find(PItemLinkshell->GetLSID()) != end into
// ShouldLoadLinkshellOnOnlineAdd; on true LoadLinkshell(id); on false reuses
// cache entry when found.
// Go dual-wire: linkshell.ShouldRejectNullAddMember
// (internal/linkshell/reject_null_add_member.go),
// linkshell.ShouldRejectDuplicateAddMember
// (internal/linkshell/reject_duplicate_add_member.go),
// linkshell.ShouldSendLinkshellMessageIPC
// (internal/linkshell/send_message_ipc.go),
// linkshell.ShouldPushStoredLinkshellMessage
// (internal/linkshell/push_stored_message.go),
// linkshell.ShouldBreakInventoryPearl
// (internal/linkshell/break_inventory_pearl.go),
// linkshell.ShouldMarkPearlBroken
// (internal/linkshell/mark_pearl_broken.go),
// linkshell.ShouldSendBreakMessage
// (internal/linkshell/send_break_message.go),
// linkshell.ShouldReceiveLinkshellPacket
// (internal/linkshell/receive_packet.go),
// linkshell.ShouldRewritePacketAsLinkshell2
// (internal/linkshell/rewrite_ls2.go),
// linkshell.ShouldLoadLinkshellOnOnlineAdd
// (internal/linkshell/load_on_online_add.go),
// linkshell.ShouldRejectNullOnlineMember
// (internal/linkshell/reject_null_online_member.go),
// linkshell.ShouldProcessLinkshellItem
// (internal/linkshell/process_linkshell_item.go).

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
//
// Formula (slice 3001 dual-wire):
//   requesterRank == LSTYPE_LINKSHELL || isEquippedItem
//
// (Go: Rank(requesterRank) == RankLinkshell || isEquippedItem)
//
// requesterRank  — host-injected LSTYPE of the remove requester
// isEquippedItem — host-evaluated newPItemLinkshell == PItemLinkshell
// true  → host may break this inventory pearl (then ShouldMarkPearlBroken)
// false → skip this inventory slot for break
//
// Dual-wire of Go linkshell.ShouldBreakInventoryPearl.
// Call site: CLinkshell::RemoveMemberByName host inject
// (requesterRank, newPItemLinkshell == PItemLinkshell).
// Prior pure port: slice 1354 (capacity suite RemoveMemberByName break gate).
inline auto ShouldBreakInventoryPearl(const uint8 requesterRank, const bool isEquippedItem) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL || isEquippedItem;
}

// ShouldMarkPearlBroken mirrors GetLSType() != LSTYPE_LINKSHELL before break.
//
// Formula (slice 3008 dual-wire):
//   lsType != LSTYPE_LINKSHELL
//
// (Go: Rank(lsType) != RankLinkshell)
//
// lsType — host-injected GetLSType() of the inventory pearl under break
// true  → host may SetLSType(LSTYPE_BROKEN) + inventory UPDATE + ITEM_ATTR
// false → keep shell item type intact (do not mark broken)
//
// Dual-wire of Go linkshell.ShouldMarkPearlBroken.
// Call site: CLinkshell::RemoveMemberByName host inject
// (static_cast<uint8>(newPItemLinkshell->GetLSType())).
// Evaluated only after ShouldBreakInventoryPearl passes (slice 3001).
// Prior pure port: slice 1354 (capacity suite RemoveMemberByName break gate).
inline auto ShouldMarkPearlBroken(const uint8 lsType) -> bool
{
    return lsType != LSTYPE_LINKSHELL;
}

// ShouldSendBreakMessage mirrors breakLinkshell true → NoLongerExists else Kicked.
//
// Formula (slice 3009 dual-wire):
//   breakLinkshell
//
// breakLinkshell — host-injected RemoveMemberByName breakLinkshell flag
// true  → host may push MsgStd::LinkshellNoLongerExists
// false → host may push MsgStd::LinkshellKicked
//
// Dual-wire of Go linkshell.ShouldSendBreakMessage.
// Call site: CLinkshell::RemoveMemberByName host inject (breakLinkshell)
// after inventory break / mark-broken / ITEM_SAME / CharStatus.
// Prior pure port: slice 1354 (capacity suite RemoveMemberByName break gate).
// Sibling dual-wire: slice 3008 (ShouldMarkPearlBroken; earlier in same path).
inline auto ShouldSendBreakMessage(const bool breakLinkshell) -> bool
{
    return breakLinkshell;
}

// --- PushPacket ---

// ShouldReceiveLinkshellPacket mirrors id != sender && not disappear && not prison.
//
// Formula (slice 3017 dual-wire):
//   !isSender && !isDisappear && !inPrison
//
// isSender    — host-evaluated member->id == senderID
// isDisappear — host-evaluated member->status == STATUS_TYPE::DISAPPEAR
// inPrison    — host-evaluated jailutils::InPrison(member)
// true  → host may copy packet, optional LS2 rewrite, and push to member
// false → skip this online member for the fan-out
//
// Dual-wire of Go linkshell.ShouldReceiveLinkshellPacket.
// Call site: CLinkshell::PushPacket host inject
// (member->id == senderID, member->status == DISAPPEAR, InPrison(member)).
// Prior pure port: slice 1354 (capacity suite PushPacket receive gate).
// Sibling dual-wire: slice 3009 (ShouldSendBreakMessage; same capacity header).
// Follow-on dual-wire: slice 3026 (ShouldRewritePacketAsLinkshell2; same loop).
inline auto ShouldReceiveLinkshellPacket(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

// ShouldRewritePacketAsLinkshell2 mirrors member->PLinkshell2 == this.
//
// Formula (slice 3026 dual-wire):
//   memberIsLS2
//
// memberIsLS2 — host-evaluated member->PLinkshell2 == this
// true  → host may rewrite chat_std type / LS message flag for LS2 slot
// false → leave packet as LS1 (no rewrite)
//
// Dual-wire of Go linkshell.ShouldRewritePacketAsLinkshell2.
// Call site: CLinkshell::PushPacket host inject (member->PLinkshell2 == this)
// after ShouldReceiveLinkshellPacket, before chat_std / LS message rewrite
// and pushPacket.
// Prior pure port: slice 1354 (capacity suite PushPacket LS2 rewrite gate).
// Sibling dual-wire: slice 3017 (ShouldReceiveLinkshellPacket; earlier in same
// PushPacket member loop).
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

// ShouldRejectNullOnlineMember mirrors PChar == nullptr on AddOnlineMember /
// DelOnlineMember before process item / load / roster work.
//
// Formula (slice 3079 dual-wire):
//   charNull
//
// charNull — host-evaluated (PChar == nullptr)
// true  → host logs FormatOnlineMemberNullWarning and returns
//         OnlineMemberAlwaysReturnsFalse before process item / load / roster
// false → proceed past the null-char early gate
//
// Dual-wire of Go linkshell.ShouldRejectNullOnlineMember.
// Call sites: linkshell::AddOnlineMember / DelOnlineMember — host injects
// (PChar == nullptr); on true ShowWarning + return OnlineMemberAlwaysReturnsFalse.
// Prior pure port: slice 1355 (linkshell registry residual). Residual pins
// remain in test_linkshell_registry_1355; dedicated dual-wire suite is
// test_linkshell_reject_null_online_3079. Sibling dual-wire:
// ShouldLoadLinkshellOnOnlineAdd (3055). Residual siblings: process item,
// add-after-lookup, always-false return, erase-after-del, null warning string
// (still 1355).
inline auto ShouldRejectNullOnlineMember(const bool charNull) -> bool
{
    return charNull;
}

// ShouldProcessLinkshellItem mirrors item != null && isType(ITEM_LINKSHELL)
// on AddOnlineMember / DelOnlineMember after the null-PChar gate.
//
// Formula (slice 3099 dual-wire):
//   itemNonNull && isLinkshellType
//
// itemNonNull     — host-evaluated PItemLinkshell != nullptr
// isLinkshellType — host-evaluated item isType(ITEM_LINKSHELL)
//                   (production also short-circuits isType when the item
//                   pointer is null)
// true  → host proceeds to load / add (AddOnlineMember) or del / erase
//         (DelOnlineMember) for this linkshell item
// false → skip item work (null or non-linkshell item)
//
// Dual-wire of Go linkshell.ShouldProcessLinkshellItem.
// Call sites: linkshell::AddOnlineMember / DelOnlineMember — host injects
// (PItemLinkshell != nullptr,
//  PItemLinkshell != nullptr && PItemLinkshell->isType(ITEM_LINKSHELL)).
// Prior pure port: slice 1355 (linkshell registry residual). Residual pins
// remain in test_linkshell_registry_1355; dedicated dual-wire suite is
// test_linkshell_process_item_3099. Sibling dual-wires (leave alone):
// ShouldLoadLinkshellOnOnlineAdd (3055), ShouldRejectNullOnlineMember (3079).
// Residual siblings: add-after-lookup, always-false return, erase-after-del,
// null warning string (still 1355).
inline auto ShouldProcessLinkshellItem(const bool itemNonNull, const bool isLinkshellType) -> bool
{
    return itemNonNull && isLinkshellType;
}

// ShouldLoadLinkshellOnOnlineAdd mirrors cache miss before LoadLinkshell on
// AddOnlineMember.
//
// Formula (slice 3055 dual-wire):
//   !foundInCache
//
// foundInCache — host: LinkshellList cache hit for item LSID
// true  → host LoadLinkshell(lsid) for online add
// false → skip load (reuse cache entry)
//
// Dual-wire of Go linkshell.ShouldLoadLinkshellOnOnlineAdd.
// Call site: linkshell::AddOnlineMember — host injects
// LinkshellList.find(PItemLinkshell->GetLSID()) != LinkshellList.end(); on true
// LoadLinkshell(id); on false reuses cache entry when found.
// Prior pure port: slice 1355 (linkshell registry residual). Residual pins
// remain in test_linkshell_registry_1355; dedicated dual-wire suite is
// test_linkshell_load_online_add_3055. Residual siblings: null reject,
// process item, add-after-lookup, always-false return, erase after del
// (still 1355 residual).
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
