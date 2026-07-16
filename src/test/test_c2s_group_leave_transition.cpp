#include "test_c2s_group_leave_transition.h"
#include "map/enums/party_kind.h"
#include "map/packets/c2s/0x06f_group_leave.h"
#include <iostream>

auto runC2SGroupLeaveTransitionSelfTests() -> bool
{
    using namespace groupleavehelpers;
    const auto direct    = MakeMutationPlan(PartyKind::Party, false, true, true, false);
    const auto remove    = MakeMutationPlan(PartyKind::Party, true, true, false, false);
    const auto dissolve  = MakeMutationPlan(PartyKind::Party, true, true, true, false);
    const auto leader    = MakeMutationPlan(PartyKind::Alliance, true, false, false, true);
    const auto nonleader = MakeMutationPlan(PartyKind::Alliance, true, false, false, false);
    const bool ok        = direct.removeMember && direct.allianceAction == AllianceAction::None &&
                           remove.removeMember && remove.allianceAction == AllianceAction::RemoveParty &&
                           dissolve.removeMember && dissolve.allianceAction == AllianceAction::Dissolve &&
                           !leader.removeMember && leader.allianceAction == AllianceAction::RemoveParty &&
                           !nonleader.removeMember && nonleader.allianceAction == AllianceAction::None;
    if (!ok)
        std::cerr << "c2s GROUP_LEAVE transition self-test failed\n";
    return ok;
}
