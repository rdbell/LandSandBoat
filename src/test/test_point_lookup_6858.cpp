#include "test_point_lookup_6858.h"

#include "map/char_points_capacity.h"

#include <iostream>

auto runPointLookup6858SelfTests() -> bool
{
    bool ok = true;

    ok = !charpointshelpers::ShouldAdvancePointResult(false, false) && ok;
    ok = !charpointshelpers::ShouldAdvancePointResult(false, true) && ok;
    ok = !charpointshelpers::ShouldAdvancePointResult(true, false) && ok;
    ok = charpointshelpers::ShouldAdvancePointResult(true, true) && ok;

    ok = charpointshelpers::ConquestPointsColumn(0) == "sandoria_cp" && ok;
    ok = charpointshelpers::ConquestPointsColumn(1) == "bastok_cp" && ok;
    ok = charpointshelpers::ConquestPointsColumn(2) == "windurst_cp" && ok;
    ok = charpointshelpers::ConquestPointsColumn(3).empty() && ok;

    if (!ok)
    {
        std::cerr << "point lookup 6858 self-test failed\\n";
    }
    return ok;
}
