/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_map_config_constants.h"

#include "map/map_config.h"
#include "map/map_constants.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map config/constants self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectTrue(const bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "map config/constants self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testMapConstants() -> bool
{
    bool ok = true;

    ok = expectEqual(std::chrono::duration_cast<std::chrono::milliseconds>(kTimeServerTickInterval).count(), std::int64_t{ 2400 }, "time server tick ms") && ok;
    ok = expectEqual(kLogicUpdateRate, 2.5f, "logic update rate") && ok;
    ok = expectEqual(kLogicUpdateInterval.count(), std::int64_t{ 400 }, "logic update interval ms") && ok;
    ok = expectEqual(kTriggerAreaInterval.count(), std::int64_t{ 200 }, "trigger area interval ms") && ok;
    ok = expectEqual(std::chrono::duration_cast<std::chrono::seconds>(kSpawnHandlerInterval).count(), std::int64_t{ 30 }, "spawn handler interval seconds") && ok;
    ok = expectEqual(std::chrono::duration_cast<std::chrono::seconds>(kSpawnHandlerWindow).count(), std::int64_t{ 15 }, "spawn handler window seconds") && ok;
    ok = expectEqual(kMainThreadBacklogThreshold.count(), std::int64_t{ 200 }, "main thread backlog threshold ms") && ok;
    ok = expectEqual(std::chrono::duration_cast<std::chrono::seconds>(kSessionCleanupInterval).count(), std::int64_t{ 5 }, "session cleanup seconds") && ok;
    ok = expectEqual(std::chrono::duration_cast<std::chrono::minutes>(kGarbageCollectionInterval).count(), std::int64_t{ 15 }, "garbage collection minutes") && ok;
    ok = expectEqual(std::chrono::duration_cast<std::chrono::minutes>(kPersistVolatileServerVarsInterval).count(), std::int64_t{ 1 }, "persist volatile vars minutes") && ok;
    ok = expectEqual(kIPCPumpInterval.count(), std::int64_t{ 100 }, "ipc pump interval ms") && ok;
    ok = expectEqual(kMaxBufferSize, 2500U, "max buffer size") && ok;
    ok = expectEqual(kMaxPacketPerCompression, 32U, "max packet per compression") && ok;
    ok = expectEqual(kMaxPacketBacklogSize, 192U, "max packet backlog size") && ok;

    return ok;
}

auto testMapConfigLayoutAndDefaults() -> bool
{
    bool ok = true;

    ok = expectTrue(std::is_standard_layout_v<MapConfig>, "MapConfig standard layout") && ok;
    ok = expectEqual(sizeof(IPP), std::size_t{ 8 }, "sizeof IPP") && ok;
    ok = expectEqual(sizeof(MapConfig), std::size_t{ 16 }, "sizeof MapConfig") && ok;
    ok = expectEqual(offsetof(MapConfig, ipp), std::size_t{ 0 }, "MapConfig ipp offset") && ok;
    ok = expectEqual(offsetof(MapConfig, inCI), std::size_t{ 8 }, "MapConfig inCI offset") && ok;
    ok = expectEqual(offsetof(MapConfig, isTestServer), std::size_t{ 9 }, "MapConfig isTestServer offset") && ok;
    ok = expectEqual(offsetof(MapConfig, lazyZones), std::size_t{ 10 }, "MapConfig lazyZones offset") && ok;
    ok = expectEqual(offsetof(MapConfig, controlledWeather), std::size_t{ 11 }, "MapConfig controlledWeather offset") && ok;
    ok = expectEqual(offsetof(MapConfig, smokeLuaFiles), std::size_t{ 12 }, "MapConfig smokeLuaFiles offset") && ok;
    ok = expectEqual(offsetof(MapConfig, rebuildNavmeshes), std::size_t{ 13 }, "MapConfig rebuildNavmeshes offset") && ok;

    const MapConfig config{};
    ok = expectEqual(config.ipp.getRawIPP(), std::uint64_t{ 0 }, "default map ipp") && ok;
    ok = expectEqual(config.inCI, false, "default inCI") && ok;
    ok = expectEqual(config.isTestServer, false, "default isTestServer") && ok;
    ok = expectEqual(config.lazyZones, false, "default lazyZones") && ok;
    ok = expectEqual(config.controlledWeather, false, "default controlledWeather") && ok;
    ok = expectEqual(config.smokeLuaFiles, false, "default smokeLuaFiles") && ok;
    ok = expectEqual(config.rebuildNavmeshes, false, "default rebuildNavmeshes") && ok;

    return ok;
}

} // namespace

auto runMapConfigConstantsSelfTests() -> bool
{
    bool ok = true;

    ok = testMapConstants() && ok;
    ok = testMapConfigLayoutAndDefaults() && ok;

    return ok;
}
