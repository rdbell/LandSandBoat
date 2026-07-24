#include "test_spawn_initial_registered_6280.h"

#include "map/spawn_initial_registered.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn initial registered 6280 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnInitialRegistered6280SelfTests() -> bool
{
    return expect(spawninitialregistered::shouldSkip(true), "registered mob stays with spawn handler") &&
           expect(!spawninitialregistered::shouldSkip(false), "unregistered mob proceeds through initial loading");
}
