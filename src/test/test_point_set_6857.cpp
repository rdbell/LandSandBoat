#include "test_point_set_6857.h"

#include "map/char_points_capacity.h"

#include <iostream>

auto runPointSet6857SelfTests() -> bool
{
    bool ok = true;

    const auto invalid = charpointshelpers::PlanPointSet(false, true);
    ok = !invalid.update && !invalid.sendUnityPacket && ok;

    const auto ordinary = charpointshelpers::PlanPointSet(true, false);
    ok = ordinary.update && !ordinary.sendUnityPacket && ok;

    const auto sparks = charpointshelpers::PlanPointSet(true, true);
    ok = sparks.update && sparks.sendUnityPacket && ok;

    if (!ok)
    {
        std::cerr << "point set 6857 self-test failed\\n";
    }
    return ok;
}
