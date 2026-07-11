#include "test_party_push_packet_1335.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party push packet 1335 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyPushPacket1335SelfTests() -> bool
{
    bool ok = true;

    // Happy path: PC, not sender, visible, free, any zone (filter 0).
    ok = expect(partyhelpers::ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230), "all zones") && ok;
    // Zone filter match.
    ok = expect(partyhelpers::ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 230), "zone match") && ok;
    // Zone filter miss.
    ok = expect(!partyhelpers::ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 231), "zone miss") && ok;
    // Sender skipped.
    ok = expect(!partyhelpers::ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230), "sender skip") && ok;
    // Non-PC skipped.
    ok = expect(!partyhelpers::ShouldPushPartyPacketToMember(false, 2, 1, true, false, 0, 230), "non-pc") && ok;
    // Disappear skipped.
    ok = expect(!partyhelpers::ShouldPushPartyPacketToMember(true, 2, 1, false, false, 0, 230), "disappear") && ok;
    // Prison skipped.
    ok = expect(!partyhelpers::ShouldPushPartyPacketToMember(true, 2, 1, true, true, 0, 230), "prison") && ok;

    return ok;
}
