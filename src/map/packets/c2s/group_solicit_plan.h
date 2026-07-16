#pragma once
#include "map/enums/party_kind.h"

namespace groupsolicit
{

enum class Disposition
{
    NoOp,
    CannotUseInArea,
    NotPartyLeader,
    CannotInvite,
    BlockAid,
    LevelSync,
    DeliverLocal,
    DeliverRemote
};

struct Input
{
    bool inviterInPrison{}, inviterHasParty{}, inviterPartyLeader{}, inviterPartyFull{}, inviterHasAlliance{}, inviterAllianceMain{}, inviterAllianceFull{}, inviteeFound{}, inviteeBlockingAid{}, inviteeDead{}, inviteeInPrison{}, inviteePending{}, inviteeHasParty{}, inviteePartyLeader{}, inviteeInAlliance{}, inviteeLevelSync{};
};

auto Plan(PartyKind kind, Input input) -> Disposition;

} // namespace groupsolicit
