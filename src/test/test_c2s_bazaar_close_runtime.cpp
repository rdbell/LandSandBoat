/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_bazaar_close_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10b_bazaar_close.h"

auto runC2SBazaarCloseRuntimeSelfTests() -> bool
{
    const auto transition = bazaarclosehelpers::SelectStateTransition();
    if (!transition.clearCustomers || !transition.settingBazaarPrices || !transition.setUpdateHP)
    {
        std::cerr << "c2s BAZAAR_CLOSE runtime self-test failed: close state transition changed\n";
        return false;
    }

    if (!bazaarclosehelpers::ShouldNotifyCustomer(true, 101, 101))
    {
        std::cerr << "c2s BAZAAR_CLOSE runtime self-test failed: current customer was not notified\n";
        return false;
    }

    if (bazaarclosehelpers::ShouldNotifyCustomer(false, 101, 101))
    {
        std::cerr << "c2s BAZAAR_CLOSE runtime self-test failed: absent customer was notified\n";
        return false;
    }

    if (bazaarclosehelpers::ShouldNotifyCustomer(true, 102, 101))
    {
        std::cerr << "c2s BAZAAR_CLOSE runtime self-test failed: reused target ID was notified\n";
        return false;
    }

    return true;
}
