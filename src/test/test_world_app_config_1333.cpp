#include "test_world_app_config_1333.h"

#include "world/world_app_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world app config 1333 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runWorldAppConfig1333SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string(worldapp::WorldServerName) == "world", "server name") && ok;
    ok = expect(std::string(worldapp::WorldHTTPEnableSettingKey) == "network.ENABLE_HTTP", "http setting key") && ok;

    return ok;
}
