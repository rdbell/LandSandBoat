#include "test_status_effect_remove_all_range_8242.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect remove all range 8242 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectRemoveAllRange8242SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldRemoveAllInRange(10, 10, 20), "first boundary included") && ok;
    ok = expect(ShouldRemoveAllInRange(20, 10, 20), "last boundary included") && ok;
    ok = expect(!ShouldRemoveAllInRange(9, 10, 20), "below range excluded") && ok;
    ok = expect(!ShouldRemoveAllInRange(21, 10, 20), "above range excluded") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-remove-all-range-8242", runStatusEffectRemoveAllRange8242SelfTests);
