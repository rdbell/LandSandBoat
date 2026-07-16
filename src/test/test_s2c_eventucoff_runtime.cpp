/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_eventucoff_runtime.h"

#include <cstdint>
#include <iostream>

#include "map/packets/s2c/eventucoff_runtime.h"

auto runS2CEventUCOffRuntimeSelfTests() -> bool
{
    const auto active = eventucoffhelpers::PlanFor(GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent, {
                                                                                                     .hasCurrentEvent = true,
                                                                                                     .eventID         = 0x1234,
                                                                                                 });
    const auto absent = eventucoffhelpers::PlanFor(GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent, {});
    const auto normal = eventucoffhelpers::PlanFor(GP_SERV_COMMAND_EVENTUCOFF_MODE::Standard, {});
    const auto ok     = active.mode == 0x00123402 && active.resetSubstate && absent.mode == 2 && absent.resetSubstate && normal.mode == 0 && normal.resetSubstate;
    if (!ok)
    {
        std::cerr << "s2c EVENTUCOFF runtime self-test failed: mode planning\n";
    }
    return ok;
}
