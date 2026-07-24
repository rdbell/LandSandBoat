#include "test_spawn_initial_respawn_6274.h"

#include "map/spawn_initial_respawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn initial respawn 6274 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins zone loading's initial m_AllowRespawn assignment.
auto runSpawnInitialRespawn6274SelfTests() -> bool
{
    return expect(spawninitialrespawn::allowsRespawn(SPAWNTYPE_NORMAL), "normal allows respawn") &&
           expect(!spawninitialrespawn::allowsRespawn(SPAWNTYPE_LOTTERY), "lottery disables respawn") &&
           expect(!spawninitialrespawn::allowsRespawn(SPAWNTYPE_SCRIPTED), "scripted disables respawn") &&
           expect(!spawninitialrespawn::allowsRespawn(SPAWNTYPE_WINDOWED), "windowed disables respawn") &&
           expect(spawninitialrespawn::allowsRespawn(SPAWNTYPE_ATNIGHT | SPAWNTYPE_LOTTERY), "combined flags remain enabled");
}
