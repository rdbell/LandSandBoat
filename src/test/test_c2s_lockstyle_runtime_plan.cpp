#include "test_c2s_lockstyle_runtime_plan.h"

#include "map/lockstyle_set_conflict_capacity.h"
#include "map/lockstyle_set_item_capacity.h"
#include "map/lockstyle_set_style_update_capacity.h"
#include "map/lockstyle_set_tail_capacity.h"
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

    const auto conflicts = lockstylesetconflicthelpers::PlanFor(
        std::array<std::uint16_t, lockstylesetconflicthelpers::StyleSlotCount>{
            100, 200, 700, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 300,
        },
        std::array<lockstylesetconflicthelpers::Item, lockstylesetconflicthelpers::StyleSlotCount>{
            lockstylesetconflicthelpers::Item{ .itemID = 100, .found = true, .removeSlots = std::uint16_t{ 1 } << 1 },
            lockstylesetconflicthelpers::Item{ .itemID = 200, .found = true, .removeSlots = std::uint16_t{ 1 } << 2 },
        });
    check(conflicts.styleItems[1] == 0 && conflicts.styleItems[2] == 700 && conflicts.styleItems[15] == 300);

    const auto zeroItemConflict = lockstylesetconflicthelpers::PlanFor(
        std::array<std::uint16_t, lockstylesetconflicthelpers::StyleSlotCount>{
            100, 200, 700, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 300,
        },
        std::array<lockstylesetconflicthelpers::Item, lockstylesetconflicthelpers::StyleSlotCount>{
            lockstylesetconflicthelpers::Item{ .itemID = 100, .found = true, .removeSlots = std::uint16_t{ 1 } << 1 },
            lockstylesetconflicthelpers::Item{ .itemID = 200, .found = true, .removeSlots = std::uint16_t{ 1 } << 2 },
            lockstylesetconflicthelpers::Item{ .itemID = 0, .found = true, .removeSlots = std::uint16_t{ 1 } << 2 },
        });
    check(zeroItemConflict.styleItems[2] == 0);

    const auto clearsUnscanned = lockstylesetconflicthelpers::PlanFor(
        std::array<std::uint16_t, lockstylesetconflicthelpers::StyleSlotCount>{
            0, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 500,
        },
        std::array<lockstylesetconflicthelpers::Item, lockstylesetconflicthelpers::StyleSlotCount>{
            lockstylesetconflicthelpers::Item{}, lockstylesetconflicthelpers::Item{}, lockstylesetconflicthelpers::Item{}, lockstylesetconflicthelpers::Item{},
            lockstylesetconflicthelpers::Item{ .itemID = 400, .found = true, .removeSlots = std::uint16_t{ 1 } << 15 },
        });
    check(clearsUnscanned.styleItems[15] == 0);

    const auto styleUpdates = lockstylesetstyleupdatehelpers::PlanFor(false);
    check(styleUpdates.actionCount == 9 &&
          styleUpdates.actions[0].kind == lockstylesetstyleupdatehelpers::ActionKind::Weapon && styleUpdates.actions[0].slot == 0 &&
          styleUpdates.actions[1].kind == lockstylesetstyleupdatehelpers::ActionKind::Weapon && styleUpdates.actions[1].slot == 1 &&
          styleUpdates.actions[2].kind == lockstylesetstyleupdatehelpers::ActionKind::Weapon && styleUpdates.actions[2].slot == 2 &&
          styleUpdates.actions[3].kind == lockstylesetstyleupdatehelpers::ActionKind::Weapon && styleUpdates.actions[3].slot == 3 &&
          styleUpdates.actions[4].kind == lockstylesetstyleupdatehelpers::ActionKind::Armor && styleUpdates.actions[4].slot == 4 &&
          styleUpdates.actions[8].kind == lockstylesetstyleupdatehelpers::ActionKind::Armor && styleUpdates.actions[8].slot == 8);

    const auto h2hStyleUpdates = lockstylesetstyleupdatehelpers::PlanFor(true);
    check(h2hStyleUpdates.actionCount == 8 && h2hStyleUpdates.actions[0].slot == 0 && h2hStyleUpdates.actions[1].slot == 2 &&
          h2hStyleUpdates.actions[7].kind == lockstylesetstyleupdatehelpers::ActionKind::Armor && h2hStyleUpdates.actions[7].slot == 8);

    const auto setTail = lockstylesettailhelpers::PlanFor();
    check(setTail.actions[0] == lockstylesettailhelpers::ActionKind::UpdateRemovedSlots &&
          setTail.actions[1] == lockstylesettailhelpers::ActionKind::Persist &&
          setTail.actions[2] == lockstylesettailhelpers::ActionKind::Refresh);

    return ok;
}
