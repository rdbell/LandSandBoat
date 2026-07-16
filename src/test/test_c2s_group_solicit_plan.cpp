#include "test_c2s_group_solicit_plan.h"
#include "map/packets/c2s/group_solicit_plan.h"
#include <iostream>

auto runC2SGroupSolicitPlanSelfTests() -> bool
{
    bool       ok    = true;
    const auto check = [&](auto got, auto want)
    {
        if (got != want)
        {
            std::cerr << "GROUP_SOLICIT_REQ plan self-test failed\n";
            ok = false;
        }
    };
    check(groupsolicit::Plan(PartyKind::Party, { .inviterInPrison = true }), groupsolicit::Disposition::CannotUseInArea);
    check(groupsolicit::Plan(PartyKind::Party, {}), groupsolicit::Disposition::DeliverRemote);
    check(groupsolicit::Plan(PartyKind::Party, { .inviteeFound = true, .inviteeBlockingAid = true, .inviteeDead = true }), groupsolicit::Disposition::CannotInvite);
    check(groupsolicit::Plan(PartyKind::Alliance, { .inviterHasParty = true, .inviterPartyLeader = true, .inviteeFound = true, .inviteeBlockingAid = true }), groupsolicit::Disposition::BlockAid);
    check(groupsolicit::Plan(PartyKind::Alliance, { .inviterHasParty = true, .inviterPartyLeader = true, .inviteeFound = true, .inviteeHasParty = true, .inviteePartyLeader = true }), groupsolicit::Disposition::DeliverLocal);
    check(groupsolicit::Plan(PartyKind::Alliance, { .inviterHasParty = true, .inviterPartyLeader = true, .inviterHasAlliance = true, .inviteeFound = true }), groupsolicit::Disposition::NoOp);
    check(groupsolicit::Plan(PartyKind::Alliance, { .inviterHasParty = true, .inviterPartyLeader = true, .inviterHasAlliance = true, .inviterAllianceMain = true, .inviterAllianceFull = true, .inviteeFound = true }), groupsolicit::Disposition::NoOp);
    return ok;
}
