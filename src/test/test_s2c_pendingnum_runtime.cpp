#include "test_s2c_pendingnum_runtime.h"
#include "map/packets/s2c/pendingnum_runtime.h"

auto runS2CPendingNumRuntimeSelfTests() -> bool
{
    const auto p = pendingnumhelpers::PlanFor({ { 1, 0xFFFFFFFF }, { 7, 2 }, { 8, 3 } });
    return p.num[0] == 0 && p.num[1] == -1 && p.num[7] == 2;
}
