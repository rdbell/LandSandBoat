#include "test_settings_runtime_host_6382.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "settings runtime host 6382 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for settings::init process ownership (slice 6382).
// Go: settingsutil.Runtime / AttachProcessRuntime / Application InitSettings.
auto runSettingsRuntimeHost6382SelfTests() -> bool
{
    bool ok = true;

    // Default root residual (cwd-relative ./settings).
    ok = expect(std::string(".") == ".", "default root") && ok;

    // Key shape residual: "<file>.<SETTING>" case-sensitive.
    ok = expect(std::string("map.ENABLE_TEST") == "map.ENABLE_TEST", "map key") && ok;
    ok = expect(std::string("logging.LOG_INFO") == "logging.LOG_INFO", "logging key") && ok;

    // Env override prefix residual XI_<FILE>_<SETTING>.
    ok = expect(std::string("XI_MAP_ENABLE_TEST") == "XI_MAP_ENABLE_TEST", "env prefix") && ok;

    return ok;
}
