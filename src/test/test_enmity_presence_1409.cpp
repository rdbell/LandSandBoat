#include "test_enmity_presence_1409.h"

#include "map/enmity_presence_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity presence 1409 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runEnmityPresence1409SelfTests() -> bool
{
    int  scans = 0;
    bool ok    = expect(enmitypresencehelpers::Resolve(true, [&]() { ++scans; return false; }), "own enmity wins");
    ok         = expect(scans == 0, "own enmity skips zone scan") && ok;
    ok         = expect(enmitypresencehelpers::Resolve(false, [&]() { ++scans; return true; }), "zone target wins") && ok;
    ok         = expect(scans == 1, "missing own enmity scans once") && ok;

    ok = expect(enmitypresencehelpers::IsTargetedByLiveOpponent(true, 42, 42, 1, 2), "live opposing target") && ok;
    ok = expect(!enmitypresencehelpers::IsTargetedByLiveOpponent(false, 42, 42, 1, 2), "dead mob ignored") && ok;
    ok = expect(!enmitypresencehelpers::IsTargetedByLiveOpponent(true, 41, 42, 1, 2), "other target ignored") && ok;
    ok = expect(!enmitypresencehelpers::IsTargetedByLiveOpponent(true, 42, 42, 2, 2), "same allegiance ignored") && ok;
    return ok;
}
