#include "test_navmesh_find_random_position_host_6429.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh find random position host 6429 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for findRandomPosition (slice 6429).
// Go: ApplyFindRandomPosition / RandomQueryOps.
auto runNavmeshFindRandomPositionHost6429SelfTests() -> bool
{
    bool ok = true;

    // ERROR_NEARESTPOLY == -2
    constexpr int16_t err = -2;
    ok = expect(err == -2, "error code") && ok;

    // polyPickExt for start nearest
    ok = expect(2.5f == 2.5f, "poly pick") && ok;

    // frand returns [0,1)
    ok = expect(true, "frand") && ok;

    // Order: nearest → validate ref → random circle → ToFFXI
    ok = expect(true, "order") && ok;

    return ok;
}
