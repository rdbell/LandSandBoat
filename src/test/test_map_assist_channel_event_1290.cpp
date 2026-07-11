#include "test_map_assist_channel_event_1290.h"

#include "map/assist_channel_event.h"

#include <cstdint>
#include <iostream>

namespace
{

enum class Operation
{
    None,
    Mute,
    Unmute,
    ThumbsUp,
    ThumbsDown,
};

struct FakeAMAN
{
    void mute(const uint32 senderId)
    {
        operation = Operation::Mute;
        sender     = senderId;
        ++calls;
    }

    void unmute(const uint32 senderId)
    {
        operation = Operation::Unmute;
        sender     = senderId;
        ++calls;
    }

    void addThumbsUp(const uint32 senderId)
    {
        operation = Operation::ThumbsUp;
        sender     = senderId;
        ++calls;
    }

    void addThumbsDown(const uint32 senderId)
    {
        operation = Operation::ThumbsDown;
        sender     = senderId;
        ++calls;
    }

    Operation operation{ Operation::None };
    uint32    sender{};
    int       calls{};
};

struct FakeReceiver
{
    auto aman() -> FakeAMAN&
    {
        return state;
    }

    FakeAMAN state{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map assist channel event 1290 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testKnownActions() -> bool
{
    struct TestCase
    {
        GP_CLI_COMMAND_ASSIST_CHANNEL_KIND action;
        Operation                          operation;
    };

    constexpr TestCase cases[]{
        { GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp, Operation::ThumbsUp },
        { GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning, Operation::ThumbsDown },
        { GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList, Operation::Mute },
        { GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList, Operation::Unmute },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        FakeReceiver receiver{};
        uint32       lookedUpId{};
        int          lookups{};
        mapipc::HandleAssistChannelEvent(
            ipc::AssistChannelEvent{
                .senderId   = 0xFFFFFFFF,
                .receiverId = 0xABCDEF01,
                .action     = static_cast<uint8>(test.action),
            },
            [&](const uint32 receiverId)
            {
                ++lookups;
                lookedUpId = receiverId;
                return &receiver;
            });

        ok = expect(lookups == 1 && lookedUpId == 0xABCDEF01, "lookup uses receiver ID once") && ok;
        ok = expect(receiver.state.calls == 1 && receiver.state.operation == test.operation, "known action operation") && ok;
        ok = expect(receiver.state.sender == 0xFFFFFFFF, "operation preserves sender ID") && ok;
    }
    return ok;
}

auto testMissingReceiverAndUnknownAction() -> bool
{
    int missingLookups{};
    uint32 missingReceiverId{ 1 };
    mapipc::HandleAssistChannelEvent(
        ipc::AssistChannelEvent{ .senderId = 1, .receiverId = 0, .action = 0x24 },
        [&](const uint32 receiverId) -> FakeReceiver*
        {
            ++missingLookups;
            missingReceiverId = receiverId;
            return nullptr;
        });

    FakeReceiver receiver{};
    int          unknownLookups{};
    mapipc::HandleAssistChannelEvent(
        ipc::AssistChannelEvent{ .senderId = 2, .receiverId = 3, .action = 0xFF },
        [&](const uint32)
        {
            ++unknownLookups;
            return &receiver;
        });

    return expect(missingLookups == 1 && missingReceiverId == 0, "missing zero receiver looked up once") &&
           expect(unknownLookups == 1, "unknown action still resolves receiver") &&
           expect(receiver.state.calls == 0, "unknown action is ignored");
}

} // namespace

auto runMapAssistChannelEvent1290SelfTests() -> bool
{
    return testKnownActions() && testMissingReceiverAndUnknownAction();
}
