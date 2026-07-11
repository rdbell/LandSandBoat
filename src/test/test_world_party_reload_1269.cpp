#include "test_world_party_reload_1269.h"

#include "world/party_members_reroute.h"
#include "world/party_reload.h"

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
        std::cerr << "world PartyReload 1269 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testFanout() -> bool
{
    const ipc::PartyReload message{ .partyId = std::numeric_limits<uint32>::max() };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x14131211, 45678), IPP{} };
    int                    rerouteCalls{};
    int                    lookupCalls{};
    uint32                 lookedUpId{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::PartyReload> sentMessages{};

    worldipc::HandlePartyReload(
        message,
        [&](const uint32 partyId, const ipc::PartyReload& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToPartyMembers(
                partyId,
                delivered,
                [&](const uint32 targetId)
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoints;
                },
                [&](const IPP& endpoint, const ipc::PartyReload& sent)
                {
                    sentEndpoints.push_back(endpoint);
                    sentMessages.push_back(sent);
                });
        });

    bool exact = rerouteCalls == 1 && lookupCalls == 1 && lookedUpId == message.partyId &&
                 sentEndpoints.size() == endpoints.size() && sentMessages.size() == endpoints.size();
    for (std::size_t index = 0; exact && index < endpoints.size(); ++index)
    {
        exact = sentEndpoints[index].getRawIPP() == endpoints[index].getRawIPP() &&
                sentMessages[index].partyId == message.partyId;
    }

    int emptyLookups{};
    int emptySends{};
    worldipc::HandlePartyReload(
        ipc::PartyReload{},
        [&](const uint32 partyId, const ipc::PartyReload& delivered)
        {
            worldipc::RerouteMessageToPartyMembers(
                partyId,
                delivered,
                [&](const uint32 targetId)
                {
                    ++emptyLookups;
                    return targetId == 0 ? std::vector<IPP>{} : std::vector<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::PartyReload&) { ++emptySends; });
        });

    return expect(exact, "party ID selects one ordered unchanged fanout") &&
           expect(emptyLookups == 1 && emptySends == 0, "empty lookup sends nothing");
}

} // namespace

auto runWorldPartyReload1269SelfTests() -> bool
{
    return testFanout();
}
