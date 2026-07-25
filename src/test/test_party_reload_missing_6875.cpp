#include "test_party_reload_missing_6875.h"

#include "map/char_party_reload_missing.h"

#include <iostream>

auto runPartyReloadMissing6875SelfTests() -> bool
{
    constexpr auto noParty = partyreloadmissinghelpers::Plan{
        .actions = { partyreloadmissinghelpers::Action::DecrementReload },
        .count   = 1,
    };
    constexpr auto hasParty = partyreloadmissinghelpers::Plan{
        .actions = { partyreloadmissinghelpers::Action::RemoveMember, partyreloadmissinghelpers::Action::DecrementReload },
        .count   = 2,
    };
    const bool ok = partyreloadmissinghelpers::MakePlan(false) == noParty &&
                    partyreloadmissinghelpers::MakePlan(true) == hasParty;
    if (!ok)
    {
        std::cerr << "party reload missing 6875 self-test failed\n";
    }
    return ok;
}
