#include "test_party_alliance_reconcile_6878.h"

#include "map/char_party_alliance_reconcile.h"

#include <iostream>

auto runPartyAllianceReconcile6878SelfTests() -> bool
{
    using Action = partyalliancereconcilehelpers::Action;

    const bool ok = partyalliancereconcilehelpers::MakePlan(0, false, 0).action == Action::None &&
                    partyalliancereconcilehelpers::MakePlan(0, true, 41).action == Action::DetachParty &&
                    partyalliancereconcilehelpers::MakePlan(42, false, 0).action == Action::None &&
                    partyalliancereconcilehelpers::MakePlan(42, true, 42).action == Action::None &&
                    partyalliancereconcilehelpers::MakePlan(42, true, 41).action == Action::SynchronizeID;
    if (!ok)
    {
        std::cerr << "party alliance reconcile 6878 self-test failed\n";
    }
    return ok;
}
