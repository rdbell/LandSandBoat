#include "test_world_main_host_6373.h"

#include "world/world_app_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "world main host 6373 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for World main / Application run host residuals (slice 6373).
// Go host halves: worldapp.NewWorldApplication, WorldMain, PlanWorldCreateEngine.
auto runWorldMainHost6373SelfTests() -> bool
{
    using worldapp::WorldHTTPEnableSettingKey;
    using worldapp::WorldMainExitCode;
    using worldapp::WorldServerName;

    bool ok = true;

    ok = expect(std::string(WorldServerName) == "world", "server name") && ok;
    ok = expect(WorldMainExitCode == 0, "main exit 0") && ok;
    ok = expect(std::string(WorldHTTPEnableSettingKey) == "network.ENABLE_HTTP", "http setting key") && ok;

    return ok;
}
