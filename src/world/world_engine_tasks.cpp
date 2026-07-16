/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "world_engine_tasks.h"

#include <common/cbasetypes.h>
#include <map/map_constants.h>

WorldEngineRecurringTasks::WorldEngineRecurringTasks(std::unique_ptr<WorldEngineTaskToken> timeServerToken, std::unique_ptr<WorldEngineTaskToken> incomingIPCPumpToken)
: timeServerToken_(std::move(timeServerToken))
, incomingIPCPumpToken_(std::move(incomingIPCPumpToken))
{
}

auto registerWorldEngineRecurringTasks(WorldEngineTaskRegistrar& registrar, WorldEngineTaskCallbacks callbacks) -> WorldEngineRecurringTasks
{
    auto timeServerToken = registrar.registerTimeServer(kTimeServerTickInterval, std::move(callbacks.timeServer));
    auto incomingIPCPumpToken = registrar.registerIncomingIPCPump(kIPCPumpInterval, std::move(callbacks.incomingIPCPump));
    return { std::move(timeServerToken), std::move(incomingIPCPumpToken) };
}
