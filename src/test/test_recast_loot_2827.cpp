#include "test_recast_loot_2827.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast loot 2827 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runRecastLoot2827SelfTests() -> bool
{
    using recasthelpers::RecastIDFromLootRecast;

    bool ok = true;
    ok = expect(RecastIDFromLootRecast(0) == 0, "zero identity") && ok;
    ok = expect(RecastIDFromLootRecast(1) == 1, "one identity") && ok;
    ok = expect(RecastIDFromLootRecast(0xFFFF) == 0xFFFF, "max identity") && ok;
    return ok;
}
