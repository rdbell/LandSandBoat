#include "test_status_effect_oldest_range_8241.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect oldest range 8241 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectOldestRange8241SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldSelectOlderInRange(10, 10, 20, false, false, 100, 0), "first active candidate") && ok;
    ok = expect(ShouldSelectOlderInRange(10, 10, 20, false, true, 99, 100), "strictly older candidate") && ok;
    ok = expect(!ShouldSelectOlderInRange(10, 10, 20, false, true, 100, 100), "equal start keeps current") && ok;
    ok = expect(!ShouldSelectOlderInRange(9, 10, 20, false, false, 1, 0), "outside range") && ok;
    ok = expect(!ShouldSelectOlderInRange(10, 10, 20, true, false, 1, 0), "deleted candidate") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-oldest-range-8241", runStatusEffectOldestRange8241SelfTests);
