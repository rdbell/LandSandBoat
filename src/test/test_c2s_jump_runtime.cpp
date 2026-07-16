/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_jump_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x11d_jump.h"

auto runC2SJumpRuntimeSelfTests() -> bool
{
    constexpr uint16_t actIndex = 0x1234;

    const auto jailed = jumphelpers::SelectDispatchPlan(true, actIndex);
    if (jailed.action != jumphelpers::Action::RejectInPrison)
    {
        std::cerr << "c2s JUMP runtime self-test failed: imprisoned character does not reject\n";
        return false;
    }

    const auto allowed = jumphelpers::SelectDispatchPlan(false, actIndex);
    if (allowed.action != jumphelpers::Action::BroadcastSelfRange || allowed.actIndex != actIndex)
    {
        std::cerr << "c2s JUMP runtime self-test failed: allowed character does not self-range broadcast original ActIndex\n";
        return false;
    }

    return true;
}
