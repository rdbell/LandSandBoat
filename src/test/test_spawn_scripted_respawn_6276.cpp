#include "test_spawn_scripted_respawn_6276.h"

#include "map/spawn_scripted_respawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn scripted respawn 6276 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnScriptedRespawn6276SelfTests() -> bool
{
    return expect(spawnscriptedrespawn::shouldAllow(SPAWNTYPE_SCRIPTED, true), "scripted positive delay re-enables respawn") &&
           expect(!spawnscriptedrespawn::shouldAllow(SPAWNTYPE_SCRIPTED, false), "scripted zero delay remains disabled") &&
           expect(!spawnscriptedrespawn::shouldAllow(SPAWNTYPE_NORMAL, true), "normal delay does not alter respawn");
}
