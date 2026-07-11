#include "test_world_party_invite_response_1268.h"

#include "world/char_id_reroute.h"
#include "world/party_invite_response.h"

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
        std::cerr << "world PartyInviteResponse 1268 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::PartyInviteResponse& left, const ipc::PartyInviteResponse& right) -> bool
{
    return left.inviteeId == right.inviteeId && left.inviteeTargId == right.inviteeTargId &&
           left.inviterId == right.inviterId && left.inviterTargId == right.inviterTargId &&
           left.inviteAnswer == right.inviteAnswer;
}

auto testResolvedInviter() -> bool
{
    const ipc::PartyInviteResponse message{
        .inviteeId     = std::numeric_limits<uint32>::max(),
        .inviteeTargId = std::numeric_limits<uint16>::max(),
        .inviterId     = std::numeric_limits<uint32>::max() - 1,
        .inviterTargId = std::numeric_limits<uint16>::max() - 1,
        .inviteAnswer  = std::numeric_limits<uint8>::max(),
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    uint32     lookedUpId{};
    IPP        sentEndpoint{};
    ipc::PartyInviteResponse sentMessage{};

    worldipc::HandlePartyInviteResponse(
        message,
        [&](const uint32 inviterId, const ipc::PartyInviteResponse& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharId(
                inviterId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::PartyInviteResponse& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved inviter routed once") &&
           expect(lookedUpId == message.inviterId, "inviter ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives response") &&
           expect(sameMessage(sentMessage, message), "full response payload forwarded unchanged");
}

auto testMissingInviter() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandlePartyInviteResponse(
        ipc::PartyInviteResponse{},
        [&](const uint32 inviterId, const ipc::PartyInviteResponse& delivered)
        {
            worldipc::RerouteMessageToCharId(
                inviterId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::PartyInviteResponse&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero inviter looked up once") &&
           expect(sendCalls == 0, "missing inviter sends nothing");
}

} // namespace

auto runWorldPartyInviteResponse1268SelfTests() -> bool
{
    return testResolvedInviter() && testMissingInviter();
}
