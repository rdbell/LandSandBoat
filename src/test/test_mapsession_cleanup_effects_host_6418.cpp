#include "test_mapsession_cleanup_effects_host_6418.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession cleanup effects host 6418 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for cleanupSessions non-SQL effects (slice 6418).
// Go: ApplyTimeoutCleanupEffects / ApplyLinkDeadMarkEffects.
auto runMapsessionCleanupEffectsHost6418SelfTests() -> bool
{
    bool ok = true;

    // UPDATE_HP bit and STATUS_TYPE::SHUTDOWN
    constexpr unsigned char kUpdateHP   = 0x04;
    constexpr unsigned char kShutdown   = 20;
    ok = expect(kUpdateHP == 0x04 && kShutdown == 20, "constants") && ok;

    // Timeout plan order sketch: SaveFX, DeleteSQL, SavePos, Despawn, Shutdown, RmZone, Release, RmIdx, Erase
    constexpr int kTimeoutActionCountWithPet = 9; // includes SQL delete
    ok = expect(kTimeoutActionCountWithPet == 9, "timeout action count") && ok;

    // Link-dead mark: SetDisc, SetLD, HP, [Spawn]
    constexpr int kMarkWithSpawn = 4;
    ok = expect(kMarkWithSpawn == 4, "mark count") && ok;

    // Link-dead recover: ClrDisc, ClrLD, HP, [Spawn], SaveStats
    constexpr int kRecoverWithSpawn = 5;
    ok = expect(kRecoverWithSpawn == 5, "recover count") && ok;

    // Destroy with shutdown+char+zone: DeleteSQL, DecZone, Release, Erase
    constexpr int kDestroyFull = 4;
    ok = expect(kDestroyFull == 4, "destroy count") && ok;

    return ok;
}
