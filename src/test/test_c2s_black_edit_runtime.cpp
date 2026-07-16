/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_black_edit_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x03d_black_edit.h"

auto runC2SBlackEditRuntimeSelfTests() -> bool
{
    using namespace blackedithelpers;

    const bool operations    = OperationFor(GP_CLI_COMMAND_BLACK_EDIT_MODE::Add) == Operation::Add &&
                               OperationFor(GP_CLI_COMMAND_BLACK_EDIT_MODE::Remove) == Operation::Remove;
    const bool lookupFailure = ResponseFor(false, Operation::Add, true) == Response::Error &&
                               ResponseFor(false, Operation::Remove, true) == Response::Error;
    const bool addRouting    = ResponseFor(true, Operation::Add, true) == Response::Add &&
                               ResponseFor(true, Operation::Add, false) == Response::Error;
    const bool removeRouting = ResponseFor(true, Operation::Remove, true) == Response::Delete &&
                               ResponseFor(true, Operation::Remove, false) == Response::Error;
    const bool invalidBypass = ResponseFor(true, Operation::None, true) == Response::None;

    if (!operations || !lookupFailure || !addRouting || !removeRouting || !invalidBypass)
    {
        std::cerr << "c2s BLACK_EDIT runtime self-test failed\n";
    }
    return operations && lookupFailure && addRouting && removeRouting && invalidBypass;
}
