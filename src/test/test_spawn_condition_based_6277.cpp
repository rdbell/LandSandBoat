#include "test_spawn_condition_based_6277.h"

#include "map/spawn_condition_based.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn condition-based 6277 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSpawnConditionBased6277SelfTests() -> bool
{
    return expect(!spawnconditionbased::requiresConditionEvent(SPAWNTYPE_NORMAL), "normal is not condition based") &&
           expect(spawnconditionbased::requiresConditionEvent(SPAWNTYPE_ATNIGHT), "night is condition based") &&
           expect(spawnconditionbased::requiresConditionEvent(SPAWNTYPE_WEATHER), "weather is condition based") &&
           expect(spawnconditionbased::requiresConditionEvent(SPAWNTYPE_ATNIGHT | SPAWNTYPE_FOG), "combined conditions remain based") &&
           expect(!spawnconditionbased::requiresConditionEvent(SPAWNTYPE_LOTTERY), "lottery is not condition based");
}
