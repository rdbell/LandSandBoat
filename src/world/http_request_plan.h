#pragma once

#include <string_view>

// Pure APIResponse data-source policy. HTTPServer invokes APIResponse only for
// /api.* routes, and every route other than the API greeting and settings
// endpoint refreshes and snapshots the cache before response shaping.
namespace worldhttp
{

struct HTTPRequestDataPlan
{
    bool refreshAndReadCache{};
    bool loadSettings{};
};

inline auto PlanHTTPRequestData(const std::string_view path) -> HTTPRequestDataPlan
{
    return {
        .refreshAndReadCache = path != "/api" && path != "/api/settings",
        .loadSettings        = path == "/api/settings",
    };
}

} // namespace worldhttp
