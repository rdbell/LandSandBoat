#include "group_solicit_plan.h"

auto groupsolicit::Plan(const PartyKind kind, const Input i) -> Disposition
{
    if (i.inviterInPrison)
        return Disposition::CannotUseInArea;
    if (kind == PartyKind::Party)
    {
        if (i.inviterHasParty && !i.inviterPartyLeader)
            return Disposition::NotPartyLeader;
        if (i.inviterPartyFull)
            return Disposition::CannotInvite;
        if (!i.inviteeFound)
            return Disposition::DeliverRemote;
        if (i.inviteeDead || i.inviteeInPrison || i.inviteePending || i.inviteeHasParty)
            return Disposition::CannotInvite;
        if (i.inviteeBlockingAid)
            return Disposition::BlockAid;
        if (i.inviteeLevelSync)
            return Disposition::LevelSync;
        return Disposition::DeliverLocal;
    }
    if (kind == PartyKind::Alliance)
    {
        if (!i.inviterHasParty || !i.inviterPartyLeader || (i.inviterHasAlliance && (!i.inviterAllianceMain || i.inviterAllianceFull)))
            return Disposition::NoOp;
        if (!i.inviteeFound)
            return Disposition::DeliverRemote;
        if (i.inviteeBlockingAid)
            return Disposition::BlockAid;
        if (i.inviteeDead || i.inviteeInPrison || i.inviteePending || !i.inviteeHasParty || !i.inviteePartyLeader || i.inviteeInAlliance)
            return Disposition::CannotInvite;
        if (i.inviteeLevelSync)
            return Disposition::LevelSync;
        return Disposition::DeliverLocal;
    }
    return Disposition::NoOp;
}
