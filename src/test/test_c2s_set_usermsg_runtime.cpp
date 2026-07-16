/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_set_usermsg_runtime.h"

#include <array>
#include <iostream>

#include "map/packets/c2s/0x0e0_set_usermsg.h"

auto runC2SSetUserMsgRuntimeSelfTests() -> bool
{
    using namespace setusermsgpackethelpers;

    auto empty = std::array<uint8_t, 128>{};
    const auto emptyPlan = PlanFor(empty, static_cast<uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Others), { .message = "old", .messageType = 0x73 }, true);
    if (emptyPlan.type != GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Default || emptyPlan.message != "" || !emptyPlan.persist || !emptyPlan.updateSearchState || !emptyPlan.sendCharStatus)
    {
        std::cerr << "c2s SET_USERMSG runtime self-test failed: empty message must use Default and update after persistence\n";
        return false;
    }

    auto message = std::array<uint8_t, 128>{};
    message[0] = 's';
    message[1] = 'e';
    message[2] = 'a';
    const auto unchanged = PlanFor(message, static_cast<uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Others), { .message = "sea", .messageType = 0x73 }, true);
    if (unchanged.persist || unchanged.updateSearchState || unchanged.sendCharStatus)
    {
        std::cerr << "c2s SET_USERMSG runtime self-test failed: unchanged message must be a no-op\n";
        return false;
    }

    const auto failed = PlanFor(message, static_cast<uint32_t>(GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Others), { .message = "old", .messageType = 0x61 }, false);
    if (!failed.persist || failed.updateSearchState || !failed.sendCharStatus || failed.message != "sea" || failed.type != GP_CLI_COMMAND_SET_USERMSG_MSGTYPE::Others)
    {
        std::cerr << "c2s SET_USERMSG runtime self-test failed: failed persistence must still refresh status without changing search state\n";
        return false;
    }

    return true;
}
