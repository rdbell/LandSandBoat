#include "test_keyitem_spell_1515.h"

#include "map/keyitem_spell_capacity.h"
#include "map/keyitem_table_bit_capacity.h"

#include <iostream>

namespace
{
using keyitemspellhelpers::KeyItemBitIndex;
using keyitemspellhelpers::KeyItemTableInRange;
using keyitemspellhelpers::KeyItemTableIndex;
using keyitemspellhelpers::LearnedWeaponskillInRange;
using keyitemspellhelpers::ShouldAddSpell;
using keyitemspellhelpers::ShouldDelSpell;
using keyitemspellhelpers::SpellMutationNoOp;
using keyitemspellhelpers::SpellMutationSuccess;

auto Check() -> bool
{
    if (KeyItemTableIndex(0) != 0 || KeyItemTableIndex(511) != 0 || KeyItemTableIndex(512) != 1 || KeyItemTableIndex(1024) != 2)
    {
        return false;
    }
    if (KeyItemBitIndex(0) != 0 || KeyItemBitIndex(511) != 511 || KeyItemBitIndex(512) != 0 || KeyItemBitIndex(513) != 1)
    {
        return false;
    }
    if (!KeyItemTableInRange(0, 1) || !KeyItemTableInRange(0, 2) || KeyItemTableInRange(1, 1) || KeyItemTableInRange(2, 2))
    {
        return false;
    }
    const auto firstTablePlan = keyitemtablebithelpers::PlanFor({
        .keyItemID   = 511,
        .tablesSize = 1,
    });
    const auto crossTablePlan = keyitemtablebithelpers::PlanFor({
        .keyItemID   = 513,
        .tablesSize = 2,
    });
    const auto missingTablePlan = keyitemtablebithelpers::PlanFor({
        .keyItemID   = 512,
        .tablesSize = 1,
    });
    if (!firstTablePlan.inRange || firstTablePlan.tableIndex != 0 || firstTablePlan.bitIndex != 511 ||
        !crossTablePlan.inRange || crossTablePlan.tableIndex != 1 || crossTablePlan.bitIndex != 1 ||
        missingTablePlan.inRange || missingTablePlan.tableIndex != 1 || missingTablePlan.bitIndex != 0)
    {
        return false;
    }
    if (!ShouldAddSpell(true, false) || ShouldAddSpell(false, false) || ShouldAddSpell(true, true))
    {
        return false;
    }
    if (!ShouldDelSpell(true, true) || ShouldDelSpell(false, true) || ShouldDelSpell(true, false))
    {
        return false;
    }
    if (SpellMutationSuccess() != 1 || SpellMutationNoOp() != 0)
    {
        return false;
    }
    if (!LearnedWeaponskillInRange(0, 1) || !LearnedWeaponskillInRange(5, 6) || LearnedWeaponskillInRange(6, 6) || LearnedWeaponskillInRange(0, 0))
    {
        return false;
    }
    return true;
}
} // namespace

auto runKeyItemSpell1515SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "keyitem spell 1515 self-test failed\n";
    }
    return ok;
}
