/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

=========================================================================
*/

#include "test_world_http_server_api.h"

#include "world/http_server_api.h"

#include <iostream>

namespace
{

auto expectResponse(const std::string& path, const HTTPServerAPIDataCache& cache, const HTTPServerAPISettings& settings,
                    int status, const std::string& contentType, const std::string& body) -> bool
{
    const auto response = makeHTTPServerAPIResponse(path, cache, settings);
    if (response.status == status && response.contentType == contentType && response.body == body)
    {
        return true;
    }

    std::cerr << "World HTTP server API self-test failed: " << path << '\n';
    return false;
}

} // namespace

auto runWorldHTTPServerAPISelfTests() -> bool
{
    HTTPServerAPIDataCache cache;
    cache.activeSessionCount      = 12;
    cache.activeUniqueIPCount     = 4;
    cache.zonePlayerCounts[2]     = 9;

    const HTTPServerAPISettings settings{
        { "main.ENABLED", true },
        { "main.NAME", std::string("Caf\xC3\xA9") },
        { "map.RATE", 1.5 },
        { "network.HTTP_PORT", 54230.0 },
        { "database.PASSWORD", std::string("secret") },
        { "logging.LOG_LEVEL", std::string("debug") },
    };

    bool ok = true;
    ok      = expectResponse("/api", cache, settings, 200, "text/plain", "Hello LSB API") && ok;
    ok      = expectResponse("/api/sessions", cache, settings, 200, "application/json", "12") && ok;
    ok      = expectResponse("/api/ips", cache, settings, 200, "application/json", "4") && ok;
    ok      = expectResponse("/api/zones/2", cache, settings, 200, "application/json", "9") && ok;
    ok      = expectResponse("/api/zones/0", cache, settings, 404, "", "") && ok;
    ok      = expectResponse("/api/zones/300", cache, settings, 404, "", "") && ok;
    ok      = expectResponse("/api/zones/nope", cache, settings, 404, "", "") && ok;

    const auto response = makeHTTPServerAPIResponse("/api/settings", cache, settings);
    ok                  = response.status == 200 && response.contentType == "application/json" &&
         response.body.find("main.ENABLED") != std::string::npos && response.body.find("main.NAME") != std::string::npos &&
         response.body.find("Caf?") != std::string::npos && response.body.find("map.RATE") != std::string::npos &&
         response.body.find("network.HTTP_PORT") == std::string::npos && response.body.find("PASSWORD") == std::string::npos &&
         response.body.find("logging.LOG_LEVEL") == std::string::npos && ok;
    if (!ok)
    {
        std::cerr << "World HTTP server settings self-test failed\n";
    }
    return ok;
}
