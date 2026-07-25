#include "test_send_to_zone_6859.h"

#include "map/char_send_to_zone_capacity.h"

#include <iostream>

auto runSendToZone6859SelfTests() -> bool
{
    bool ok = true;
    ok = sendtozonehelpers::ShouldRejectPendingZone(true) && !sendtozonehelpers::ShouldRejectPendingZone(false) && ok;
    ok = sendtozonehelpers::ShouldRejectInvalidZoneIP(false) && !sendtozonehelpers::ShouldRejectInvalidZoneIP(true) && ok;

    const auto normal = sendtozonehelpers::MakeSendToZonePlan(false, false, 100, 200, false, false);
    ok = normal.previousZoneToPersist == 200 && !normal.savePetZoningInfoBeforeTransition && normal.resetPetZoningInfo &&
         !normal.forceSynthCriticalFail && normal.requestZoneChange && normal.clearRequestedWarp && normal.clearSessionZoneIPP &&
         normal.sendZoneLogout && normal.setDisappearStatus && !normal.savePetZoningInfoAfterTransition && ok;

    const auto mogPetSynth = sendtozonehelpers::MakeSendToZonePlan(true, false, 100, 200, true, true);
    ok = mogPetSynth.previousZoneToPersist == 100 && mogPetSynth.savePetZoningInfoBeforeTransition && !mogPetSynth.resetPetZoningInfo &&
         mogPetSynth.forceSynthCriticalFail && mogPetSynth.requestZoneChange && mogPetSynth.clearRequestedWarp &&
         mogPetSynth.clearSessionZoneIPP && mogPetSynth.sendZoneLogout && mogPetSynth.setDisappearStatus &&
         mogPetSynth.savePetZoningInfoAfterTransition && ok;

    const auto sameZone = sendtozonehelpers::MakeSendToZonePlan(false, true, 100, 200, false, false);
    ok = sameZone.previousZoneToPersist == 100 && ok;

    if (!ok)
    {
        std::cerr << "send to zone 6859 self-test failed\\n";
    }
    return ok;
}
