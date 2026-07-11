#include "test_map_engine_config_1338.h"

#include "map/map_app_config.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine config 1338 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapEngineConfig1338SelfTests() -> bool
{
    bool ok = true;

    ok = expect(mapapp::ResolveMapBindIP(false, 0x01020304) == 0, "ip absent") && ok;
    ok = expect(mapapp::ResolveMapBindIP(true, 0x01020304) == 0x01020304, "ip present") && ok;

    ok = expect(mapapp::ResolveMapBindPort(false, 54230) == 0, "port absent") && ok;
    ok = expect(mapapp::ResolveMapBindPort(true, 54230) == 54230, "port present") && ok;
    ok = expect(mapapp::ResolveMapBindPort(true, -1) == 0, "port negative") && ok;
    ok = expect(mapapp::ResolveMapBindPort(true, 70000) == 65535, "port over") && ok;

    const auto cfg = mapapp::BuildMapEngineConfigInputs(1, 2, true, true, false);
    ok             = expect(cfg.ip == 1 && cfg.port == 2 && cfg.inCI && cfg.lazyZones && !cfg.rebuildNavmeshes, "build inputs") && ok;

    return ok;
}
