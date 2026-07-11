#include "test_world_party_invite_1267.h"

#include "world/char_id_reroute.h"
#include "world/party_invite.h"

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
        std::cerr << "world PartyInvite 1267 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::PartyInvite& left, const ipc::PartyInvite& right) -> bool
{
    return left.inviteeId == right.inviteeId && left.inviteeTargId == right.inviteeTargId &&
           left.inviterId == right.inviterId && left.inviterTargId == right.inviterTargId &&
           left.inviterName == right.inviterName && left.inviteType == right.inviteType;
}

auto testResolvedInvitee() -> bool
{
    const ipc::PartyInvite message{
        .inviteeId     = std::numeric_limits<uint32>::max(),
        .inviteeTargId = std::numeric_limits<uint16>::max(),
        .inviterId     = std::numeric_limits<uint32>::max() - 1,
        .inviterTargId = std::numeric_limits<uint16>::max() - 1,
        .inviterName   = "Inviter",
        .inviteType    = PartyKind::Alliance,
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    uint32     lookedUpId{};
    IPP        sentEndpoint{};
    ipc::PartyInvite sentMessage{};

    worldipc::HandlePartyInvite(
        message,
        [&](const uint32 inviteeId, const ipc::PartyInvite& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharId(
                inviteeId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::PartyInvite& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved invitee routed once") &&
           expect(lookedUpId == message.inviteeId, "invitee ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives invite") &&
           expect(sameMessage(sentMessage, message), "full invite payload forwarded unchanged");
}

auto testMissingInvitee() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandlePartyInvite(
        ipc::PartyInvite{},
        [&](const uint32 inviteeId, const ipc::PartyInvite& delivered)
        {
            worldipc::RerouteMessageToCharId(
                inviteeId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::PartyInvite&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero invitee looked up once") &&
           expect(sendCalls == 0, "missing invitee sends nothing");
}

} // namespace

auto runWorldPartyInvite1267SelfTests() -> bool
{
    return testResolvedInvitee() && testMissingInvitee();
}
