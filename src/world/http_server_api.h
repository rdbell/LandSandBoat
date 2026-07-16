/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

=========================================================================
*/

#pragma once

#include "map/zone.h"

#include "common/timer.h"

#include <array>
#include <cstdint>
#include <string>
#include <optional>
#include <utility>
#include <unordered_map>
#include <variant>
#include <vector>

struct HTTPServerAPIDataCache
{
    uint32                                 activeSessionCount{};
    uint32                                 activeUniqueIPCount{};
    std::array<uint32, ZONEID::MAX_ZONEID> zonePlayerCounts{};
};

using HTTPServerAPISettings = std::unordered_map<std::string, std::variant<bool, double, std::string>>;

struct HTTPServerAPIResponse
{
    int         status{ 200 };
    std::string contentType;
    std::string body;
};

// HTTPServerAPIRefresh is one attempted cache refresh. Missing scalar values
// model failed/empty database queries; zone rows update only their own slots.
struct HTTPServerAPIRefresh
{
    std::optional<uint32>                    activeSessionCount;
    std::optional<uint32>                    activeUniqueIPCount;
    std::vector<std::pair<uint16, uint32>> zonePlayerCounts;
};

// ApplyHTTPServerAPIRefresh updates cache fields supplied by refresh and
// returns attemptTime for HTTPServer's last-update timestamp.
auto ApplyHTTPServerAPIRefresh(HTTPServerAPIDataCache& cache, const HTTPServerAPIRefresh& refresh,
                               timer::time_point attemptTime) -> timer::time_point;

// Returns the response body and status for a GET path registered by
// HTTPServer. Cache and settings snapshots are supplied by the host so this
// route logic can remain independent of database and scheduler ownership.
auto makeHTTPServerAPIResponse(const std::string& path, const HTTPServerAPIDataCache& cache,
                               const HTTPServerAPISettings& settings) -> HTTPServerAPIResponse;
