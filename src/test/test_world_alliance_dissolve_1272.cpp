#include "test_world_alliance_dissolve_1272.h"

#include "world/alliance_dissolve.h"
#include "world/alliance_members_reroute.h"

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
        std::cerr << "world AllianceDissolve 1272 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testFanout() -> bool
{
    const ipc::AllianceDissolve message{ .allianceId = std::numeric_limits<uint32>::max() };
    const std::vector<IPP>      endpoints{ IPP{}, IPP(0x14131211, 45678), IPP{} };
    int                         rerouteCalls{};
    int                         lookupCalls{};
    uint32                      lookedUpId{};
    std::vector<IPP>            sentEndpoints{};
    std::vector<ipc::AllianceDissolve> sentMessages{};

    worldipc::HandleAllianceDissolve(
        message,
        [&](const uint32 allianceId, const ipc::AllianceDissolve& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToAllianceMembers(
                allianceId,
                delivered,
                [&](const uint32 targetId)
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoints;
                },
                [&](const IPP& endpoint, const ipc::AllianceDissolve& sent)
                {
                    sentEndpoints.push_back(endpoint);
                    sentMessages.push_back(sent);
                });
        });

    bool exact = rerouteCalls == 1 && lookupCalls == 1 && lookedUpId == message.allianceId &&
                 sentEndpoints.size() == endpoints.size() && sentMessages.size() == endpoints.size();
    for (std::size_t index = 0; exact && index < endpoints.size(); ++index)
    {
        exact = sentEndpoints[index].getRawIPP() == endpoints[index].getRawIPP() &&
                sentMessages[index].allianceId == message.allianceId;
    }

    int emptyLookups{};
    int emptySends{};
    worldipc::HandleAllianceDissolve(
        ipc::AllianceDissolve{},
        [&](const uint32 allianceId, const ipc::AllianceDissolve& delivered)
        {
            worldipc::RerouteMessageToAllianceMembers(
                allianceId,
                delivered,
                [&](const uint32 targetId)
                {
                    ++emptyLookups;
                    return targetId == 0 ? std::vector<IPP>{} : std::vector<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::AllianceDissolve&) { ++emptySends; });
        });

    return expect(exact, "alliance ID selects one ordered unchanged fanout") &&
           expect(emptyLookups == 1 && emptySends == 0, "empty lookup sends nothing");
}

} // namespace

auto runWorldAllianceDissolve1272SelfTests() -> bool
{
    return testFanout();
}
