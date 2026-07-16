/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_equipset_check_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x052_equipset_check.h"

auto runC2SEquipSetCheckRuntimeSelfTests() -> bool
{
    if (equipsetcheckhelpers::SelectReply() != equipsetcheckhelpers::Reply::EquipSetValid)
    {
        std::cerr << "c2s EQUIPSET_CHECK runtime self-test failed: response is not EQUIPSET_VALID\n";
        return false;
    }
    return true;
}
