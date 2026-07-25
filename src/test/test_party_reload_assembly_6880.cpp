#include "test_party_reload_assembly_6880.h"

#include "map/char_party_reload_assembly.h"

#include <iostream>

auto runPartyReloadAssembly6880SelfTests() -> bool
{
    using Action = partyreloadassemblyhelpers::Action;

    constexpr auto none = partyreloadassemblyhelpers::Plan{};
    constexpr auto push = partyreloadassemblyhelpers::Plan{
        .actions = { Action::PushMember },
        .count   = 1,
    };
    constexpr auto createAndPush = partyreloadassemblyhelpers::Plan{
        .actions = { Action::CreateParty, Action::PushMember },
        .count   = 2,
    };
    const bool ok = partyreloadassemblyhelpers::MakePlan(true, false) == none &&
                    partyreloadassemblyhelpers::MakePlan(false, true) == push &&
                    partyreloadassemblyhelpers::MakePlan(false, false) == createAndPush;
    if (!ok)
    {
        std::cerr << "party reload assembly 6880 self-test failed\n";
    }
    return ok;
}
