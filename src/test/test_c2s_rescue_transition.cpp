/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_rescue_transition.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0f0_rescue.h"

namespace
{

auto expect(bool condition, std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s rescue transition self-test failed: " << label << '\n';
    }
    return condition;
}

auto testTransitions() -> bool
{
    using Action = rescue::Action;

    struct TestCase
    {
        bool        selfUnstuckEnabled;
        bool        cooldownActive;
        uint32_t    now;
        uint32_t    cooldown;
        Action      action;
        bool        setCooldown;
        uint32_t    cooldownExpiry;
        const char* label;
    };

    const auto cases = std::array{
        TestCase{ false, false, 100, 30, Action::None,                false, 0,   "disabled request is a no-op" },
        TestCase{ false, true,  100, 30, Action::None,                false, 0,   "disabled request ignores cooldown" },
        TestCase{ true,  true,  100, 30, Action::SendCooldownMessage, false, 0,   "active cooldown sends message only" },
        TestCase{ true,  false, 100, 30, Action::WarpHomePoint,       true,  130, "eligible request warps and sets expiry" },
        TestCase{ true,  false, UINT32_MAX - 4, 10, Action::WarpHomePoint, true, 5, "expiry uses uint32 timestamp wrap" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        const auto transition = rescue::TransitionFor(test.selfUnstuckEnabled, test.cooldownActive, test.now, test.cooldown);
        ok                    = expect(transition.action == test.action, std::string{ test.label } + " action") && ok;
        ok                    = expect(transition.setCooldown == test.setCooldown, std::string{ test.label } + " set cooldown") && ok;
        ok                    = expect(transition.cooldownExpiry == test.cooldownExpiry, std::string{ test.label } + " expiry") && ok;
    }
    return ok;
}

} // namespace

auto runC2SRescueTransitionSelfTests() -> bool
{
    return testTransitions();
}
