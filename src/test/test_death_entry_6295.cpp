#include "test_death_entry_6295.h"

#include "map/ai/states/death_entry.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "death entry 6295 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runDeathEntry6295SelfTests() -> bool
{
    using deathentry::ApplyConstructionEffects;
    using deathentry::shouldClearPathFind;

    if (!expect(shouldClearPathFind(true), "hasPathFind clears") ||
        !expect(!shouldClearPathFind(false), "no PathFind does not clear"))
    {
        return false;
    }

    // With PathFind: del → animation → updatemask → clear, in that order.
    {
        std::vector<std::string> calls;
        ApplyConstructionEffects(
            [&]() { calls.emplace_back("del status"); },
            [&]() { calls.emplace_back("animation death"); },
            [&]() { calls.emplace_back("update hp"); },
            true,
            [&]() { calls.emplace_back("clear path"); });

        const std::vector<std::string> want{ "del status", "animation death", "update hp", "clear path" };
        if (!expect(calls == want, "ordered entry with PathFind"))
        {
            return false;
        }
    }

    // Without PathFind: same always-on steps, clear inject not invoked.
    {
        std::vector<std::string> calls;
        bool                     clearCalled = false;
        ApplyConstructionEffects(
            [&]() { calls.emplace_back("del status"); },
            [&]() { calls.emplace_back("animation death"); },
            [&]() { calls.emplace_back("update hp"); },
            false,
            [&]()
            {
                clearCalled = true;
                calls.emplace_back("clear path");
            });

        const std::vector<std::string> want{ "del status", "animation death", "update hp" };
        if (!expect(calls == want, "ordered entry without PathFind") ||
            !expect(!clearCalled, "clear inject skipped without PathFind"))
        {
            return false;
        }
    }

    return true;
}
