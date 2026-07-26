/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_map_networking_session_container_6936.h"

#include <iostream>

#include "common/ipp.h"
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
        std::cerr << "map networking session-container self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runMapNetworkingSessionContainer6936SelfTests() -> bool
{
    auto config         = MapConfig{};
    config.isTestServer = true;

    Scheduler     scheduler(1);
    MapStatistics statistics;
    MapNetworking networking(scheduler, statistics, config);

    // mapSessions_ is a value member, independent of the socket-open gate.
    auto& sessions = networking.sessions();
    bool ok        = true;
    ok             = expect(sessions.getSessionByIPP(IPP(0x01020304, 54000)) == nullptr, "empty owned lookup") && ok;
    ok             = expect(&sessions == &networking.sessions(), "stable owned accessor") && ok;
    return ok;
}
