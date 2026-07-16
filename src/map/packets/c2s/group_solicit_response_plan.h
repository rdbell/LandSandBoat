#pragma once

namespace groupsolicitresponse
{

enum class Action
{
    None,
    Remote,
    Declined,
    AllianceFull,
    TrustBlocked,
    AddAllianceParty,
    CreateAlliance,
    LevelSyncBlocked,
    PartyFull,
    AddPartyMember
};

struct Input
{
    bool inviterFound{}, inviteeHasParty{}, inviterHasParty{}, inviteePartyLeader{}, inviterPartyLeader{}, inviterAlliance{}, inviterAllianceMain{}, inviterAllianceFull{}, inviteeTrusts{}, inviterTrusts{}, inviteeLevelSyncAndRestriction{}, inviterPartyFull{};
};

auto Plan(bool accept, Input input) -> Action;

} // namespace groupsolicitresponse
