/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_subcontainer_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x03b_subcontainer.h"

auto runC2SSubcontainerRuntimeSelfTests() -> bool
{
    using subcontainerhelpers::BuildTransition;
    using subcontainerhelpers::ItemLock;

    const std::array<uint8, 8> equipment = { 11, 12, 13, 14, 15, 16, 17, 18 };
    const auto equip = BuildTransition(GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip, LOC_STORAGE, 5, equipment, 44);
    const bool equipsWithoutUnlockingReplacedItem = equip.accepted &&
                                                    equip.equipment == std::array<uint8, 8>{ 11, 12, 13, 14, 15, 44, 17, 18 } &&
                                                    equip.lockUpdates.size() == 1 && equip.lockUpdates[0].slot == 44 && equip.lockUpdates[0].lock == ItemLock::Mannequin;

    const auto toggle = BuildTransition(GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip, LOC_STORAGE, 5, equip.equipment, 44);
    const bool togglesEquippedItem = toggle.accepted && toggle.equipment[5] == 0 &&
                                     toggle.lockUpdates.size() == 1 && toggle.lockUpdates[0].slot == 44 && toggle.lockUpdates[0].lock == ItemLock::Normal;

    const auto invalidCategory = BuildTransition(GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip, LOC_MOGSAFE, 0, equipment, 44);
    const bool rejectsNonStorageEquip = !invalidCategory.accepted && invalidCategory.equipment == equipment && invalidCategory.lockUpdates.empty();

    const auto unequip = BuildTransition(GP_CLI_COMMAND_SUBCONTAINER_KIND::Unequip, LOC_MOGSAFE, 3, equipment, 70);
    const bool unequipsSelectedSlot = unequip.accepted && unequip.equipment[3] == 0 &&
                                      unequip.lockUpdates.size() == 1 && unequip.lockUpdates[0].slot == 70 && unequip.lockUpdates[0].lock == ItemLock::Normal;

    const auto all = BuildTransition(GP_CLI_COMMAND_SUBCONTAINER_KIND::UnequipAll, LOC_MOGSAFE, 7, equipment, 0);
    const bool unequipsAll = all.accepted && all.equipment == std::array<uint8, 8>{} && all.lockUpdates.size() == 8 &&
                             all.lockUpdates[0].slot == 11 && all.lockUpdates[7].slot == 18 &&
                             all.lockUpdates[0].lock == ItemLock::Normal && all.lockUpdates[7].lock == ItemLock::Normal;

    if (!equipsWithoutUnlockingReplacedItem || !togglesEquippedItem || !rejectsNonStorageEquip || !unequipsSelectedSlot || !unequipsAll)
    {
        std::cerr << "c2s SUBCONTAINER runtime self-test failed\n";
    }
    return equipsWithoutUnlockingReplacedItem && togglesEquippedItem && rejectsNonStorageEquip && unequipsSelectedSlot && unequipsAll;
}
