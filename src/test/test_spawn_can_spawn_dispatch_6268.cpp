#include "test_spawn_can_spawn_dispatch_6268.h"

#include "map/spawn_can_spawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn can-spawn dispatch 6268 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::canSpawnNow's live mob, TOTD, and weather field injection.
auto runSpawnCanSpawnDispatch6268SelfTests() -> bool
{
    using vanadiel_time::TOTD;

    bool ok = true;
    ok = expect(spawncanspawn::canSpawnNow(false, true, 0x00, TOTD::DAY, true, false, false, true),
                "ordinary allowed mob can spawn") && ok;
    ok = expect(!spawncanspawn::canSpawnNow(true, true, 0x00, TOTD::DAY, true, false, false, true),
                "null mob cannot spawn") && ok;
    ok = expect(!spawncanspawn::canSpawnNow(false, false, 0x00, TOTD::DAY, true, false, false, true),
                "respawn-disabled mob cannot spawn") && ok;
    ok = expect(!spawncanspawn::canSpawnNow(false, true, 0x01, TOTD::DAY, true, false, false, true),
                "ATNIGHT mob cannot spawn during day") && ok;
    ok = expect(!spawncanspawn::canSpawnNow(false, true, 0x08, TOTD::DAY, false, false, false, true),
                "fog mob cannot spawn outside fog") && ok;
    ok = expect(!spawncanspawn::canSpawnNow(false, true, 0x04, TOTD::DAY, true, true, false, false),
                "unmastered elemental cannot spawn in mismatched weather") && ok;
    return ok;
}
