#include "test_spawn_tod_despawn_6266.h"

#include "map/spawn_tod_despawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn TOTD despawn 6266 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::onTOTDChange's production-wired trigger, spawn-type gate,
// and immediate despawn scheduling policy.
auto runSpawnTodDespawn6266SelfTests() -> bool
{
    using vanadiel_time::TOTD;

    bool ok = true;
    ok = expect(spawntoddespawn::triggerFor(TOTD::NEWDAY) == spawntoddespawn::Trigger::NewDay,
                "NEWDAY selects NewDay trigger") && ok;
    ok = expect(spawntoddespawn::triggerFor(TOTD::DAWN) == spawntoddespawn::Trigger::Dawn,
                "DAWN selects Dawn trigger") && ok;
    ok = expect(spawntoddespawn::triggerFor(TOTD::DAY) == spawntoddespawn::Trigger::None,
                "non-despawn TOTD selects no trigger") && ok;

    ok = expect(spawntoddespawn::shouldDespawn(spawntoddespawn::Trigger::NewDay, 0x01),
                "NEWDAY despawns ATNIGHT mobs") && ok;
    ok = expect(!spawntoddespawn::shouldDespawn(spawntoddespawn::Trigger::NewDay, 0x02),
                "NEWDAY keeps ATEVENING-only mobs") && ok;
    ok = expect(spawntoddespawn::shouldDespawn(spawntoddespawn::Trigger::Dawn, 0x02),
                "DAWN despawns ATEVENING mobs") && ok;
    ok = expect(!spawntoddespawn::shouldDespawn(spawntoddespawn::Trigger::Dawn, 0x01),
                "DAWN keeps ATNIGHT-only mobs") && ok;
    ok = expect(!spawntoddespawn::shouldDespawn(spawntoddespawn::Trigger::None, 0xFF),
                "other TOTD never despawns") && ok;
    ok = expect(spawntoddespawn::delay() == std::chrono::milliseconds{ 1 },
                "matching mobs schedule despawn after one millisecond") && ok;
    return ok;
}
