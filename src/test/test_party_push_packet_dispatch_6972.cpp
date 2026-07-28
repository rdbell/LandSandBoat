#include "test_party_push_packet_dispatch_6972.h"

#include "map/entities/char_entity.h"
#include "map/packets/basic.h"
#include "map/party.h"

#include <iostream>
#include <memory>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party PushPacket dispatch 6972 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CParty::PushPacket characterization (slice 6972). The sender is
// excluded and an eligible peer receives a copy of the supplied packet.
auto runPartyPushPacketDispatch6972SelfTests() -> bool
{
    CParty      party(1);
    CCharEntity sender;
    CCharEntity peer;
    sender.id     = 6972001;
    sender.status = STATUS_TYPE::NORMAL;
    peer.id       = 6972002;
    peer.status   = STATUS_TYPE::NORMAL;
    sender.PParty = &party;
    peer.PParty   = &party;
    party.members.emplace_back(&sender);
    party.members.emplace_back(&peer);

    auto packet = std::make_unique<CBasicPacket>();
    packet->setType(0x121);
    party.PushPacket(sender.id, 0, packet);
    packet->setType(0x123);

    bool ok = expect(sender.getPacketCount() == 0, "sender is excluded") &&
              expect(peer.getPacketCount() == 1, "eligible peer receives packet");
    if (peer.getPacketCount() == 1)
    {
        ok = expect(peer.getPacketList().front()->getType() == 0x121, "peer receives packet copy isolated from source mutation") && ok;
    }
    sender.PParty = nullptr;
    peer.PParty   = nullptr;
    return ok;
}
