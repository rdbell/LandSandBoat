#include "test_party_set_sync_target_host_7004.h"

#include <iostream>

#include "common/settings.h"
#include "map/enums/msg_std.h"

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
        std::cerr << "party set sync target host 7004 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::SetSyncTarget characterization (slice 7004). An empty
// designee clears an active target only when level sync is enabled; disabled
// settings short-circuit before the clear path.
auto runPartySetSyncTargetHost7004SelfTests() -> bool
{
    const bool originalEnabled = settings::get<bool>("map.LEVEL_SYNC_ENABLE");
    CParty     party(1);
    CBattleEntity target;

    settings::set("map.LEVEL_SYNC_ENABLE", true);
    party.m_PSyncTarget = &target;
    party.SetSyncTarget("", MsgStd::LevelSyncRemoveLeftParty);
    const bool enabledClear = expect(party.m_PSyncTarget == nullptr, "enabled empty designee clears target");

    settings::set("map.LEVEL_SYNC_ENABLE", false);
    party.m_PSyncTarget = &target;
    party.SetSyncTarget("", MsgStd::LevelSyncRemoveLeftParty);
    const bool disabledNoOp = expect(party.m_PSyncTarget == &target, "disabled settings preserve target");

    settings::set("map.LEVEL_SYNC_ENABLE", originalEnabled);
    return enabledClear && disabledNoOp;
}
