#include "test_spawn_tick_spawn_6271.h"

#include "map/spawn_tick_spawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn tick spawn 6271 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::Tick's non-slotted admission and Lua-retention branch.
auto runSpawnTickSpawn6271SelfTests() -> bool
{
    return expect(spawntickspawn::shouldKeepPending(false, true), "admission rejection remains pending") &&
           expect(spawntickspawn::shouldKeepPending(true, false), "Lua cancellation remains pending") &&
           expect(!spawntickspawn::shouldKeepPending(true, true), "admitted Lua-approved mob is spawned");
}
