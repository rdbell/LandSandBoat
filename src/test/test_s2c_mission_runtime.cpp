#include "test_s2c_mission_runtime.h"
#include <iostream>
#include "map/packets/s2c/0x056_mission.h"
auto runS2CMissionRuntimeSelfTests() -> bool
{
    const auto p = missionhelpers::PlanFor({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x3344, 0x1122, true, false, false, true, true, false, false });
    if (p.port != 0xFFFF || p.rotz != 0 || p.addons.AMK != 0 || p.addons.ASA != 0 || p.cop2 != 0x11223344 || p.soa != 0x7E || p.rov != 0x75 || !p.tales.RoTZ || !p.tales.ASA)
    { std::cerr << "s2c MISSION runtime self-test failed\n"; return false; }
    return true;
}
