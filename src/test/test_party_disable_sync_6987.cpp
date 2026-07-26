#include "test_party_disable_sync_6987.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party disable sync 6987 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::DisableSync characterization (slice 6987). The method clears
// the sync-target pointer before refreshing the party. A mob party avoids the
// packet/DB reload body while preserving the state transition.
auto runPartyDisableSync6987SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity syncTarget;

    party.m_PartyType    = PARTY_MOBS;
    party.m_PSyncTarget  = &syncTarget;
    party.DisableSync();

    const bool clearedTarget = expect(party.GetSyncTarget() == nullptr, "sync target cleared");

    party.DisableSync();
    const bool idempotent = expect(party.GetSyncTarget() == nullptr, "second disable remains clear");

    return clearedTarget && idempotent;
}
