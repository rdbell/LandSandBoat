/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_clistatus_runtime.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x061_clistatus.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s CLISTATUS runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testBothValidUnknownValuesSelectTheSameRefresh() -> bool
{
    auto zero      = GP_CLI_COMMAND_CLISTATUS{};
    zero.unknown00 = 0;
    auto one       = GP_CLI_COMMAND_CLISTATUS{};
    one.unknown00  = 1;

    const auto plan = clistatushelpers::MakeDispatchPlan();
    return expect(zero.validate(nullptr, nullptr).valid(), "unknown00 zero validates") &&
           expect(one.validate(nullptr, nullptr).valid(), "unknown00 one validates") &&
           expect(plan.refreshLocalPlayerPackets, "both valid values refresh local player packets");
}

auto testInvalidUnknownValueDoesNotReachTheProcessPlan() -> bool
{
    auto invalid      = GP_CLI_COMMAND_CLISTATUS{};
    invalid.unknown00 = 2;

    return expect(!invalid.validate(nullptr, nullptr).valid(), "unknown00 two rejects before process");
}

} // namespace

auto runC2SCLIStatusRuntimeSelfTests() -> bool
{
    return testBothValidUnknownValuesSelectTheSameRefresh() &&
           testInvalidUnknownValueDoesNotReachTheProcessPlan();
}
