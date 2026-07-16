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

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

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

// Returns the response body and status for a GET path registered by
// HTTPServer. Cache and settings snapshots are supplied by the host so this
// route logic can remain independent of database and scheduler ownership.
auto makeHTTPServerAPIResponse(const std::string& path, const HTTPServerAPIDataCache& cache,
                               const HTTPServerAPISettings& settings) -> HTTPServerAPIResponse;
