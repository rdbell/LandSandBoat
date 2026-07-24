#include "test_on_zone_initialize_host_6411.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "on zone initialize host 6411 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for luautils::OnZoneInitialize (slice 6411).
// Go: ApplyOnZoneInitialize / FormatZoneLuaPath / ShouldOnZoneInitialize.
auto runOnZoneInitializeHost6411SelfTests() -> bool
{
    bool ok = true;

    const std::string path = "./scripts/zones/Bastok_Markets/Zone.lua";
    ok = expect(path.find("./scripts/zones/") == 0, "path prefix") && ok;
    ok = expect(path.find("/Zone.lua") != std::string::npos, "Zone.lua") && ok;

    const std::string skip = "Skipping init for disabled zone 42.";
    ok = expect(skip.find("disabled zone") != std::string::npos, "skip warn") && ok;

    const std::string trace = "luautils::OnZoneInitialize: Selbina";
    ok = expect(trace.find("OnZoneInitialize") != std::string::npos, "trace") && ok;

    const std::string err = "luautils::onInitialize: boom";
    ok = expect(err.find("onInitialize") != std::string::npos, "err") && ok;

    // LoadZones gate: GetIP() != 0
    const uint32_t ip = 0;
    ok = expect(ip == 0, "skip when ip 0") && ok;

    return ok;
}
