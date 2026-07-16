#include "test_c2s_group_breakup_dispatch.h"
#include "map/packets/c2s/group_breakup_dispatch.h"
#include <iostream>

auto runC2SGroupBreakupDispatchSelfTests() -> bool
{
    bool       ok    = true;
    const auto check = [&](const auto got, const auto want)
    {
        if (got != want)
        {
            std::cerr << "GROUP_BREAKUP dispatch self-test failed\n";
            ok = false;
        }
    };
    check(groupbreakup::Select({ PartyKind::Party, true, true, false, false }), groupbreakup::Action::DisbandParty);
    check(groupbreakup::Select({ PartyKind::Party, true, true, true, false }), groupbreakup::Action::None);
    check(groupbreakup::Select({ PartyKind::Party, true, false, false, false }), groupbreakup::Action::None);
    check(groupbreakup::Select({ PartyKind::Alliance, true, true, true, true }), groupbreakup::Action::DissolveAlliance);
    check(groupbreakup::Select({ PartyKind::Alliance, true, true, true, false }), groupbreakup::Action::None);
    check(groupbreakup::Select({ static_cast<PartyKind>(2), true, true, false, false }), groupbreakup::Action::None);
    return ok;
}
