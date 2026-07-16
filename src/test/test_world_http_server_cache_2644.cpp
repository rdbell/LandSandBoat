#include "test_world_http_server_cache_2644.h"

#include "world/http_server_api.h"

#include <chrono>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world HTTP cache 2644 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runWorldHTTPServerCache2644SelfTests() -> bool
{
    HTTPServerAPIDataCache cache{};
    cache.activeSessionCount  = 7;
    cache.activeUniqueIPCount = 3;
    cache.zonePlayerCounts[2] = 11;
    cache.zonePlayerCounts[3] = 22;

    const auto attempt = timer::time_point{ std::chrono::seconds{ 2644 } };
    const auto committed = ApplyHTTPServerAPIRefresh(cache,
        {
            .activeSessionCount  = 12,
            .activeUniqueIPCount = 4,
            .zonePlayerCounts    = { { 2, 9 } },
        },
        attempt);

    bool ok = true;
    ok = expect(cache.activeSessionCount == 12 && cache.activeUniqueIPCount == 4, "successful scalar updates") && ok;
    ok = expect(cache.zonePlayerCounts[2] == 9 && cache.zonePlayerCounts[3] == 22, "partial zone rows preserve absent zones") && ok;
    ok = expect(committed == attempt, "attempt commits last update") && ok;

    const auto failedAttempt = timer::time_point{ std::chrono::seconds{ 2645 } };
    const auto failedCommit  = ApplyHTTPServerAPIRefresh(cache, {}, failedAttempt);
    ok = expect(cache.activeSessionCount == 12 && cache.activeUniqueIPCount == 4 && cache.zonePlayerCounts[2] == 9 && cache.zonePlayerCounts[3] == 22,
                "failed query fields unchanged") && ok;
    ok = expect(failedCommit == failedAttempt, "failed refresh still commits last update") && ok;
    return ok;
}
