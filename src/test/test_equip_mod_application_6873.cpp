#include "test_equip_mod_application_6873.h"

#include "map/char_equip_mod_update.h"

#include <iostream>

auto runEquipModApplication6873SelfTests() -> bool
{
    const bool ok = equipmodupdatehelpers::MakePlan(false, 0, 99) == equipmodupdatehelpers::Plan{} &&
                    equipmodupdatehelpers::MakePlan(true, 75, 74) == equipmodupdatehelpers::Plan{ .updateModifiers = true } &&
                    equipmodupdatehelpers::MakePlan(true, 75, 75) == equipmodupdatehelpers::Plan{
                        .updateModifiers     = true,
                        .updateLatentEffects = true,
                        .checkLatents        = true,
                    };
    if (!ok)
    {
        std::cerr << "equip mod application 6873 self-test failed\n";
    }
    return ok;
}
