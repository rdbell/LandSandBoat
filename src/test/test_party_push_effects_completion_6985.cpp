#include "test_party_push_effects_completion_6985.h"

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
        std::cerr << "party push effects completion 6985 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::PushEffectsPacket characterization (slice 6985). The clean
// gate returns without mutation; a dirty empty local roster completes and
// clears the packet-dirty state after its loop.
auto runPartyPushEffectsCompletion6985SelfTests() -> bool
{
    CParty party(1);

    party.PushEffectsPacket();
    const bool cleanRemainsClean = expect(!party.m_EffectsChanged, "clean gate remains clean");

    party.m_EffectsChanged = true;
    party.PushEffectsPacket();
    const bool dirtyCleared = expect(!party.m_EffectsChanged, "completed packet clears dirty state");

    return cleanRemainsClean && dirtyCleared;
}
