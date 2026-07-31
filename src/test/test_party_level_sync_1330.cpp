#include "test_party_level_sync_1330.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party level sync 1330 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyLevelSync1330SelfTests() -> bool
{
    bool ok = true;

    ok = expect(partyhelpers::LevelSyncMinLevel == 10, "min level") && ok;

    ok = expect(partyhelpers::ShouldRemoveSyncForLowLevel(0), "level 0 remove") && ok;
    ok = expect(partyhelpers::ShouldRemoveSyncForLowLevel(9), "level 9 remove") && ok;
    ok = expect(!partyhelpers::ShouldRemoveSyncForLowLevel(10), "level 10 keep") && ok;
    ok = expect(!partyhelpers::ShouldRemoveSyncForLowLevel(99), "level 99 keep") && ok;

    ok = expect(partyhelpers::ResolveSyncMemberLevel(30, 50) == 30, "sync lower") && ok;
    ok = expect(partyhelpers::ResolveSyncMemberLevel(50, 30) == 30, "member lower") && ok;
    ok = expect(partyhelpers::ResolveSyncMemberLevel(40, 40) == 40, "equal") && ok;
    ok = expect(partyhelpers::ResolveSyncMemberLevel(0, 75) == 0, "zero sync") && ok;

    ok = expect(partyhelpers::ShouldApplySyncToMember(true, true), "pc same zone") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncToMember(true, false), "pc other zone") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncToMember(false, true), "mob same zone") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncToMember(false, false), "mob other zone") && ok;

    auto plan = partyhelpers::PlanRefreshSyncMember(true, true, 30, 50, 50);
    ok = expect(plan.apply && plan.newMainLevel == 30 && plan.rebuild, "eligible rebuild") && ok;
    plan = partyhelpers::PlanRefreshSyncMember(true, true, 50, 30, 30);
    ok = expect(plan.apply && plan.newMainLevel == 30 && !plan.rebuild, "eligible no rebuild") && ok;
    plan = partyhelpers::PlanRefreshSyncMember(true, false, 30, 50, 50);
    ok = expect(!plan.apply && !plan.rebuild, "other-zone skip") && ok;
    plan = partyhelpers::PlanRefreshSyncMember(false, true, 30, 50, 50);
    ok = expect(!plan.apply && !plan.rebuild, "non-PC skip") && ok;

    return ok;
}
