#include "test_equip_mod_removal_6871.h"

#include "map/char_equip_mod_removal.h"

#include <iostream>

auto runEquipModRemoval6871SelfTests() -> bool
{
    const bool ok = equipmodremovalhelpers::EquipSlotCount == 16 &&
                    equipmodremovalhelpers::MakePlan(false, 0, 99) == equipmodremovalhelpers::Plan{} &&
                    equipmodremovalhelpers::MakePlan(true, 75, 74) == equipmodremovalhelpers::Plan{ .removeModifiers = true } &&
                    equipmodremovalhelpers::MakePlan(true, 75, 75) == equipmodremovalhelpers::Plan{
                        .removeModifiers    = true,
                        .removeLatentEffects = true,
                        .checkLatents       = true,
                    } &&
                    equipmodremovalhelpers::MakePlan(true, 1, 99) == equipmodremovalhelpers::Plan{
                        .removeModifiers    = true,
                        .removeLatentEffects = true,
                        .checkLatents       = true,
                    };
    if (!ok)
    {
        std::cerr << "equip mod removal 6871 self-test failed\n";
    }
    return ok;
}
