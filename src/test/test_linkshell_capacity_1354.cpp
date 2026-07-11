#include "test_linkshell_capacity_1354.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell capacity 1354 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLinkshellCapacity1354SelfTests() -> bool
{
    bool ok = true;

    // AddMember
    ok = expect(linkshellhelpers::ShouldRejectNullAddMember(true), "null add") && ok;
    ok = expect(!linkshellhelpers::ShouldRejectNullAddMember(false), "null ok") && ok;
    ok = expect(linkshellhelpers::ShouldRejectDuplicateAddMember(true), "dup") && ok;
    ok = expect(linkshellhelpers::IsLinkshellSlot1(1), "ls1") && ok;
    ok = expect(!linkshellhelpers::IsLinkshellSlot1(2), "ls2") && ok;
    ok = expect(linkshellhelpers::FormatAddMemberAlreadyWarning("Alice") ==
                    "CLinkshell::AddMember attempted to add member 'Alice' who is already in the online member list.",
                "dup warn") &&
         ok;

    // setMessage IPC
    ok = expect(linkshellhelpers::ShouldSendLinkshellMessageIPC(true), "ipc non-empty") && ok;
    ok = expect(!linkshellhelpers::ShouldSendLinkshellMessageIPC(false), "ipc empty") && ok;

    // ChangeMemberRank
    ok = expect(linkshellhelpers::IsValidRankChangeNewRank(2), "rank 2") && ok;
    ok = expect(linkshellhelpers::IsValidRankChangeNewRank(3), "rank 3") && ok;
    ok = expect(!linkshellhelpers::IsValidRankChangeNewRank(1), "rank 1") && ok;
    ok = expect(!linkshellhelpers::IsValidRankChangeNewRank(4), "rank 4") && ok;
    ok = expect(linkshellhelpers::IsValidRankChangeRequester(LSTYPE_LINKSHELL), "req shell") && ok;
    ok = expect(!linkshellhelpers::IsValidRankChangeRequester(LSTYPE_PEARLSACK), "req sack") && ok;

    ok = expect(linkshellhelpers::ResolveRankChangeItemID(2) == ITEMID::PEARLSACK, "item sack") && ok;
    ok = expect(linkshellhelpers::ResolveRankChangeItemID(3) == ITEMID::LINKPEARL, "item pearl") && ok;
    ok = expect(linkshellhelpers::IsValidRankChangeItemID(ITEMID::PEARLSACK), "valid sack") && ok;
    ok = expect(linkshellhelpers::IsValidRankChangeItemID(ITEMID::LINKPEARL), "valid pearl") && ok;
    ok = expect(!linkshellhelpers::IsValidRankChangeItemID(ITEMID::LINKSHELL), "invalid shell") && ok;
    ok = expect(linkshellhelpers::ResolveLSTypeFromRankItemID(ITEMID::PEARLSACK) == LSTYPE_PEARLSACK, "type sack") && ok;
    ok = expect(linkshellhelpers::ResolveLSTypeFromRankItemID(ITEMID::LINKPEARL) == LSTYPE_LINKPEARL, "type pearl") && ok;

    ok = expect(linkshellhelpers::FormatChangeMemberRankError("Bob", 7) ==
                    "CLinkshell::ChangeMemberRank: Invalid rank change request for member 'Bob' in linkshell 7.",
                "rank err") &&
         ok;

    // RemoveMemberByName break policy
    ok = expect(linkshellhelpers::ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false), "shell break all") && ok;
    ok = expect(linkshellhelpers::ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true), "equipped only") && ok;
    ok = expect(!linkshellhelpers::ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false), "not equipped") && ok;
    ok = expect(linkshellhelpers::ShouldMarkPearlBroken(LSTYPE_PEARLSACK), "mark sack") && ok;
    ok = expect(!linkshellhelpers::ShouldMarkPearlBroken(LSTYPE_LINKSHELL), "keep shell") && ok;
    ok = expect(linkshellhelpers::ShouldSendBreakMessage(true), "break msg") && ok;
    ok = expect(!linkshellhelpers::ShouldSendBreakMessage(false), "kick msg") && ok;

    // PushPacket
    ok = expect(linkshellhelpers::ShouldReceiveLinkshellPacket(false, false, false), "recv") && ok;
    ok = expect(!linkshellhelpers::ShouldReceiveLinkshellPacket(true, false, false), "sender") && ok;
    ok = expect(!linkshellhelpers::ShouldReceiveLinkshellPacket(false, true, false), "disappear") && ok;
    ok = expect(!linkshellhelpers::ShouldReceiveLinkshellPacket(false, false, true), "prison") && ok;
    ok = expect(linkshellhelpers::ShouldRewritePacketAsLinkshell2(true), "ls2 rewrite") && ok;
    ok = expect(linkshellhelpers::ChatStdMessageTypeForLS2() == 27, "msg ls2") && ok;
    ok = expect(linkshellhelpers::ApplyLinkshellMessageLS2Flag(0x01) == 0x41, "flag or") && ok;
    ok = expect(linkshellhelpers::ShouldPushStoredLinkshellMessage(true), "push stored") && ok;
    ok = expect(linkshellhelpers::DelMemberRemaining(1), "remaining") && ok;
    ok = expect(!linkshellhelpers::DelMemberRemaining(0), "empty") && ok;

    return ok;
}
