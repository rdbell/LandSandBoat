#include "test_c2s_lockstyle_runtime_plan.h"

#include "map/lockstyle_set_item_capacity.h"
#include "map/packets/c2s/0x053_lockstyle.h"

#include <iostream>

namespace
{

struct Plan
{
    bool set     = false;
    bool value   = false;
    bool persist = false;
    bool refresh = false;
    bool removed = false;
    bool query   = false;
};

auto plan(const GP_CLI_COMMAND_LOCKSTYLE_MODE mode, const bool locked) -> Plan
{
    switch (mode)
    {
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable:
            return locked ? Plan{ true, false, true, true } : Plan{};
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Continue:
            return { true, true };
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Query:
            return { .query = true };
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Enable:
            return { true, true, true, true, true };
        default:
            return {};
    }
}

} // namespace

auto runC2SLockstyleRuntimePlanSelfTests() -> bool
{
    bool ok = true;
    const auto check = [&ok](const bool value)
    {
        if (!value)
        {
            std::cerr << "LOCKSTYLE runtime plan self-test failed\n";
            ok = false;
        }
    };

    check(!plan(GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable, false).set);
    const auto disable = plan(GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable, true);
    check(disable.set && !disable.value && disable.persist && disable.refresh);
    check(plan(GP_CLI_COMMAND_LOCKSTYLE_MODE::Continue, false).value);
    check(plan(GP_CLI_COMMAND_LOCKSTYLE_MODE::Query, false).query);
    const auto enable = plan(GP_CLI_COMMAND_LOCKSTYLE_MODE::Enable, false);
    check(enable.set && enable.value && enable.persist && enable.refresh && enable.removed);

    const auto hidden = lockstylesetitemhelpers::PlanFor({
        .packetIndex   = 0,
        .equipKind     = 9,
        .itemFound     = true,
        .isVisibleItem = true,
        .fitsEquipKind = true,
        .isHandToHand  = true,
    });
    check(!hidden.writeStyleItem && !hidden.mainHasH2H);

    const auto accepted = lockstylesetitemhelpers::PlanFor({
        .packetIndex   = 0,
        .equipKind     = 0,
        .itemID        = 100,
        .itemFound     = true,
        .isVisibleItem = true,
        .fitsEquipKind = true,
        .isHandToHand  = true,
    });
    check(accepted.writeStyleItem && accepted.styleSlot == 0 && accepted.styleItemID == 100 && accepted.mainHasH2H);

    const auto rejectedFit = lockstylesetitemhelpers::PlanFor({
        .packetIndex   = 0,
        .equipKind     = 0,
        .itemID        = 100,
        .itemFound     = true,
        .isVisibleItem = true,
        .isHandToHand  = true,
    });
    check(rejectedFit.writeStyleItem && rejectedFit.styleItemID == 0 && rejectedFit.mainHasH2H);

    const auto missing = lockstylesetitemhelpers::PlanFor({
        .packetIndex = 1,
        .equipKind   = 1,
        .itemID      = 100,
    });
    check(missing.writeStyleItem && missing.styleSlot == 1 && missing.styleItemID == 0 && !missing.mainHasH2H);

    return ok;
}
