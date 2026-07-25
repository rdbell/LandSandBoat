#include "test_equip_mod_removal_6871.h"

#include "map/char_equip_mod_update.h"

#include <iostream>

auto runEquipModRemoval6871SelfTests() -> bool
{
    const bool ok = equipmodupdatehelpers::EquipSlotCount == 16 &&
                    equipmodupdatehelpers::MakePlan(false, 0, 99) == equipmodupdatehelpers::Plan{} &&
                    equipmodupdatehelpers::MakePlan(true, 75, 74) == equipmodupdatehelpers::Plan{ .updateModifiers = true } &&
                    equipmodupdatehelpers::MakePlan(true, 75, 75) == equipmodupdatehelpers::Plan{
                        .updateModifiers     = true,
                        .updateLatentEffects = true,
                        .checkLatents        = true,
                    } &&
                    equipmodupdatehelpers::MakePlan(true, 1, 99) == equipmodupdatehelpers::Plan{
                        .updateModifiers     = true,
                        .updateLatentEffects = true,
                        .checkLatents        = true,
                    };
    if (!ok)
    {
        std::cerr << "equip mod removal 6871 self-test failed\n";
    }
    return ok;
}
