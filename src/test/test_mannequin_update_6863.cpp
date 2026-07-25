#include "test_mannequin_update_6863.h"

#include "map/char_mannequin_update.h"

#include <iostream>

auto runMannequinUpdate6863SelfTests() -> bool
{
    bool ok = true;

    const auto missing = mannequinupdatehelpers::MakeMannequinUpdatePlan(false, false, false, false, 0);
    ok = !missing.warnInvalidRace && !missing.sendSubcontainerPacket && ok;

    const auto nonFurnishing = mannequinupdatehelpers::MakeMannequinUpdatePlan(true, false, true, true, 0);
    ok = !nonFurnishing.warnInvalidRace && !nonFurnishing.sendSubcontainerPacket && ok;

    const auto uninstalled = mannequinupdatehelpers::MakeMannequinUpdatePlan(true, true, false, true, 0);
    ok = !uninstalled.warnInvalidRace && !uninstalled.sendSubcontainerPacket && ok;

    const auto nonMannequin = mannequinupdatehelpers::MakeMannequinUpdatePlan(true, true, true, false, 0);
    ok = !nonMannequin.warnInvalidRace && !nonMannequin.sendSubcontainerPacket && ok;

    const auto valid = mannequinupdatehelpers::MakeMannequinUpdatePlan(true, true, true, true, 1);
    ok = !valid.warnInvalidRace && valid.sendSubcontainerPacket && ok;

    const auto invalidRace = mannequinupdatehelpers::MakeMannequinUpdatePlan(true, true, true, true, 0);
    ok = invalidRace.warnInvalidRace && invalidRace.sendSubcontainerPacket && ok;

    if (!ok)
    {
        std::cerr << "mannequin update 6863 self-test failed\n";
    }
    return ok;
}
