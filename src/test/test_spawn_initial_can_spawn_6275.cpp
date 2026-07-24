#include "test_spawn_initial_can_spawn_6275.h"

#include "map/spawn_initial_can_spawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn initial can-spawn 6275 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnInitialCanSpawn6275SelfTests() -> bool
{
    return expect(spawninitialcanspawn::canSpawn(SPAWNTYPE_NORMAL), "normal initially spawns") &&
           expect(spawninitialcanspawn::canSpawn(SPAWNTYPE_LOTTERY), "lottery initially spawns") &&
           expect(spawninitialcanspawn::canSpawn(SPAWNTYPE_SCRIPTED), "scripted initially spawns") &&
           expect(spawninitialcanspawn::canSpawn(SPAWNTYPE_WINDOWED), "windowed initially spawns") &&
           expect(!spawninitialcanspawn::canSpawn(SPAWNTYPE_ATNIGHT), "night waits for time event") &&
           expect(!spawninitialcanspawn::canSpawn(SPAWNTYPE_WEATHER), "weather waits for weather event");
}
