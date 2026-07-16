/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_effectend_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x059_effectend.h"

auto runC2SEffectEndRuntimeSelfTests() -> bool
{
    if (effectendhelpers::SelectAction() != effectendhelpers::Action::None)
    {
        std::cerr << "c2s EFFECTEND runtime self-test failed: action is not none\n";
        return false;
    }
    return true;
}
