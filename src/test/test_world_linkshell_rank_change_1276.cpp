#include "test_world_linkshell_rank_change_1276.h"

#include "world/char_name_reroute.h"
#include "world/linkshell_rank_change.h"

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
        std::cerr << "world LinkshellRankChange 1276 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::LinkshellRankChange& left, const ipc::LinkshellRankChange& right) -> bool
{
    return left.requesterId == right.requesterId && left.requesterRank == right.requesterRank &&
           left.memberName == right.memberName && left.linkshellId == right.linkshellId && left.newRank == right.newRank;
}

auto testResolvedMember() -> bool
{
    const ipc::LinkshellRankChange message{
        .requesterId   = std::numeric_limits<uint32>::max(),
        .requesterRank = std::numeric_limits<uint8>::max(),
        .memberName    = "Member",
        .linkshellId   = std::numeric_limits<uint32>::max() - 1,
        .newRank       = std::numeric_limits<uint8>::max() - 1,
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    std::string lookedUpName{};
    IPP         sentEndpoint{};
    ipc::LinkshellRankChange sentMessage{};

    worldipc::HandleLinkshellRankChange(
        message,
        [&](const std::string& memberName, const ipc::LinkshellRankChange& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharName(
                memberName,
                delivered,
                [&](const std::string& targetName) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpName = targetName;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::LinkshellRankChange& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved member routed once") &&
           expect(lookedUpName == message.memberName, "member name selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives change") &&
           expect(sameMessage(sentMessage, message), "full rank-change payload forwarded unchanged");
}

auto testMissingMember() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleLinkshellRankChange(
        ipc::LinkshellRankChange{},
        [&](const std::string& memberName, const ipc::LinkshellRankChange& delivered)
        {
            worldipc::RerouteMessageToCharName(
                memberName,
                delivered,
                [&](const std::string& targetName) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetName.empty() ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::LinkshellRankChange&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "empty member name looked up once") &&
           expect(sendCalls == 0, "missing member sends nothing");
}

} // namespace

auto runWorldLinkshellRankChange1276SelfTests() -> bool
{
    return testResolvedMember() && testMissingMember();
}
