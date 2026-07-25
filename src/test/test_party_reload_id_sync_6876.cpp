#include "test_party_reload_id_sync_6876.h"

#include "map/char_party_reload_id_sync.h"

#include <iostream>

auto runPartyReloadIDSync6876SelfTests() -> bool
{
    const bool ok = !partyreloadidsynchelpers::ShouldSynchronize(42, 42) &&
                    !partyreloadidsynchelpers::ShouldSynchronize(0, 0) &&
                    partyreloadidsynchelpers::ShouldSynchronize(42, 43) &&
                    partyreloadidsynchelpers::ShouldSynchronize(42, 0);
    if (!ok)
    {
        std::cerr << "party reload ID sync 6876 self-test failed\n";
    }
    return ok;
}
