/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

=========================================================================
*/

#include "http_server_api.h"

#include "common/utils.h"
#include "common/xi.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string_view>

namespace
{

constexpr std::string_view ZonePathPrefix = "/api/zones/";

auto isVisibleSettingKey(const std::string& key) -> bool
{
    const auto lowerKey = to_lower(key);
    return lowerKey.find("logging.") == std::string::npos && lowerKey.find("network.") == std::string::npos &&
           lowerKey.find("password") == std::string::npos;
}

auto notFoundResponse() -> HTTPServerAPIResponse
{
    return { .status = 404 };
}

} // namespace

auto makeHTTPServerAPIResponse(const std::string& path, const HTTPServerAPIDataCache& cache,
                               const HTTPServerAPISettings& settings) -> HTTPServerAPIResponse
{
    if (path == "/api")
    {
        return { .contentType = "text/plain", .body = "Hello LSB API" };
    }

    if (path == "/api/sessions")
    {
        return { .contentType = "application/json", .body = nlohmann::json(cache.activeSessionCount).dump() };
    }

    if (path == "/api/ips")
    {
        return { .contentType = "application/json", .body = nlohmann::json(cache.activeUniqueIPCount).dump() };
    }

    if (path == "/api/zones")
    {
        return { .contentType = "application/json", .body = nlohmann::json(cache.zonePlayerCounts).dump() };
    }

    if (path.starts_with(ZonePathPrefix))
    {
        const auto zoneText = std::string_view(path).substr(ZonePathPrefix.size());
        if (zoneText.empty() || !std::ranges::all_of(zoneText, [](const unsigned char character) { return std::isdigit(character); }))
        {
            return notFoundResponse();
        }

        // HTTPServer's original route used std::strtol and then assigned the
        // result to uint16. Preserve that established conversion behavior in
        // this testable response helper.
        const uint16 zoneId = std::strtol(std::string(zoneText).c_str(), nullptr, 10);
        if (zoneId == 0 || zoneId >= ZONEID::MAX_ZONEID)
        {
            return notFoundResponse();
        }

        return { .contentType = "application/json", .body = nlohmann::json(cache.zonePlayerCounts[zoneId]).dump() };
    }

    if (path == "/api/settings")
    {
        nlohmann::json response{};
        for (const auto& [key, value] : settings)
        {
            if (!isVisibleSettingKey(key))
            {
                continue;
            }

            std::visit(
                xi::overload{
                    [&](const bool arg) { response[key] = arg; },
                    [&](const double arg) { response[key] = arg; },
                    [&](const std::string& arg) { response[key] = utils::toASCII(arg, '?'); },
                },
                value);
        }
        return { .contentType = "application/json", .body = response.dump() };
    }

    return notFoundResponse();
}
