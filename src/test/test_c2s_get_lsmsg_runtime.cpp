/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_get_lsmsg_runtime.h"

#include <initializer_list>
#include <iostream>

#include "map/packets/c2s/get_lsmsg_dispatch.h"

auto runC2SGetLSMsgRuntimeSelfTests() -> bool
{
    using namespace getlsmsgdispatch;

    bool ok = true;
    for (const auto slot : { LinkshellSlot::LS1, LinkshellSlot::LS2 })
    {
        const auto plan = PlanFor(slot);
        if (plan.slot != slot || !plan.pushLinkshellMessage)
        {
            std::cerr << "c2s GET_LSMSG runtime self-test failed: valid slot dispatch\n";
            ok = false;
        }
    }

    const auto invalid = PlanFor(static_cast<LinkshellSlot>(2));
    if (invalid.slot != static_cast<LinkshellSlot>(2) || invalid.pushLinkshellMessage)
    {
        std::cerr << "c2s GET_LSMSG runtime self-test failed: invalid raw slot does not dispatch\n";
        ok = false;
    }

    return ok;
}
