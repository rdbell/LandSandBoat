#include "test_world_http_request_plan_2746.h"

#include "world/http_request_plan.h"

#include <iostream>
#include <string_view>

namespace
{

auto expectPlan(const std::string_view path, const bool refreshAndReadCache, const bool loadSettings) -> bool
{
    const auto plan = worldhttp::PlanHTTPRequestData(path);
    if (plan.refreshAndReadCache == refreshAndReadCache && plan.loadSettings == loadSettings)
    {
        return true;
    }

    std::cerr << "world HTTP request plan 2746 self-test failed: " << path << '\n';
    return false;
}

} // namespace

auto runWorldHTTPRequestPlan2746SelfTests() -> bool
{
    return expectPlan("/api", false, false) &&
           expectPlan("/api/settings", false, true) &&
           expectPlan("/api/sessions", true, false) &&
           expectPlan("/api/zones/0", true, false) &&
           expectPlan("/api/unknown", true, false);
}
