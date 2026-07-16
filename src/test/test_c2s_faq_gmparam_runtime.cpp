/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_faq_gmparam_runtime.h"

#include <array>
#include <iostream>

#include "map/packets/c2s/0x0d4_faq_gmparam.h"

auto runC2SFAQGMParamRuntimeSelfTests() -> bool
{
    constexpr auto id      = uint16_t{ 0x4A7C };
    constexpr auto options = std::array<uint16_t, 3>{ 0, 1, 0xFFFF };

    for (const auto option : options)
    {
        const auto response = faqgmparampackethelpers::ResponseFor(id, option);
        if (response.Id != id)
        {
            std::cerr << "c2s FAQ_GMPARAM runtime self-test failed: response Id was not reflected for Option " << option << '\n';
            return false;
        }
    }

    return true;
}
