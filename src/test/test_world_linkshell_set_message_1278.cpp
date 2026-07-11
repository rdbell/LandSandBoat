#include "test_world_linkshell_set_message_1278.h"

#include "world/linkshell_members_reroute.h"
#include "world/linkshell_set_message.h"

#include "common/ipp.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world LinkshellSetMessage 1278 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::LinkshellSetMessage& left, const ipc::LinkshellSetMessage& right) -> bool
{
    return left.linkshellId == right.linkshellId && left.linkshellName == right.linkshellName && left.poster == right.poster &&
           left.message == right.message && left.postTime == right.postTime;
}

auto testFanout() -> bool
{
    const ipc::LinkshellSetMessage message{
        .linkshellId   = std::numeric_limits<uint32>::max(),
        .linkshellName = "Linkshell",
        .poster        = "Poster",
        .message       = "Message",
        .postTime      = std::numeric_limits<uint32>::max() - 1,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP{ 0x01020304, 5000 }, IPP{} };
    int                    rerouteCalls{};
    int                    lookupCalls{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::LinkshellSetMessage> sentMessages{};

    worldipc::HandleLinkshellSetMessage(
        message,
        [&](uint32 linkshellId, const ipc::LinkshellSetMessage& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToLinkshellMembers(
                linkshellId,
                delivered,
                [&](uint32 selectedId)
                {
                    ++lookupCalls;
                    return selectedId == message.linkshellId ? endpoints : std::vector<IPP>{};
                },
                [&](const IPP& endpoint, const ipc::LinkshellSetMessage& sent)
                {
                    sentEndpoints.push_back(endpoint);
                    sentMessages.push_back(sent);
                });
        });

    bool payloadsMatch = sentMessages.size() == endpoints.size();
    for (const auto& sent : sentMessages)
    {
        payloadsMatch = payloadsMatch && sameMessage(sent, message);
    }
    return expect(rerouteCalls == 1 && lookupCalls == 1, "linkshell selected and looked up once") &&
           expect(sentEndpoints == endpoints, "ordered duplicate endpoints preserved") &&
           expect(payloadsMatch, "full set-message payload forwarded unchanged");
}

auto testEmptyFanout() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    bool selectedZeroId{};
    worldipc::HandleLinkshellSetMessage(
        ipc::LinkshellSetMessage{},
        [&](uint32 linkshellId, const ipc::LinkshellSetMessage& delivered)
        {
            worldipc::RerouteMessageToLinkshellMembers(
                linkshellId,
                delivered,
                [&](uint32 selectedId)
                {
                    ++lookupCalls;
                    selectedZeroId = selectedId == 0;
                    return std::vector<IPP>{};
                },
                [&](const IPP&, const ipc::LinkshellSetMessage&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1 && selectedZeroId, "zero linkshell ID looked up once") &&
           expect(sendCalls == 0, "empty endpoint lookup sends nothing");
}

} // namespace

auto runWorldLinkshellSetMessage1278SelfTests() -> bool
{
    return testFanout() && testEmptyFanout();
}
