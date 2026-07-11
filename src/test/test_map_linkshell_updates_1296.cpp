#include "test_map_linkshell_updates_1296.h"

#include "map/linkshell_updates.h"

#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace
{

struct FakeLinkshell
{
    auto getID() const -> uint32
    {
        return id;
    }

    void ChangeMemberRank(const std::string& name, const uint8 requesterRank, const uint8 newRank)
    {
        ++rankCalls;
        memberName = name;
        requester  = requesterRank;
        rank       = newRank;
    }

    void RemoveMemberByName(const std::string& name, const uint8 requesterRank)
    {
        ++removeCalls;
        memberName = name;
        requester  = requesterRank;
    }

    uint32      id{};
    int         rankCalls{};
    int         removeCalls{};
    std::string memberName{};
    uint8       requester{};
    uint8       rank{};
};

struct FakeCharacter
{
    FakeLinkshell*       slot1{};
    FakeLinkshell*       slot2{};
    std::optional<uint8> rank1{};
    std::optional<uint8> rank2{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map linkshell updates 1296 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testRankChange() -> bool
{
    FakeLinkshell linkshell{ .id = std::numeric_limits<uint32>::max() };
    uint32        lookedUpId{};
    mapipc::HandleLinkshellRankChange(
        ipc::LinkshellRankChange{
            .requesterId   = 0x01020304,
            .requesterRank = 0xFE,
            .memberName    = "MixedCase",
            .linkshellId   = std::numeric_limits<uint32>::max(),
            .newRank       = 0xFD,
        },
        [&](const uint32 id)
        {
            lookedUpId = id;
            return &linkshell;
        });

    int missingLookups{};
    mapipc::HandleLinkshellRankChange(
        ipc::LinkshellRankChange{},
        [&](const uint32) -> FakeLinkshell*
        {
            ++missingLookups;
            return nullptr;
        });

    return expect(lookedUpId == std::numeric_limits<uint32>::max(), "rank linkshell ID") &&
           expect(linkshell.rankCalls == 1 && linkshell.memberName == "MixedCase", "rank member name") &&
           expect(linkshell.requester == 0xFE && linkshell.rank == 0xFD, "rank values passed through") &&
           expect(missingLookups == 1, "missing rank target no-op");
}

auto remove(const ipc::LinkshellRemove& message, FakeCharacter* character) -> void
{
    mapipc::HandleLinkshellRemove(
        message,
        [character](const std::string&)
        {
            return character;
        },
        [](FakeCharacter* target, const int slot)
        {
            return slot == 1 ? target->slot1 : target->slot2;
        },
        [](FakeCharacter* target, const int slot)
        {
            return slot == 1 ? target->rank1 : target->rank2;
        });
}

auto testRemoveSelectionAndAuthorization() -> bool
{
    FakeLinkshell wrong{ .id = 10 };
    FakeLinkshell target{ .id = 20 };
    FakeCharacter character{ .slot1 = &wrong, .slot2 = &target, .rank1 = LSTYPE_LINKSHELL, .rank2 = LSTYPE_LINKPEARL };

    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_PEARLSACK, .victimName = "Victim", .linkshellId = 20 }, &character);
    const bool selectedSlot2 = target.removeCalls == 1 && target.memberName == "Victim" && target.requester == LSTYPE_PEARLSACK;

    character.rank2 = LSTYPE_PEARLSACK;
    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_PEARLSACK, .victimName = "Victim", .linkshellId = 20 }, &character);
    const bool sackCannotRemoveSack = target.removeCalls == 1;

    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_LINKSHELL, .victimName = "Victim", .linkshellId = 20 }, &character);
    const bool ownerCanRemoveSack = target.removeCalls == 2;

    character.rank2.reset();
    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_LINKSHELL, .victimName = "Victim", .linkshellId = 20 }, &character);
    const bool missingItemBlocks = target.removeCalls == 2;

    return expect(selectedSlot2, "remove falls back to matching LS2") &&
           expect(sackCannotRemoveSack, "pearlsack cannot remove pearlsack") &&
           expect(ownerCanRemoveSack, "owner can remove pearlsack") &&
           expect(missingItemBlocks, "missing equipped item blocks removal");
}

auto testRemoveMissesAndLS1Precedence() -> bool
{
    FakeLinkshell first{ .id = 30 };
    FakeLinkshell second{ .id = 30 };
    FakeCharacter character{ .slot1 = &first, .slot2 = &second, .rank1 = LSTYPE_LINKPEARL, .rank2 = LSTYPE_LINKPEARL };

    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_LINKSHELL, .victimName = "First", .linkshellId = 30 }, &character);
    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_LINKPEARL, .victimName = "Denied", .linkshellId = 30 }, &character);
    remove(ipc::LinkshellRemove{ .requesterRank = LSTYPE_LINKSHELL, .victimName = "Missing", .linkshellId = 99 }, &character);
    remove(ipc::LinkshellRemove{}, nullptr);

    return expect(first.removeCalls == 1 && second.removeCalls == 0, "LS1 match takes precedence") &&
           expect(first.memberName == "First", "denied and missing removals are no-ops");
}

auto testSetMessage() -> bool
{
    FakeLinkshell                  linkshell{ .id = 0x10203040 };
    ipc::LinkshellSetMessage       delivered{};
    uint32                         lookedUpId{};
    int                            deliveries{};
    const ipc::LinkshellSetMessage message{
        .linkshellId   = 0x10203040,
        .linkshellName = "ShellName",
        .poster        = "Poster",
        .message       = "Message body",
        .postTime      = std::numeric_limits<uint32>::max(),
    };

    mapipc::HandleLinkshellSetMessage(
        message,
        [&](const uint32 id)
        {
            lookedUpId = id;
            return &linkshell;
        },
        [&](FakeLinkshell* target, const ipc::LinkshellSetMessage& update)
        {
            ++deliveries;
            delivered = update;
            expect(target == &linkshell, "message delivery target");
        });

    int missingDeliveries{};
    mapipc::HandleLinkshellSetMessage(
        ipc::LinkshellSetMessage{},
        [](const uint32) -> FakeLinkshell*
        {
            return nullptr;
        },
        [&](FakeLinkshell*, const ipc::LinkshellSetMessage&)
        {
            ++missingDeliveries;
        });

    return expect(lookedUpId == message.linkshellId && deliveries == 1, "message lookup and delivery") &&
           expect(delivered.linkshellName == "ShellName" && delivered.poster == "Poster", "message names") &&
           expect(delivered.message == "Message body" && delivered.postTime == std::numeric_limits<uint32>::max(), "message body and time") &&
           expect(missingDeliveries == 0, "missing message target no-op");
}

} // namespace

auto runMapLinkshellUpdates1296SelfTests() -> bool
{
    return testRankChange() && testRemoveSelectionAndAuthorization() && testRemoveMissesAndLS1Precedence() && testSetMessage();
}
