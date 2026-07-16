#include "group_solicit_response_plan.h"

auto groupsolicitresponse::Plan(const bool accept, const Input i) -> Action
{
    if (!i.inviterFound)
        return Action::Remote;
    if (!accept)
        return Action::Declined;
    if (i.inviteeHasParty)
    {
        if (!i.inviterHasParty || !i.inviteePartyLeader || !i.inviterPartyLeader)
            return Action::None;
        if (i.inviterAlliance)
            return i.inviterAllianceFull || !i.inviterAllianceMain ? Action::AllianceFull : Action::AddAllianceParty;
        return i.inviteeTrusts || i.inviterTrusts ? Action::TrustBlocked : Action::CreateAlliance;
    }
    if (i.inviteeLevelSyncAndRestriction)
        return Action::LevelSyncBlocked;
    if (i.inviterHasParty && !i.inviterPartyLeader)
        return Action::None;
    if (i.inviterPartyFull)
        return Action::PartyFull;
    return Action::AddPartyMember;
}
