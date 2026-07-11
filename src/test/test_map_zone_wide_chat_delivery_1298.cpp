#include "test_map_zone_wide_chat_delivery_1298.h"

#include "map/zone_wide_chat_delivery.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace
{

struct FakeCharacter
{
    uint32 id{};
    int    deliveries{};
};

struct Candidate
{
    bool           zoneAllows{};
    bool           eligible{};
    FakeCharacter* character{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map zone-wide chat delivery 1298 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testYellPredicatesAndFanout() -> bool
{
    FakeCharacter sender{ .id = 100 };
    FakeCharacter other{ .id = 200 };
    FakeCharacter disabledZone{ .id = 300 };
    std::vector<Candidate> candidates{
        { .zoneAllows = true, .character = &sender },
        { .zoneAllows = true, .character = &other },
        { .zoneAllows = false, .character = &disabledZone },
    };

    ipc::ChatMessageYell delivered{};
    int                  deliveries{};
    const ipc::ChatMessageYell message{
        .senderId    = 100,
        .senderName  = "YellSender",
        .message     = "yell body",
        .zoneId      = 0xABCD,
        .gmLevel     = 0xFE,
        .messageType = MESSAGE_YELL,
    };

    mapipc::HandleChatMessageYell(
        message,
        [&](auto&& visit)
        {
            for (const auto& candidate : candidates)
            {
                std::invoke(visit, candidate.zoneAllows, candidate.character->id, candidate.character);
            }
            std::invoke(visit, true, uint32{ 400 }, static_cast<FakeCharacter*>(nullptr));
        },
        [&](FakeCharacter* character, const ipc::ChatMessageYell& chat)
        {
            ++character->deliveries;
            ++deliveries;
            delivered = chat;
        });

    return expect(mapipc::ShouldDeliverYell(true, 1, 2) && !mapipc::ShouldDeliverYell(false, 1, 2), "yell predicate") &&
           expect(sender.deliveries == 0 && disabledZone.deliveries == 0, "yell excludes sender and disabled zone") &&
           expect(other.deliveries == 1 && deliveries == 1, "yell delivers once to other") &&
           expect(delivered.senderName == "YellSender" && delivered.message == "yell body", "yell body preserved") &&
           expect(delivered.zoneId == 0xABCD && delivered.gmLevel == 0xFE && delivered.messageType == MESSAGE_YELL, "yell fields preserved");
}

auto testAssistPredicatesAndFanout() -> bool
{
    FakeCharacter sender{ .id = 10 };
    FakeCharacter eligible{ .id = 20 };
    FakeCharacter ineligible{ .id = 30 };
    FakeCharacter disabledZone{ .id = 40 };
    std::vector<Candidate> candidates{
        { .zoneAllows = true, .eligible = true, .character = &sender },
        { .zoneAllows = true, .eligible = true, .character = &eligible },
        { .zoneAllows = true, .eligible = false, .character = &ineligible },
        { .zoneAllows = false, .eligible = true, .character = &disabledZone },
    };

    ipc::ChatMessageAssist delivered{};
    const ipc::ChatMessageAssist message{
        .senderId    = 10,
        .senderName  = "AssistSender",
        .message     = "assist body",
        .mentorRank  = 0x11,
        .masteryRank = 0x22,
        .gmLevel     = 0x33,
        .messageType = MESSAGE_NA_ASSIST,
    };

    mapipc::HandleChatMessageAssist(
        message,
        [&](auto&& visit)
        {
            for (const auto& candidate : candidates)
            {
                std::invoke(visit, candidate.zoneAllows, candidate.character->id, candidate.eligible, candidate.character);
            }
        },
        [&](FakeCharacter* character, const ipc::ChatMessageAssist& chat)
        {
            ++character->deliveries;
            delivered = chat;
        });

    return expect(mapipc::ShouldDeliverAssist(true, 1, 2, true) && !mapipc::ShouldDeliverAssist(true, 1, 2, false), "assist predicate") &&
           expect(sender.deliveries == 0 && ineligible.deliveries == 0 && disabledZone.deliveries == 0, "assist exclusions") &&
           expect(eligible.deliveries == 1, "assist delivers to eligible other") &&
           expect(delivered.senderName == "AssistSender" && delivered.message == "assist body", "assist body preserved") &&
           expect(delivered.mentorRank == 0x11 && delivered.masteryRank == 0x22, "assist ranks preserved") &&
           expect(delivered.gmLevel == 0x33 && delivered.messageType == MESSAGE_NA_ASSIST, "assist type preserved");
}

auto testServerMessageSkipAndPreserve() -> bool
{
    FakeCharacter sender{ .id = std::numeric_limits<uint32>::max() };
    FakeCharacter other{ .id = 1 };
    FakeCharacter zero{ .id = 0 };

    const ipc::ChatMessageServerMessage message{
        .senderId    = std::numeric_limits<uint32>::max(),
        .senderName  = "ServerSender",
        .message     = "server body",
        .zoneId      = 0x1234,
        .gmLevel     = 0x44,
        .messageType = MESSAGE_SYSTEM_1,
        .skipSender  = true,
    };

    ipc::ChatMessageServerMessage delivered{};
    mapipc::HandleChatMessageServerMessage(
        message,
        [&](auto&& visit)
        {
            std::invoke(visit, sender.id, &sender);
            std::invoke(visit, other.id, &other);
            std::invoke(visit, zero.id, &zero);
            std::invoke(visit, uint32{ 99 }, static_cast<FakeCharacter*>(nullptr));
        },
        [&](FakeCharacter* character, const ipc::ChatMessageServerMessage& chat)
        {
            ++character->deliveries;
            delivered = chat;
        });

    FakeCharacter includeSender{ .id = 7 };
    mapipc::HandleChatMessageServerMessage(
        ipc::ChatMessageServerMessage{ .senderId = 7, .skipSender = false },
        [&](auto&& visit)
        {
            std::invoke(visit, includeSender.id, &includeSender);
        },
        [&](FakeCharacter* character, const ipc::ChatMessageServerMessage&)
        {
            ++character->deliveries;
        });

    return expect(mapipc::ShouldDeliverServerMessage(1, 1, true) == false, "server skip excludes sender") &&
           expect(mapipc::ShouldDeliverServerMessage(1, 1, false) == true, "server includes sender when not skipped") &&
           expect(sender.deliveries == 0 && other.deliveries == 1 && zero.deliveries == 1, "server fanout with skip") &&
           expect(delivered.senderName == "ServerSender" && delivered.message == "server body", "server body preserved") &&
           expect(delivered.zoneId == 0x1234 && delivered.gmLevel == 0x44 && delivered.messageType == MESSAGE_SYSTEM_1, "server fields preserved") &&
           expect(includeSender.deliveries == 1, "server delivers to sender when skip disabled");
}

} // namespace

auto runMapZoneWideChatDelivery1298SelfTests() -> bool
{
    return testYellPredicatesAndFanout() && testAssistPredicatesAndFanout() && testServerMessageSkipAndPreserve();
}
