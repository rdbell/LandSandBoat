#include "test_spawn_initial_dispatch_6279.h"

#include "map/spawn_initial_dispatch.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn initial dispatch 6279 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnInitialDispatch6279SelfTests() -> bool
{
    return expect(spawninitialdispatch::shouldSpawn(true, true), "eligible allowed mob spawns") &&
           expect(!spawninitialdispatch::shouldSpawn(false, true), "ineligible mob remains queued") &&
           expect(!spawninitialdispatch::shouldSpawn(true, false), "respawn-disabled mob remains queued") &&
           expect(!spawninitialdispatch::shouldSpawn(false, false), "disabled ineligible mob remains queued");
}
