#include "test_map_group_chat_delivery_1297.h"

#include "map/group_chat_delivery.h"

#include <iostream>
#include <limits>
#include <string>

namespace
{

struct FakeParty
{
    int deliveries{};
};

struct FakeAlliance
{
    int deliveries{};
};

struct FakeLinkshell
{
    int deliveries{};
};

struct FakeUnityChat
{
    int deliveries{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map group chat delivery 1297 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testPartyChat() -> bool
{
    FakeParty                party{};
    uint32                   lookedUpId{};
    ipc::ChatMessageParty    delivered{};
    const ipc::ChatMessageParty message{
        .partyId     = std::numeric_limits<uint32>::max(),
        .senderId    = 0x01020304,
        .senderName  = "PartySender",
        .message     = "party body",
        .zoneId      = 0xABCD,
        .gmLevel     = 0xFE,
        .messageType = MESSAGE_NS_PARTY,
    };

    mapipc::HandleChatMessageParty(
        message,
        [&](const uint32 partyId)
        {
            lookedUpId = partyId;
            return &party;
        },
        [&](FakeParty* target, const ipc::ChatMessageParty& chat)
        {
            ++target->deliveries;
            delivered = chat;
            expect(target == &party, "party delivery target");
        });

    int missingDeliveries{};
    mapipc::HandleChatMessageParty(
        ipc::ChatMessageParty{},
        [](const uint32) -> FakeParty*
        {
            return nullptr;
        },
        [&](FakeParty*, const ipc::ChatMessageParty&)
        {
            ++missingDeliveries;
        });

    return expect(lookedUpId == message.partyId && party.deliveries == 1, "party lookup and delivery") &&
           expect(delivered.senderId == message.senderId && delivered.senderName == "PartySender", "party sender fields") &&
           expect(delivered.message == "party body" && delivered.zoneId == 0xABCD, "party body and zone") &&
           expect(delivered.gmLevel == 0xFE && delivered.messageType == MESSAGE_NS_PARTY, "party gm and type") &&
           expect(missingDeliveries == 0, "missing party target no-op");
}

auto testAllianceChat() -> bool
{
    FakeAlliance                alliance{};
    uint32                      lookedUpId{};
    ipc::ChatMessageAlliance    delivered{};
    const ipc::ChatMessageAlliance message{
        .allianceId  = 0x10203040,
        .senderId    = 0x05060708,
        .senderName  = "AllianceSender",
        .message     = "alliance body",
        .zoneId      = 0x1234,
        .gmLevel     = 0x11,
        .messageType = MESSAGE_PARTY,
    };

    mapipc::HandleChatMessageAlliance(
        message,
        [&](const uint32 allianceId)
        {
            lookedUpId = allianceId;
            return &alliance;
        },
        [&](FakeAlliance* target, const ipc::ChatMessageAlliance& chat)
        {
            ++target->deliveries;
            delivered = chat;
            expect(target == &alliance, "alliance delivery target");
        });

    int missingDeliveries{};
    mapipc::HandleChatMessageAlliance(
        ipc::ChatMessageAlliance{},
        [](const uint32) -> FakeAlliance*
        {
            return nullptr;
        },
        [&](FakeAlliance*, const ipc::ChatMessageAlliance&)
        {
            ++missingDeliveries;
        });

    return expect(lookedUpId == message.allianceId && alliance.deliveries == 1, "alliance lookup and delivery") &&
           expect(delivered.senderId == message.senderId && delivered.senderName == "AllianceSender", "alliance sender fields") &&
           expect(delivered.message == "alliance body" && delivered.zoneId == 0x1234, "alliance body and zone") &&
           expect(delivered.gmLevel == 0x11 && delivered.messageType == MESSAGE_PARTY, "alliance gm and type") &&
           expect(missingDeliveries == 0, "missing alliance target no-op");
}

auto testLinkshellChat() -> bool
{
    FakeLinkshell                linkshell{};
    uint32                       lookedUpId{};
    ipc::ChatMessageLinkshell    delivered{};
    CHAT_MESSAGE_TYPE            deliveredType{};
    const ipc::ChatMessageLinkshell message{
        .linkshellId = 0xA0B0C0D0,
        .senderId    = 0x11223344,
        .senderName  = "ShellSender",
        .message     = "shell body",
        .zoneId      = 0x5678,
        .gmLevel     = 0x22,
    };

    mapipc::HandleChatMessageLinkshell(
        message,
        [&](const uint32 linkshellId)
        {
            lookedUpId = linkshellId;
            return &linkshell;
        },
        [&](FakeLinkshell* target, const ipc::ChatMessageLinkshell& chat, const CHAT_MESSAGE_TYPE messageType)
        {
            ++target->deliveries;
            delivered     = chat;
            deliveredType = messageType;
            expect(target == &linkshell, "linkshell delivery target");
        });

    int missingDeliveries{};
    mapipc::HandleChatMessageLinkshell(
        ipc::ChatMessageLinkshell{},
        [](const uint32) -> FakeLinkshell*
        {
            return nullptr;
        },
        [&](FakeLinkshell*, const ipc::ChatMessageLinkshell&, const CHAT_MESSAGE_TYPE)
        {
            ++missingDeliveries;
        });

    return expect(lookedUpId == message.linkshellId && linkshell.deliveries == 1, "linkshell lookup and delivery") &&
           expect(delivered.senderId == message.senderId && delivered.senderName == "ShellSender", "linkshell sender fields") &&
           expect(delivered.message == "shell body" && delivered.zoneId == 0x5678, "linkshell body and zone") &&
           expect(delivered.gmLevel == 0x22 && deliveredType == MESSAGE_LINKSHELL, "linkshell gm and forced type") &&
           expect(missingDeliveries == 0, "missing linkshell target no-op");
}

auto testUnityChat() -> bool
{
    FakeUnityChat             unity{};
    uint32                    lookedUpId{};
    ipc::ChatMessageUnity     delivered{};
    const ipc::ChatMessageUnity message{
        .unityLeaderId = 0xFFEEDDCC,
        .senderId      = 0x55667788,
        .senderName    = "UnitySender",
        .message       = "unity body",
        .zoneId        = 0x9ABC,
        .gmLevel       = 0x33,
        .messageType   = MESSAGE_UNITY,
    };

    mapipc::HandleChatMessageUnity(
        message,
        [&](const uint32 unityLeaderId)
        {
            lookedUpId = unityLeaderId;
            return &unity;
        },
        [&](FakeUnityChat* target, const ipc::ChatMessageUnity& chat)
        {
            ++target->deliveries;
            delivered = chat;
            expect(target == &unity, "unity delivery target");
        });

    int missingDeliveries{};
    mapipc::HandleChatMessageUnity(
        ipc::ChatMessageUnity{},
        [](const uint32) -> FakeUnityChat*
        {
            return nullptr;
        },
        [&](FakeUnityChat*, const ipc::ChatMessageUnity&)
        {
            ++missingDeliveries;
        });

    return expect(lookedUpId == message.unityLeaderId && unity.deliveries == 1, "unity lookup and delivery") &&
           expect(delivered.senderId == message.senderId && delivered.senderName == "UnitySender", "unity sender fields") &&
           expect(delivered.message == "unity body" && delivered.zoneId == 0x9ABC, "unity body and zone") &&
           expect(delivered.gmLevel == 0x33 && delivered.messageType == MESSAGE_UNITY, "unity gm and type") &&
           expect(missingDeliveries == 0, "missing unity target no-op");
}

} // namespace

auto runMapGroupChatDelivery1297SelfTests() -> bool
{
    return testPartyChat() && testAllianceChat() && testLinkshellChat() && testUnityChat();
}
