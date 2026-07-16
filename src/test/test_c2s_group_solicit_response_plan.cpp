#include "test_c2s_group_solicit_response_plan.h"
#include "map/packets/c2s/group_solicit_response_plan.h"
#include <iostream>

auto runC2SGroupSolicitResponsePlanSelfTests() -> bool
{
    using namespace groupsolicitresponse;
    bool       ok = true;
    const auto c  = [&](Action a, Action w)
    {
        if (a != w)
        {
            std::cerr << "GROUP_SOLICIT_RES plan self-test failed\n";
            ok = false;
        }
    };
    c(Plan(true, {}), Action::Remote);
    c(Plan(false, { .inviterFound = true }), Action::Declined);
    c(Plan(true, { .inviterFound = true, .inviteeHasParty = true, .inviterHasParty = true, .inviteePartyLeader = true, .inviterPartyLeader = true, .inviterAlliance = true, .inviterAllianceFull = true }), Action::AllianceFull);
    c(Plan(true, { .inviterFound = true, .inviteeHasParty = true, .inviterHasParty = true }), Action::None);
    c(Plan(true, { .inviterFound = true, .inviteeHasParty = true }), Action::None);
    c(Plan(true, { .inviterFound = true, .inviteeHasParty = true, .inviterHasParty = true, .inviteePartyLeader = true, .inviterPartyLeader = true, .inviteeTrusts = true }), Action::TrustBlocked);
    c(Plan(true, { .inviterFound = true, .inviteeHasParty = true, .inviterHasParty = true, .inviteePartyLeader = true, .inviterPartyLeader = true }), Action::CreateAlliance);
    c(Plan(true, { .inviterFound = true, .inviteeLevelSyncAndRestriction = true }), Action::LevelSyncBlocked);
    c(Plan(true, { .inviterFound = true, .inviterHasParty = true }), Action::None);
    c(Plan(true, { .inviterFound = true, .inviterHasParty = true, .inviterPartyLeader = true, .inviterPartyFull = true }), Action::PartyFull);
    c(Plan(true, { .inviterFound = true }), Action::AddPartyMember);
    return ok;
}
