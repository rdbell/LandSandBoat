#include "map/packets/c2s/0x028_item_dump.h"
#include "map/item_container.h"

#include <iostream>

namespace
{

auto expectPlan(const itemdump::Plan actual, const itemdump::Action action, const bool breakLinkshell, const char* label) -> bool
{
    if (actual.action == action && actual.breakLinkshell == breakLinkshell)
    {
        return true;
    }
    std::cerr << "item dump 2635 self-test failed: " << label << '\n';
    return false;
}

} // namespace

auto runItemDump2635SelfTests() -> bool
{
    using itemdump::Action;
    using itemdump::PlanFor;

    bool ok = true;
    // Gil is checked before item presence and all other state.
    ok = expectPlan(PlanFor(LOC_INVENTORY, 0, 99, false, true, 0, true, true, false, true), Action::Message, false, "gil") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, false, false, 99, false, false, true, false), Action::Reject, false, "missing") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, true, true, 99, false, false, true, false), Action::Reject, false, "locked") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 2, true, false, 1, false, false, true, false), Action::Reject, false, "insufficient quantity") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, true, false, 99, true, false, true, false), Action::Message, false, "stored slip") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 0, true, false, 0, false, false, true, false), Action::Recycle, false, "zero quantity recycles") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, true, false, 99, false, false, true, false), Action::Recycle, false, "inventory recycle") && ok;
    ok = expectPlan(PlanFor(LOC_MOGSAFE, 1, 1, true, false, 99, false, false, true, false), Action::Drop, false, "non-inventory drop") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, true, false, 99, false, false, false, false), Action::Drop, false, "recycle disabled") && ok;
    ok = expectPlan(PlanFor(LOC_INVENTORY, 1, 1, true, false, 99, false, true, true, true), Action::Drop, true, "main linkshell no recycle") && ok;
    return ok;
}
