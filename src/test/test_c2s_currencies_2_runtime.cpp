/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_currencies_2_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x115_currencies_2.h"

auto runC2SCurrencies2RuntimeSelfTests() -> bool
{
    if (currencies2packethelpers::SelectAction() != currencies2packethelpers::Action::SendCurrencies2)
    {
        std::cerr << "c2s CURRENCIES_2 runtime self-test failed: action is not SendCurrencies2\\n";
        return false;
    }
    return true;
}
