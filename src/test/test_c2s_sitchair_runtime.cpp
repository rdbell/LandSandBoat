/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_sitchair_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x113_sitchair.h"

auto runC2SSitChairRuntimeSelfTests() -> bool
{
    using sitchairhelpers::AnimationNone;
    using sitchairhelpers::AnimationSitchair0;
    using sitchairhelpers::ChairKeyItemID;
    using sitchairhelpers::RequiresChairKeyItem;
    using sitchairhelpers::SelectTransition;

    constexpr auto Toggle = static_cast<uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::Toggle);
    constexpr auto On     = static_cast<uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::On);
    constexpr auto Off    = static_cast<uint8_t>(GP_CLI_COMMAND_SITCHAIR_MODE::Off);

    const auto chair0 = SelectTransition(Toggle, AnimationSitchair0, AnimationNone, true);
    if (chair0.animation != AnimationSitchair0 || !chair0.removeHealingSilently || !chair0.setUpdateHP)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: chair zero transition changed\n";
        return false;
    }

    const auto toggleOff = SelectTransition(Toggle, AnimationSitchair0, AnimationSitchair0, true);
    if (toggleOff.animation != AnimationNone || !toggleOff.removeHealingSilently || !toggleOff.setUpdateHP)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: toggle-off transition changed\n";
        return false;
    }

    const auto onStillToggles = SelectTransition(On, AnimationSitchair0, AnimationSitchair0, true);
    if (onStillToggles.animation != AnimationNone)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: On no longer toggles\n";
        return false;
    }

    const auto off = SelectTransition(Off, AnimationSitchair0 + 7, AnimationSitchair0 + 7, false);
    if (off.animation != AnimationNone || !off.removeHealingSilently || !off.setUpdateHP)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: Off transition changed\n";
        return false;
    }

    const auto gated = SelectTransition(Toggle, AnimationSitchair0 + 1, AnimationNone, false);
    if (gated.animation != AnimationSitchair0)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: missing key item did not fall back\n";
        return false;
    }

    const auto owned = SelectTransition(Toggle, AnimationSitchair0 + 1, AnimationNone, true);
    if (owned.animation != AnimationSitchair0 + 1)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: owned key item did not select chair\n";
        return false;
    }

    if (RequiresChairKeyItem(AnimationSitchair0) || !RequiresChairKeyItem(AnimationSitchair0 + 1) ||
        !RequiresChairKeyItem(AnimationSitchair0 + 20) || ChairKeyItemID(AnimationSitchair0 + 1) != 0xB0A ||
        ChairKeyItemID(AnimationSitchair0 + 20) != 0xB1D)
    {
        std::cerr << "c2s SITCHAIR runtime self-test failed: key-item range or IDs changed\n";
        return false;
    }

    return true;
}
