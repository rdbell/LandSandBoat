#include "test_despawn_finalize_1404.h"

#include "map/despawn_finalize_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "despawn finalize 1404 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDespawnFinalize1404SelfTests() -> bool
{
    std::vector<int> calls;
    despawnfinalizehelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); });

    return expect(calls == std::vector<int>{ 1, 2 }, "fade out once before listener");
}
