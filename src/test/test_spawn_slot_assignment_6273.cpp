#include "test_spawn_slot_assignment_6273.h"

#include "map/spawn_slot_assignment.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn slot assignment 6273 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins zone loading's nonzero spawn-slot association guard.
auto runSpawnSlotAssignment6273SelfTests() -> bool
{
    return expect(!spawnslotassignment::shouldAssign(0), "zero slot ID keeps independent spawn") &&
           expect(spawnslotassignment::shouldAssign(1), "nonzero slot ID associates shared spawn slot");
}
