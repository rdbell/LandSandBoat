#include "test_unity_leader_transition_6856.h"

#include "map/char_unity_leader_capacity.h"

#include <iostream>

auto runUnityLeaderTransition6856SelfTests() -> bool
{
    bool ok = true;

    const auto invalidZero = unityleaderhelpers::PlanUnityLeaderChange(0, true);
    ok = !invalidZero.accepted && !invalidZero.removeExistingMember && !invalidZero.addNewMember && !invalidZero.persist && ok;

    const auto invalidHigh = unityleaderhelpers::PlanUnityLeaderChange(12, true);
    ok = !invalidHigh.accepted && !invalidHigh.removeExistingMember && !invalidHigh.addNewMember && !invalidHigh.persist && ok;

    const auto withoutMembership = unityleaderhelpers::PlanUnityLeaderChange(1, false);
    ok = withoutMembership.accepted && withoutMembership.newLeader == 1 && !withoutMembership.removeExistingMember &&
         withoutMembership.addNewMember && withoutMembership.persist && ok;

    const auto withMembership = unityleaderhelpers::PlanUnityLeaderChange(11, true);
    ok = withMembership.accepted && withMembership.newLeader == 11 && withMembership.removeExistingMember &&
         withMembership.addNewMember && withMembership.persist && ok;

    if (!ok)
    {
        std::cerr << "unity leader transition 6856 self-test failed\\n";
    }
    return ok;
}
