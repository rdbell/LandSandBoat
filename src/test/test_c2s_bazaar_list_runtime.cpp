/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_bazaar_list_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x105_bazaar_list.h"

auto runC2SBazaarListRuntimeSelfTests() -> bool
{
    using bazaarlisthelpers::CanOpenBazaar;
    using bazaarlisthelpers::Lookup;
    using bazaarlisthelpers::SelectLookup;
    using bazaarlisthelpers::ShouldListItem;
    using bazaarlisthelpers::ShouldNotifySeller;

    if (SelectLookup(0) != Lookup::CurrentTarget || SelectLookup(123) != Lookup::UniqueNo)
    {
        std::cerr << "c2s BAZAAR_LIST runtime self-test failed: seller lookup selection changed\n";
        return false;
    }

    if (!CanOpenBazaar(true, 123, 123, true) || CanOpenBazaar(false, 123, 123, true) ||
        CanOpenBazaar(true, 124, 123, true) || CanOpenBazaar(true, 123, 123, false))
    {
        std::cerr << "c2s BAZAAR_LIST runtime self-test failed: seller access guard changed\n";
        return false;
    }

    if (!ShouldNotifySeller(false, 99, 0) || ShouldNotifySeller(true, 4, 3) ||
        !ShouldNotifySeller(true, 4, 4) || !ShouldNotifySeller(true, 4, 5))
    {
        std::cerr << "c2s BAZAAR_LIST runtime self-test failed: GM-hidden notification rule changed\n";
        return false;
    }

    if (!ShouldListItem(true, 1) || ShouldListItem(false, 1) || ShouldListItem(true, 0))
    {
        std::cerr << "c2s BAZAAR_LIST runtime self-test failed: sale-item filter changed\n";
        return false;
    }

    return true;
}
