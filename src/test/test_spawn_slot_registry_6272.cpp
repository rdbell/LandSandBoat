#include "test_spawn_slot_registry_6272.h"

#include "map/spawn_slot_registry.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn slot registry 6272 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::getOrCreateSpawnSlot's lazy keyed ownership decision.
auto runSpawnSlotRegistry6272SelfTests() -> bool
{
    return expect(spawnslotregistry::shouldCreateSlot(false), "missing slot is created") &&
           expect(!spawnslotregistry::shouldCreateSlot(true), "existing slot is retained");
}
