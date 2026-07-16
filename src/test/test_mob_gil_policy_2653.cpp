#include "test_mob_gil_policy_2653.h"

#include "map/entities/mob_gil_policy.h"

#include <iostream>

namespace
{
auto expect(const bool actual, const bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mob gil policy 2653 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}
} // namespace

auto runMobGilPolicy2653SelfTests() -> bool
{
    bool ok = true;
    ok      = expect(mobgilhelpers::CanDropGil(0, 0, 0), false, "all zero modifiers") && ok;
    ok      = expect(mobgilhelpers::CanDropGil(1, 0, 0), true, "positive minimum permits drop") && ok;
    ok      = expect(mobgilhelpers::CanDropGil(0, 1, 0), true, "positive maximum permits drop") && ok;
    ok      = expect(mobgilhelpers::CanDropGil(0, 0, 1), true, "positive bonus permits drop") && ok;
    ok      = expect(mobgilhelpers::CanDropGil(1, -1, 1), false, "negative maximum suppresses every path") && ok;
    ok      = expect(mobgilhelpers::CanDropGil(0, 0, -1), false, "negative bonus does not permit drop") && ok;
    return ok;
}
