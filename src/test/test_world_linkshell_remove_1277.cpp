#include "test_world_linkshell_remove_1277.h"

#include "world/char_name_reroute.h"
#include "world/linkshell_remove.h"

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world LinkshellRemove 1277 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::LinkshellRemove& left, const ipc::LinkshellRemove& right) -> bool
{
    return left.requesterId == right.requesterId && left.requesterRank == right.requesterRank &&
           left.victimName == right.victimName && left.linkshellId == right.linkshellId;
}

auto testResolvedVictim() -> bool
{
    const ipc::LinkshellRemove message{
        .requesterId   = std::numeric_limits<uint32>::max(),
        .requesterRank = std::numeric_limits<uint8>::max(),
        .victimName    = "Victim",
        .linkshellId   = std::numeric_limits<uint32>::max() - 1,
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    std::string lookedUpName{};
    IPP         sentEndpoint{};
    ipc::LinkshellRemove sentMessage{};

    worldipc::HandleLinkshellRemove(
        message,
        [&](const std::string& victimName, const ipc::LinkshellRemove& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharName(
                victimName,
                delivered,
                [&](const std::string& targetName) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpName = targetName;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::LinkshellRemove& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved victim routed once") &&
           expect(lookedUpName == message.victimName, "victim name selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives removal") &&
           expect(sameMessage(sentMessage, message), "full removal payload forwarded unchanged");
}

auto testMissingVictim() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleLinkshellRemove(
        ipc::LinkshellRemove{},
        [&](const std::string& victimName, const ipc::LinkshellRemove& delivered)
        {
            worldipc::RerouteMessageToCharName(
                victimName,
                delivered,
                [&](const std::string& targetName) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetName.empty() ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::LinkshellRemove&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "empty victim name looked up once") &&
           expect(sendCalls == 0, "missing victim sends nothing");
}

} // namespace

auto runWorldLinkshellRemove1277SelfTests() -> bool
{
    return testResolvedVictim() && testMissingVictim();
}
