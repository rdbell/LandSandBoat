#include "test_char_points_6855.h"

#include "map/char_points_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

auto runCharPoints6855SelfTests() -> bool
{
    bool ok = true;
    ok = charpointshelpers::ClampPointTotal(40, 15, 100) == 55 && ok;
    ok = charpointshelpers::ClampPointTotal(3, -10, 100) == 0 && ok;
    ok = charpointshelpers::ClampPointTotal(90, 20, 100) == 100 && ok;
    ok = charpointshelpers::ClampPointTotal(std::numeric_limits<int32_t>::max(), 1, std::numeric_limits<int32_t>::max()) == std::numeric_limits<int32_t>::max() && ok;
    ok = charpointshelpers::ClampPointTotal(20, 10, 0) == 0 && ok;
    ok = !charpointshelpers::ShouldAwardUnityAccolades(false, 10) && ok;
    ok = !charpointshelpers::ShouldAwardUnityAccolades(true, 0) && ok;
    ok = !charpointshelpers::ShouldAwardUnityAccolades(true, -10) && ok;
    ok = charpointshelpers::ShouldAwardUnityAccolades(true, 10) && ok;

    if (!ok)
    {
        std::cerr << "char points 6855 self-test failed\\n";
    }
    return ok;
}
