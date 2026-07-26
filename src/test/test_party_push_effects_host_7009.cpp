#include "test_party_push_effects_host_7009.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party push effects host 7009 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::PushEffectsPacket characterization (slice 7009). With no
// members, a dirty party still reaches the tail and clears m_EffectsChanged.
auto runPartyPushEffectsHost7009SelfTests() -> bool
{
    CParty party(1);
    party.m_EffectsChanged = true;

    party.PushEffectsPacket();

    return expect(!party.m_EffectsChanged, "dirty flag cleared after dispatch");
}
