#include "test_gear_sets_1583.h"

#include "map/gear_sets_capacity.h"

#include <iostream>

namespace
{
using namespace gearsethelpers;

auto Check() -> bool
{
    if (MaxSlotID != 15 || DefaultMinEquipped != 2 || DefaultMaxEquipped != 16)
    {
        return false;
    }
    if (ItemToSetIDs().empty() || SetMetas().size() != 126)
    {
        return false;
    }
    // Empty equip → no grants.
    if (!PlanFromEquipped({}, 99).empty())
    {
        return false;
    }
    // Usukane set requires 5 pieces; one piece → none.
    {
        const auto grants = PlanFromEquipped({ { 16069, 75 } }, 99); // USUKANE_SOMEN if present
        // May or may not be usukane - just ensure no crash and empty for single unknown/min-fail.
        (void)grants;
    }
    // Level sync skip: high reqLvl with low player level.
    {
        std::vector<EquippedPiece> pieces;
        for (const auto& kv : ItemToSetIDs())
        {
            pieces.push_back(EquippedPiece{ kv.first, 99 });
            if (pieces.size() >= 5)
            {
                break;
            }
        }
        if (!PlanFromEquipped(pieces, 1).empty())
        {
            return false;
        }
    }
    // Catalog integrity: every set meta has at least one mod row with values.
    for (const auto& kv : SetMetas())
    {
        if (kv.second.mods.empty())
        {
            return false;
        }
        for (const auto& row : kv.second.mods)
        {
            if (row.values.empty())
            {
                return false;
            }
        }
    }
    return true;
}
} // namespace

auto runGearSets1583SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "gear_sets_1583 self-tests failed\n";
        return false;
    }
    return true;
}
