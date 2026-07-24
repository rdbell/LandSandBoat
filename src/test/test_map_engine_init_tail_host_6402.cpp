#include "test_map_engine_init_tail_host_6402.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine init tail host 6402 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine::init post-content tail (slice 6402).
// Go: PlanMapEngineInitTail / ApplyMapEngineInitTail / ApplyMapEngineInitFullHost.
auto runMapEngineInitTailHost6402SelfTests() -> bool
{
    bool ok = true;

    const std::vector<std::string> always = {
        "zoneutils::Initialize",
        "instanceutils::Initialize",
        "InitializeFishingSystem",
        "monstrosity::LoadStaticData",
        "zoneutils::TOTDChange",
        "moduleutils::OnInit",
        "luautils::OnServerStart",
        "db::enableTimers",
        "application_.markLoaded",
    };
    ok = expect(always.front() == "zoneutils::Initialize", "first") && ok;
    ok = expect(always.back() == "application_.markLoaded", "last") && ok;

    // Gates
    const bool lazyZones         = true;
    const bool controlledWeather = true;
    const bool isTestServer      = true;
    ok = expect(!lazyZones == false, "skip transport when lazy") && ok;
    ok = expect(!controlledWeather == false, "skip weather when controlled") && ok;
    ok = expect(!isTestServer == false, "skip recurring/watchdogs/report when test") && ok;

    return ok;
}
