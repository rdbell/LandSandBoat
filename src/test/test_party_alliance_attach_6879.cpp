#include "test_party_alliance_attach_6879.h"

#include "map/char_party_alliance_attach.h"

#include <iostream>

auto runPartyAllianceAttach6879SelfTests() -> bool
{
    using Action = partyallianceattachhelpers::Action;

    constexpr auto none = partyallianceattachhelpers::Plan{};
    constexpr auto attach = partyallianceattachhelpers::Plan{
        .actions = { Action::AttachParty },
        .count   = 1,
    };
    constexpr auto createAndAttach = partyallianceattachhelpers::Plan{
        .actions = { Action::CreateAlliance, Action::AttachParty },
        .count   = 2,
    };
    const bool ok = partyallianceattachhelpers::MakePlan(false, false, false) == none &&
                    partyallianceattachhelpers::MakePlan(true, true, false) == none &&
                    partyallianceattachhelpers::MakePlan(true, false, true) == attach &&
                    partyallianceattachhelpers::MakePlan(true, false, false) == createAndAttach;
    if (!ok)
    {
        std::cerr << "party alliance attach 6879 self-test failed\n";
    }
    return ok;
}
