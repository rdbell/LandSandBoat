#include "test_party_effects_changed_6984.h"

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
        std::cerr << "party effects changed 6984 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::EffectsChanged characterization (slice 6984). The method
// only marks the party packet state dirty; repeated marks remain dirty.
auto runPartyEffectsChanged6984SelfTests() -> bool
{
    CParty party(1);

    const bool initiallyClean = expect(!party.m_EffectsChanged, "constructor starts clean");
    party.EffectsChanged();
    const bool markedDirty = expect(party.m_EffectsChanged, "EffectsChanged marks dirty");
    party.EffectsChanged();
    const bool remainsDirty = expect(party.m_EffectsChanged, "repeat mark remains dirty");

    return initiallyClean && markedDirty && remainsDirty;
}
