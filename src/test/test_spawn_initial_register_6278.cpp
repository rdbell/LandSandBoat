#include "test_spawn_initial_register_6278.h"

#include "map/spawn_initial_register.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn initial register 6278 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnInitialRegister6278SelfTests() -> bool
{
    return expect(spawninitialregister::shouldUseZeroDelayOverride(true), "condition-based spawn uses zero override") &&
           expect(!spawninitialregister::shouldUseZeroDelayOverride(false), "ordinary spawn keeps default delay");
}
