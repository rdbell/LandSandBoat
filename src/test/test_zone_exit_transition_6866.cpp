#include "test_zone_exit_transition_6866.h"
#include "map/char_zone_exit_transition.h"
#include <iostream>
auto runZoneExitTransition6866SelfTests() -> bool
{
    bool ok = true;
    const auto disconnect = zoneexithelpers::MakeZoneExitPlan(zoneexithelpers::ZoneExitKind::Disconnect, false);
    ok = disconnect.savePosition && disconnect.clearPackets && disconnect.destination == zoneexithelpers::Destination::Invalid && disconnect.setShutdownStatus && !disconnect.setDisappearStatus && !disconnect.clearBoundary && disconnect.requestZoneChange && !disconnect.savePetZoningInfo && disconnect.sendLogoutPacket && ok;
    const auto disconnectPet = zoneexithelpers::MakeZoneExitPlan(zoneexithelpers::ZoneExitKind::Disconnect, true);
    ok = disconnectPet.savePetZoningInfo && ok;
    const auto rezone = zoneexithelpers::MakeZoneExitPlan(zoneexithelpers::ZoneExitKind::Rezone, false);
    ok = !rezone.savePosition && rezone.clearPackets && rezone.destination == zoneexithelpers::Destination::Current && !rezone.setShutdownStatus && rezone.setDisappearStatus && rezone.clearBoundary && rezone.requestZoneChange && !rezone.savePetZoningInfo && !rezone.sendLogoutPacket && ok;
    const auto rezonePet = zoneexithelpers::MakeZoneExitPlan(zoneexithelpers::ZoneExitKind::Rezone, true);
    ok = rezonePet.savePetZoningInfo && ok;
    if (!ok) std::cerr << "zone exit transition 6866 self-test failed\n";
    return ok;
}
