#include "test_party_level_sync_restore_6877.h"

#include "map/char_party_level_sync_restore.h"

#include <iostream>

auto runPartyLevelSyncRestore6877SelfTests() -> bool
{
    constexpr auto restore = partylevelsyncrestorehelpers::Plan{
        .actions = { partylevelsyncrestorehelpers::Action::SendActivation,
                     partylevelsyncrestorehelpers::Action::RemoveDispelableEffects,
                     partylevelsyncrestorehelpers::Action::AddLevelSync },
        .count   = 3,
    };
    const bool ok = partylevelsyncrestorehelpers::MakePlan(false, true, false, true, true) == partylevelsyncrestorehelpers::Plan{} &&
                    partylevelsyncrestorehelpers::MakePlan(true, false, false, true, true) == partylevelsyncrestorehelpers::Plan{} &&
                    partylevelsyncrestorehelpers::MakePlan(true, true, true, true, true) == partylevelsyncrestorehelpers::Plan{} &&
                    partylevelsyncrestorehelpers::MakePlan(true, true, false, false, true) == partylevelsyncrestorehelpers::Plan{} &&
                    partylevelsyncrestorehelpers::MakePlan(true, true, false, true, false) == partylevelsyncrestorehelpers::Plan{} &&
                    partylevelsyncrestorehelpers::MakePlan(true, true, false, true, true) == restore;
    if (!ok)
    {
        std::cerr << "party level sync restore 6877 self-test failed\n";
    }
    return ok;
}
