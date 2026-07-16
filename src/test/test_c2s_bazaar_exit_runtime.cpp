/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_bazaar_exit_runtime.h"

#include <iostream>
#include <vector>

#include "map/packets/c2s/0x104_bazaar_exit.h"

auto runC2SBazaarExitRuntimeSelfTests() -> bool
{
    using bazaarexithelpers::SelectStateTransition;

    const auto missingSeller = SelectStateTransition(false, false, false, 0, 0);
    if (missingSeller.removeBuyerCustomer || missingSeller.notifySeller || missingSeller.cleanBazaarID)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: missing seller transition changed\n";
        return false;
    }

    const auto reusedTarget = SelectStateTransition(true, false, false, 0, 0);
    if (reusedTarget.removeBuyerCustomer || reusedTarget.notifySeller || !reusedTarget.cleanBazaarID)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: reused target transition changed\n";
        return false;
    }

    const auto visibleBuyer = SelectStateTransition(true, true, false, 3, 0);
    if (!visibleBuyer.removeBuyerCustomer || !visibleBuyer.notifySeller || !visibleBuyer.cleanBazaarID)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: visible buyer transition changed\n";
        return false;
    }

    const auto hiddenBuyerLowerSeller = SelectStateTransition(true, true, true, 3, 2);
    if (!hiddenBuyerLowerSeller.removeBuyerCustomer || hiddenBuyerLowerSeller.notifySeller || !hiddenBuyerLowerSeller.cleanBazaarID)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: hidden buyer lower-GM notification changed\n";
        return false;
    }

    const auto hiddenBuyerEqualSeller = SelectStateTransition(true, true, true, 3, 3);
    if (!hiddenBuyerEqualSeller.notifySeller)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: hidden buyer equal-GM notification changed\n";
        return false;
    }

    struct Customer
    {
        uint32_t id;
    };
    auto customers = std::vector<Customer>{ { 7 }, { 42 }, { 8 }, { 42 }, { 9 } };
    bazaarexithelpers::RemoveBuyerCustomers(customers, 42);
    if (customers.size() != 3 || customers[0].id != 7 || customers[1].id != 8 || customers[2].id != 9)
    {
        std::cerr << "c2s BAZAAR_EXIT runtime self-test failed: buyer customers were not all removed in order\n";
        return false;
    }

    return true;
}
