#include "test_map_chat_message_tell_1258.h"

#include "map/chat_message_tell.h"

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace
{

struct FakeCharacter
{
    uint32 id{};
};

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "map ChatMessageTell 1258 self-test failed: " << label << '\n';
    }
    return condition;
}

struct Outcome
{
    int                  lookupCalls{};
    int                  inspectCalls{};
    int                  settingCalls{};
    int                  feedbackCalls{};
    int                  deliverCalls{};
    ipc::MessageStandard feedback{};
    ipc::ChatMessageTell delivered{};
    std::vector<char>    order{};
};

auto runCase(const ipc::ChatMessageTell& message, FakeCharacter* character, const mapipc::ChatMessageTellRecipientState state,
             const bool blockHidden, const bool away) -> Outcome
{
    Outcome outcome{};
    mapipc::HandleChatMessageTell(
        message,
        [&](const std::string& recipientName) -> FakeCharacter*
        {
            outcome.order.push_back('L');
            ++outcome.lookupCalls;
            return recipientName == message.recipientName ? character : nullptr;
        },
        [&](FakeCharacter* inspected)
        {
            outcome.order.push_back('I');
            ++outcome.inspectCalls;
            return inspected == character ? state : mapipc::ChatMessageTellRecipientState{};
        },
        [&]
        {
            outcome.order.push_back('S');
            ++outcome.settingCalls;
            return blockHidden;
        },
        [&](FakeCharacter* inspected)
        {
            outcome.order.push_back('A');
            return inspected == character && away;
        },
        [&](const ipc::MessageStandard& feedback)
        {
            outcome.order.push_back('F');
            ++outcome.feedbackCalls;
            outcome.feedback = feedback;
        },
        [&](FakeCharacter* deliveredTo, const ipc::ChatMessageTell& delivered)
        {
            outcome.order.push_back('D');
            if (deliveredTo == character)
            {
                ++outcome.deliverCalls;
            }
            outcome.delivered = delivered;
        });
    return outcome;
}

auto testMissingAndInvalidRecipientsReplyOffline() -> bool
{
    bool ok = true;
    for (const auto& [name, character, state, order] : {
             std::tuple{ "missing", static_cast<FakeCharacter*>(nullptr), mapipc::ChatMessageTellRecipientState{}, std::vector<char>{ 'L', 'F' } },
             std::tuple{ "invalid", static_cast<FakeCharacter*>(nullptr), mapipc::ChatMessageTellRecipientState{}, std::vector<char>{ 'L', 'I', 'F' } },
         })
    {
        FakeCharacter present{};
        auto*         selected = std::string{name} == "invalid" ? &present : character;
        const auto message = ipc::ChatMessageTell{ .senderId = 0, .recipientName = "", .gmLevel = 0 };
        const auto outcome = runCase(message, selected, state, true, true);
        ok = expect(outcome.lookupCalls == 1 && outcome.inspectCalls == (selected ? 1 : 0) && outcome.settingCalls == 0,
                    std::string{name} + " short-circuit calls") &&
             ok;
        ok = expect(outcome.feedbackCalls == 1 && outcome.deliverCalls == 0 &&
                        outcome.feedback.recipientId == 0 && outcome.feedback.message == MsgStd::TellNotReceivedOffline &&
                        outcome.feedback.param0 == 0 && outcome.feedback.param1 == 0 && outcome.feedback.string2.empty(),
                    std::string{name} + " exact offline feedback") &&
             ok;
        ok = expect(outcome.order == order, std::string{name} + " operation order") && ok;
    }
    return ok;
}

auto testRecipientInspection() -> bool
{
    bool ok = true;
    for (const auto& [name, disappeared, inPrison, hidden, want, prisonCalls] : {
             std::tuple{ "valid", false, false, true, mapipc::ChatMessageTellRecipientState{ true, true }, 1 },
             std::tuple{ "disappeared", true, false, true, mapipc::ChatMessageTellRecipientState{}, 0 },
             std::tuple{ "prison", false, true, true, mapipc::ChatMessageTellRecipientState{}, 1 },
             std::tuple{ "both invalid", true, true, true, mapipc::ChatMessageTellRecipientState{}, 0 },
         })
    {
        int prisonCallCount = 0;
        const auto got = mapipc::InspectChatMessageTellRecipient(
            disappeared,
            [&]
            {
                ++prisonCallCount;
                return inPrison;
            },
            hidden);
        ok = expect(got.valid == want.valid && got.hidden == want.hidden && prisonCallCount == prisonCalls,
                    std::string{name} + " recipient inspection") &&
             ok;
    }
    return ok;
}

auto testValidRecipientPolicy() -> bool
{
    struct Case
    {
        const char*                              name;
        mapipc::ChatMessageTellRecipientState state;
        bool                                     blockHidden;
        uint8                                    gmLevel;
        MsgStd                                   feedback;
        bool                                     deliver;
    };
    const std::vector<Case> cases{
        { "hidden blocked before away", { true, true }, true, 0, MsgStd::TellNotReceivedOffline, false },
        { "hidden allowed then away", { true, true }, false, 0, MsgStd::TellNotReceivedAway, false },
        { "away", { true, false }, true, 0, MsgStd::TellNotReceivedAway, false },
        { "normal", { true, false }, true, 0, MsgStd::CouldNotEnter, true },
        { "GM bypass", { true, true }, true, 1, MsgStd::CouldNotEnter, true },
    };

    bool          ok = true;
    FakeCharacter character{ .id = 7 };
    for (const auto& test : cases)
    {
        const auto message = ipc::ChatMessageTell{
            .senderId = 9, .senderName = "Sender", .recipientName = "Recipient", .message = "payload", .zoneId = 10, .gmLevel = test.gmLevel,
        };
        const bool away = std::string{test.name} != "normal";
        const auto outcome = runCase(message, &character, test.state, test.blockHidden, away);
        ok = expect(outcome.lookupCalls == 1 && outcome.inspectCalls == 1 && outcome.settingCalls == 1,
                    std::string{test.name} + " lookup/inspect/setting once") &&
             ok;
        if (test.deliver)
        {
            ok = expect(outcome.feedbackCalls == 0 && outcome.deliverCalls == 1 &&
                            outcome.delivered.senderId == message.senderId && outcome.delivered.senderName == message.senderName &&
                            outcome.delivered.recipientName == message.recipientName && outcome.delivered.message == message.message &&
                            outcome.delivered.zoneId == message.zoneId && outcome.delivered.gmLevel == message.gmLevel,
                        std::string{test.name} + " unchanged delivery") &&
                 ok;
            ok = expect(outcome.order == std::vector<char>{ 'L', 'I', 'S', 'A', 'D' }, std::string{test.name} + " delivery order") && ok;
        }
        else
        {
            ok = expect(outcome.feedbackCalls == 1 && outcome.deliverCalls == 0 && outcome.feedback.recipientId == message.senderId &&
                            outcome.feedback.message == test.feedback,
                        std::string{test.name} + " feedback") &&
                 ok;
            const auto wantOrder = test.state.hidden && test.blockHidden ? std::vector<char>{ 'L', 'I', 'S', 'F' } :
                                                                          std::vector<char>{ 'L', 'I', 'S', 'A', 'F' };
            ok = expect(outcome.order == wantOrder, std::string{test.name} + " feedback order") && ok;
        }
    }
    return ok;
}

} // namespace

auto runMapChatMessageTell1258SelfTests() -> bool
{
    return testMissingAndInvalidRecipientsReplyOffline() && testRecipientInspection() && testValidRecipientPolicy();
}
