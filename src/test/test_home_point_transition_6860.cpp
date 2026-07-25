#include "test_home_point_transition_6860.h"

#include "map/char_home_point_transition.h"

#include <iostream>

auto runHomePointTransition6860SelfTests() -> bool
{
    bool ok = true;

    const auto capped = homepointtransitionhelpers::MakeHomePointPlan(true, true);
    ok = capped.rejectAtCap && capped.sendCouldNotEnterMessage && capped.clearRequestedWarp &&
         !capped.removeWeakness && !capped.removeLevelSync && !capped.clearDeathTime && !capped.restoreHPMP &&
         !capped.clearBoundary && !capped.setHomePointLocation && !capped.setDisappearStatus &&
         !capped.setAnimationNone && !capped.setUpdateHP && !capped.clearPacketList && !capped.requestSendToZone && ok;

    const auto ordinary = homepointtransitionhelpers::MakeHomePointPlan(false, false);
    ok = !ordinary.rejectAtCap && !ordinary.sendCouldNotEnterMessage && !ordinary.clearRequestedWarp &&
         !ordinary.removeWeakness && !ordinary.removeLevelSync && !ordinary.clearDeathTime && !ordinary.restoreHPMP &&
         ordinary.clearBoundary && ordinary.setHomePointLocation && ordinary.setDisappearStatus &&
         ordinary.setAnimationNone && ordinary.setUpdateHP && ordinary.clearPacketList && ordinary.requestSendToZone && ok;

    const auto reset = homepointtransitionhelpers::MakeHomePointPlan(false, true);
    ok = !reset.rejectAtCap && !reset.sendCouldNotEnterMessage && !reset.clearRequestedWarp &&
         reset.removeWeakness && reset.removeLevelSync && reset.clearDeathTime && reset.restoreHPMP &&
         reset.clearBoundary && reset.setHomePointLocation && reset.setDisappearStatus && reset.setAnimationNone &&
         reset.setUpdateHP && reset.clearPacketList && reset.requestSendToZone && ok;

    if (!ok)
    {
        std::cerr << "home point transition 6860 self-test failed\\n";
    }
    return ok;
}
