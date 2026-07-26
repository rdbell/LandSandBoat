/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_incoming_callback_6939.h"

#include <array>
#include <iostream>

#include "common/scheduler.h"
#include "map/map_config.h"
#include "map/map_networking.h"
#include "map/map_statistics.h"

namespace
{

auto expect(const bool actual, const char* label) -> bool
{
    if (!actual)
    {
        std::cerr << "map networking incoming-callback self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingIncomingCallback6939SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);

    const auto source = IPP{};
    std::array<uint8, 1> packet{};
    networking.handle_incoming_packet({ packet.data(), packet.size() }, source);

    return expect(networking.sessions().getSessionByIPP(source) == nullptr,
                  "no-session inbound callback returns without creating a session");
}
