#include "test_spawn_tick_cleanup_6269.h"

#include "map/spawn_tick_cleanup.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn tick cleanup 6269 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::Tick's due-window and missing-mob cleanup branch for
// non-slotted pending respawns.
auto runSpawnTickCleanup6269SelfTests() -> bool
{
    return expect(spawntickcleanup::shouldProcess(false), "due registration is processed") &&
           expect(!spawntickcleanup::shouldProcess(true), "registration after window remains pending") &&
           expect(spawntickcleanup::shouldDrop(false), "missing mob registration is removed") &&
           expect(!spawntickcleanup::shouldDrop(true), "found mob registration remains for spawn checks");
}
