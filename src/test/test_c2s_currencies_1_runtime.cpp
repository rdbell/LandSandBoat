/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_currencies_1_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10f_currencies_1.h"

auto runC2SCurrencies1RuntimeSelfTests() -> bool
{
    if (currencies1packethelpers::SelectAction() != currencies1packethelpers::Action::SendCurrencies1)
    {
        std::cerr << "c2s CURRENCIES_1 runtime self-test failed: action is not SendCurrencies1\\n";
        return false;
    }
    return true;
}
