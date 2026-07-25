#include "test_zone_out_transition_6861.h"

#include "map/char_zone_out_transition.h"

#include <iostream>

namespace
{
auto HasCommonFinalization(const zoneouttransitionhelpers::ZoneOutTransitionPlan& plan) -> bool
{
    return plan.cleanTradePending && plan.cleanInvitePending && plan.clearWideScanTarget && plan.persistData &&
           plan.savePlayTime && plan.saveCharStats && plan.saveCharExp && plan.saveEminenceData &&
           plan.saveLastLogout && plan.setDisappearStatus;
}

auto Check() -> bool
{
    bool ok = true;

    const auto zoning = zoneouttransitionhelpers::MakeZoneOutTransitionPlan({
        .hasSession = true, .hasZone = true, .animation = 1, .hasTrusts = true, .status = 2,
    });
    ok = zoning.markBlowfishPendingZone && zoning.notifyNominateOnLeave && zoning.clearAttackAnimation &&
         zoning.setUpdateHP && zoning.clearTrusts && zoning.partyAction == zoneouthelpers::ZoneOutPartyAction::None &&
         !zoning.savePetZoningInfo && !zoning.resetPetZoningInfo && zoning.sessionShuttingDown == 2 &&
         zoning.charStatsZoning == 1 && zoning.decreaseZoneCounter && !zoning.saveStatusEffectsAsLogout &&
         HasCommonFinalization(zoning) && ok;

    const auto dissolve = zoneouttransitionhelpers::MakeZoneOutTransitionPlan({
        .hasSession = true, .hasZone = true, .status = 20, .hasParty = true, .hasAlliance = true,
        .isPartyLeader = true, .partyHasOnlyOneMember = true, .allianceHasOnlyOneParty = true, .petPersists = true,
    });
    ok = dissolve.markBlowfishPendingZone && dissolve.notifyNominateOnLeave && !dissolve.clearAttackAnimation &&
         !dissolve.setUpdateHP && !dissolve.clearTrusts &&
         dissolve.partyAction == zoneouthelpers::ZoneOutPartyAction::DissolveAlliance &&
         dissolve.savePetZoningInfo && !dissolve.resetPetZoningInfo && dissolve.sessionShuttingDown == 1 &&
         dissolve.charStatsZoning == 0 && dissolve.decreaseZoneCounter && dissolve.saveStatusEffectsAsLogout &&
         HasCommonFinalization(dissolve) && ok;

    const auto removeMember = zoneouttransitionhelpers::MakeZoneOutTransitionPlan({
        .hasSession = true, .status = 20, .hasParty = true,
    });
    ok = !removeMember.notifyNominateOnLeave &&
         removeMember.partyAction == zoneouthelpers::ZoneOutPartyAction::RemoveMember &&
         !removeMember.savePetZoningInfo && removeMember.resetPetZoningInfo && !removeMember.decreaseZoneCounter &&
         removeMember.saveStatusEffectsAsLogout && HasCommonFinalization(removeMember) && ok;

    const auto removeParty = zoneouttransitionhelpers::MakeZoneOutTransitionPlan({
        .hasSession = true, .status = 20, .hasParty = true, .hasAlliance = true, .isPartyLeader = true,
        .partyHasOnlyOneMember = true,
    });
    ok = removeParty.partyAction == zoneouthelpers::ZoneOutPartyAction::RemovePartyFromAlliance &&
         removeParty.resetPetZoningInfo && removeParty.saveStatusEffectsAsLogout && HasCommonFinalization(removeParty) && ok;

    return ok;
}
} // namespace

auto runZoneOutTransition6861SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "zone out transition 6861 self-test failed\\n";
    }
    return ok;
}
