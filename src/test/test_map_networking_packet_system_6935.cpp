/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_packet_system_6935.h"

#include <iostream>

#include "common/scheduler.h"
#include "map/map_config.h"
#include "map/map_networking.h"
#include "map/map_statistics.h"

namespace
{

auto expect(bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "map networking packet-system self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingPacketSystem6935SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);

    bool ok = true;
    ok      = expect(networking.packetSystem().registeredHandlerCount() == 129, "owned handler count") && ok;
    ok      = expect(networking.packetSystem().isHandlerRegistered(0x00A), "owned LOGIN handler") && ok;
    ok      = expect(!networking.packetSystem().isHandlerRegistered(0x066), "owned FISHING alias hole") && ok;
    return ok;
}
