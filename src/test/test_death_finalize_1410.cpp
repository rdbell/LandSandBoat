#include "test_death_finalize_1410.h"

#include "map/death_finalize_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "death finalize 1410 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDeathFinalize1410SelfTests() -> bool
{
    std::vector<int> calls;
    deathfinalizehelpers::Apply(
        true,
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); });
    bool ok = expect(calls == std::vector<int>{ 1, 2, 4 }, "killer order and branch");

    calls.clear();
    deathfinalizehelpers::Apply(
        false,
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); });
    ok = expect(calls == std::vector<int>{ 3, 4 }, "missing-killer order and branch") && ok;
    return ok;
}
