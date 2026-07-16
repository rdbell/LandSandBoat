/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_group_comlink_active_runtime.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x0c4_group_comlink_active.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s GROUP_COMLINK_ACTIVE runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runC2SGroupComlinkActiveRuntimeSelfTests() -> bool
{
    using namespace groupcomlinkactive;
    using Flag = GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG;

    return expect(BranchFor(false, true, true, Flag::EquipOrCreate) == Branch::None, "missing storage item is silent") &&
           expect(BranchFor(true, false, true, Flag::EquipOrCreate) == Branch::None, "non-linkshell item is silent") &&
           expect(BranchFor(true, true, false, Flag::Unequip) == Branch::Unequip, "unequip flag takes its live branch") &&
           expect(BranchFor(true, true, true, Flag::EquipOrCreate) == Branch::Create, "new linkshell creates") &&
           expect(BranchFor(true, true, false, Flag::EquipOrCreate) == Branch::Equip, "existing linkshell equips");
}
