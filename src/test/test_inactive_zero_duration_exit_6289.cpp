#include "test_inactive_zero_duration_exit_6289.h"

#include "map/ai/states/inactive_zero_duration_exit.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "inactive zero-duration exit 6289 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runInactiveZeroDurationExit6289SelfTests() -> bool
{
    using inactivezerodurationexit::shouldExit;

    bool queriedAfterDeath = false;
    const bool deadExit = shouldExit(
        []() { return true; },
        [&]()
        {
            queriedAfterDeath = true;
            return false;
        },
        [&]()
        {
            queriedAfterDeath = true;
            return false;
        },
        [&]()
        {
            queriedAfterDeath = true;
            return false;
        });

    bool queriedAfterNoPrevent = false;
    const bool noPreventExit = shouldExit(
        []() { return false; },
        []() { return false; },
        [&]()
        {
            queriedAfterNoPrevent = true;
            return false;
        },
        [&]()
        {
            queriedAfterNoPrevent = true;
            return false;
        });

    return expect(deadExit && !queriedAfterDeath, "dead exits before status queries") &&
           expect(noPreventExit && !queriedAfterNoPrevent, "cleared prevent action skips charm queries") &&
           expect(shouldExit(false, false, false, false), "cleared prevent action exits") &&
           expect(shouldExit(false, true, true, false), "charm alone exits") &&
           expect(!shouldExit(false, true, true, true), "charm plus another prevent action holds") &&
           expect(!shouldExit(false, true, false, true), "ordinary prevent action holds");
}
