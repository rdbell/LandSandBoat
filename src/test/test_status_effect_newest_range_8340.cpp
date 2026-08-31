#include "test_status_effect_newest_range_8340.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect newest range 8340 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectNewestRange8340SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldSelectNewerInRange(10, 10, 20, false, false, 100, 0), "first active candidate") && ok;
    ok = expect(ShouldSelectNewerInRange(10, 10, 20, false, true, 101, 100), "strictly newer candidate") && ok;
    ok = expect(!ShouldSelectNewerInRange(10, 10, 20, false, true, 100, 100), "equal start keeps current") && ok;
    ok = expect(!ShouldSelectNewerInRange(9, 10, 20, false, false, 1, 0), "outside range") && ok;
    ok = expect(!ShouldSelectNewerInRange(10, 10, 20, true, false, 1, 0), "deleted candidate") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-newest-range-8340", runStatusEffectNewestRange8340SelfTests);
