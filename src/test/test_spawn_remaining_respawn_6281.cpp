#include "test_spawn_remaining_respawn_6281.h"

#include "map/spawn_remaining_respawn.h"

#include <chrono>
#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn remaining respawn 6281 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnRemainingRespawn6281SelfTests() -> bool
{
    using namespace std::chrono_literals;

    return expect(spawnremainingrespawn::clamp(30s) == 30s, "positive delay remains") &&
           expect(spawnremainingrespawn::clamp(timer::duration::zero()) == timer::duration::zero(), "due delay clamps to zero") &&
           expect(spawnremainingrespawn::clamp(-1s) == timer::duration::zero(), "overdue delay clamps to zero");
}
