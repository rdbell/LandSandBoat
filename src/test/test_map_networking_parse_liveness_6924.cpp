#include "test_map_networking_parse_liveness_6924.h"

#include "map/map_networking_parse_liveness.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking parse liveness 6924 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingParseLiveness6924SelfTests() -> bool
{
    using mapnetworkingparselivenesshelpers::MakePlan;

    bool ok = true;
    ok = expect(MakePlan(false, false).tapLastUpdate, "active session traffic taps liveness") && ok;
    ok = expect(!MakePlan(true, false).tapLastUpdate, "pending-zone traffic does not tap liveness") && ok;
    ok = expect(!MakePlan(false, true).tapLastUpdate, "waiting-for-zone traffic does not tap liveness") && ok;
    ok = expect(!MakePlan(true, true).tapLastUpdate, "combined transition traffic does not tap liveness") && ok;
    return ok;
}
