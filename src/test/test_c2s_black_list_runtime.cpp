/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_black_list_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x03c_black_list.h"

auto runC2SBlackListRuntimeSelfTests() -> bool
{
    if (blacklistpackethelpers::SelectAction() != blacklistpackethelpers::Action::SendBlacklist)
    {
        std::cerr << "c2s BLACK_LIST runtime self-test failed: action is not SendBlacklist\n";
        return false;
    }
    return true;
}
