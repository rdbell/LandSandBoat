/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_set_lsmsg_runtime.h"

#include <array>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0e2_set_lsmsg.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s SET_LSMSG runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto message() -> std::array<uint8_t, 128>
{
    auto value = std::array<uint8_t, 128>{};
    value[0]   = 'm';
    value[1]   = 'o';
    value[2]   = 't';
    value[3]   = 'd';
    value[5]   = 'x';
    return value;
}

auto testEquipmentAndAccessPriority() -> bool
{
    using namespace setlsmsgpackethelpers;

    const auto owner = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Linkshell, .postRights = 0 };
    const auto sack  = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Pearlsack, .postRights = 2 };

    const auto ignored = PlanFor({}, true, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack, message());
    const auto change  = PlanFor(owner, true, false, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack, message());
    const auto denied  = PlanFor(sack, true, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkpearl, message());

    return expect(!ignored.setPostRights && !ignored.setMessage && !ignored.sendLinkshellNoAccess, "missing LS1 is silent") &&
           expect(change.setPostRights && change.postRights == GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Pearlsack && !change.setMessage, "owner changes post rights") &&
           expect(denied.sendLinkshellNoAccess && !denied.setMessage && !denied.setPostRights, "access flag wins over message for non-owner");
}

auto testQuirkyMessageRightsAndBoundedMessage() -> bool
{
    using namespace setlsmsgpackethelpers;

    const auto ownerOnly = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Linkshell, .postRights = 1 };
    const auto sackOnly  = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Pearlsack, .postRights = 2 };
    const auto pearl     = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Linkpearl, .postRights = 3 };
    const auto zero      = Facts{ .hasEquippedLS1 = true, .equippedItemIsLinkshell = true, .equippedItemType = ItemType::Linkshell, .postRights = 0 };
    auto noNul = std::array<uint8_t, 128>{};
    noNul.fill('x');

    const auto ownerPlan = PlanFor(ownerOnly, false, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, message());
    const auto sackPlan  = PlanFor(sackOnly, false, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, message());
    const auto pearlPlan = PlanFor(pearl, false, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, message());
    const auto zeroPlan  = PlanFor(zero, false, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, message());
    const auto longPlan  = PlanFor(pearl, false, true, GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, noNul);

    return expect(ownerPlan.setMessage && ownerPlan.message == "motd", "post rights one allows owner and bounds at NUL") &&
           expect(sackPlan.setMessage && sackPlan.message == "motd", "post rights two allows pearlsack") &&
           expect(pearlPlan.setMessage && pearlPlan.message == "motd", "post rights three allows linkpearl") &&
           expect(zeroPlan.sendLinkshellNoAccess, "post rights zero underflow denies owner") &&
           expect(longPlan.setMessage && longPlan.message == std::string(128, 'x'), "unterminated message remains bounded to field");
}

} // namespace

auto runC2SSetLSMsgRuntimeSelfTests() -> bool
{
    return testEquipmentAndAccessPriority() && testQuirkyMessageRightsAndBoundedMessage();
}
