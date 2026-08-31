#include "test_status_effect_remove_by_icon_8243.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect remove by icon 8243 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectRemoveByIcon8243SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(CanClientCancelIcon(5, 5, false), "matching icon without NoCancel") && ok;
    ok = expect(!CanClientCancelIcon(5, 5, true), "matching icon with NoCancel") && ok;
    ok = expect(!CanClientCancelIcon(5, 6, false), "different icon") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-remove-by-icon-8243", runStatusEffectRemoveByIcon8243SelfTests);
