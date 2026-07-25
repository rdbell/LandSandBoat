#include "test_c2s_equip_set_runtime.h"

#include "map/packets/c2s/0x050_equip_set.h"

auto runC2SEquipSetRuntimeSelfTests() -> bool
{
    const auto plan = equipsethelpers::MakeDispatchPlan();
    bool       ok   = plan.equip && plan.persist && plan.checkGearSet && plan.updateHealth && plan.retriggerLatents;

    const auto base = equipsethelpers::ValidContainers({});
    ok              = base.contains(LOC_INVENTORY) && base.contains(LOC_WARDROBE) && base.contains(LOC_WARDROBE2) &&
         !base.contains(LOC_WARDROBE3) && !base.contains(LOC_MOGSATCHEL) && ok;

    const auto unlocked = equipsethelpers::ValidContainers({
        .wardrobesAvailable          = { true, false, false, false, false, true },
        .equipFromOtherContainers    = true,
        .mogSatchelAvailable         = true,
        .mogSackAvailable            = true,
        .mogCaseAvailable            = false,
    });
    ok = unlocked.contains(LOC_WARDROBE3) && unlocked.contains(LOC_WARDROBE8) && unlocked.contains(LOC_MOGSATCHEL) &&
         unlocked.contains(LOC_MOGSACK) && !unlocked.contains(LOC_MOGCASE) && ok;

    return ok;
}
