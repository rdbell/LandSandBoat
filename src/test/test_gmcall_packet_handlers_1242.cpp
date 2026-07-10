#include "test_gmcall_packet_handlers_1242.h"

#include "map/gmcall_packet_handlers.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "GM-call packet handlers 1242 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testIgnoredFAQKindsDoNothing() -> bool
{
    int callbackCount = 0;
    for (const auto type : { GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory, GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMNotice })
    {
        GP_CLI_COMMAND_FAQ_GMCALL packet{};
        packet.type = static_cast<uint16_t>(type);
        gmcall::handler::HandleFAQGMCall(
            packet,
            [&](const auto&) -> bool
            {
                ++callbackCount;
                return true;
            },
            [&]()
            {
                ++callbackCount;
            },
            [&]()
            {
                ++callbackCount;
            });
    }
    return expect(callbackCount == 0, "ignored FAQ kinds invoke no callbacks");
}

auto testIncompleteGMCallOnlyBuffers() -> bool
{
    GP_CLI_COMMAND_FAQ_GMCALL packet{};
    packet.type  = static_cast<uint16_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall);
    packet.pktId = 17;
    packet.seq   = 3;

    std::vector<std::string>         events;
    const GP_CLI_COMMAND_FAQ_GMCALL* addedPacket = nullptr;
    gmcall::handler::HandleFAQGMCall(
        packet,
        [&](const auto& value) -> bool
        {
            events.emplace_back("add");
            addedPacket = &value;
            return false;
        },
        [&]()
        {
            events.emplace_back("process");
        },
        [&]()
        {
            events.emplace_back("increment");
        });

    return expect(events == std::vector<std::string>{ "add" }, "incomplete GM call events") &&
           expect(addedPacket == &packet, "exact packet passed to buffer callback");
}

auto testCompleteGMCallProcessesBeforeHistoryIncrement() -> bool
{
    GP_CLI_COMMAND_FAQ_GMCALL packet{};
    packet.type = static_cast<uint16_t>(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall);
    packet.eos  = 1;

    std::vector<std::string> events;
    gmcall::handler::HandleFAQGMCall(
        packet,
        [&](const auto&) -> bool
        {
            events.emplace_back("add");
            return true;
        },
        [&]()
        {
            events.emplace_back("process");
        },
        [&]()
        {
            events.emplace_back("increment");
        });

    return expect(events == std::vector<std::string>{ "add", "process", "increment" }, "complete GM call event order");
}

auto testAcknowledgementPayloadIsIgnored() -> bool
{
    int acknowledgeCount = 0;
    for (const auto& packet : {
             GP_CLI_COMMAND_ACK_GMMSG{ .msgId = 0, .seqId = 0, .seqNum = 0 },
             GP_CLI_COMMAND_ACK_GMMSG{ .msgId = 0xFFFFFFFF, .seqId = 0xFFFF, .seqNum = 0xFFFF },
         })
    {
        gmcall::handler::HandleAckGMMsg(packet,
                                        [&]()
                                        {
                                            ++acknowledgeCount;
                                        });
    }
    return expect(acknowledgeCount == 2, "acknowledgement payload ignored");
}

} // namespace

auto runGMCallPacketHandlers1242SelfTests() -> bool
{
    bool ok = true;
    ok      = testIgnoredFAQKindsDoNothing() && ok;
    ok      = testIncompleteGMCallOnlyBuffers() && ok;
    ok      = testCompleteGMCallProcessesBeforeHistoryIncrement() && ok;
    ok      = testAcknowledgementPayloadIsIgnored() && ok;
    return ok;
}
