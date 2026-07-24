#include "test_spawn_tick_slot_6270.h"

#include "map/spawn_tick_slot.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn tick slot 6270 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::Tick's slotted-respawn window and successful-attempt
// cleanup decisions.
auto runSpawnTickSlot6270SelfTests() -> bool
{
    return expect(spawntickslot::shouldProcess(false), "due slot registration is processed") &&
           expect(!spawntickslot::shouldProcess(true), "slot registration after window remains pending") &&
           expect(spawntickslot::shouldRemoveAfterAttempt(true), "successful slot spawn removes registration") &&
           expect(!spawntickslot::shouldRemoveAfterAttempt(false), "failed slot spawn remains pending");
}
