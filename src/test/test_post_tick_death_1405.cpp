#include "test_post_tick_death_1405.h"

#include "map/post_tick_death_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "post tick death 1405 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runPostTickDeath1405SelfTests() -> bool
{
    bool ok = true;
    ok = expect(posttickdeathhelpers::ShouldDie(0, true, false, false), "zero hp dies") && ok;
    ok = expect(posttickdeathhelpers::ShouldDie(-1, true, false, false), "negative hp dies") && ok;
    ok = expect(!posttickdeathhelpers::ShouldDie(1, true, false, false), "positive hp survives") && ok;
    ok = expect(!posttickdeathhelpers::ShouldDie(0, false, false, false), "unspawned suppressed") && ok;
    ok = expect(!posttickdeathhelpers::ShouldDie(0, true, true, false), "death state suppressed") && ok;
    ok = expect(!posttickdeathhelpers::ShouldDie(0, true, false, true), "despawn state suppressed") && ok;
    return ok;
}
