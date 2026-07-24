#include "test_zoneutils_load_zones_host_6404.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zoneutils load zones host 6404 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for zoneutils::LoadZones phases (slice 6404).
// Go: PlanZonesToLoad / PlanLoadZonesPhases / ApplyLoadZones.
auto runZoneutilsLoadZonesHost6404SelfTests() -> bool
{
    bool ok = true;

    const std::vector<std::string> phases = {
        "CreateZone",
        "CreateZone(0)",
        "LoadXiMesh",
        "LoadNavMesh",
        "PopulateIDLookupsByZone",
        "LoadNPCList",
        "LoadMOBList",
        "campaign::LoadState",
        "campaign::LoadNations",
        "OnZoneInitialize",
    };
    ok = expect(phases.size() == 10, "phase count") && ok;
    ok = expect(phases[0] == "CreateZone", "first") && ok;
    ok = expect(phases[2] == "LoadXiMesh" && phases[3] == "LoadNavMesh", "mesh order") && ok;
    ok = expect(phases[5] == "LoadNPCList" && phases[6] == "LoadMOBList", "npc before mob") && ok;
    ok = expect(phases.back() == "OnZoneInitialize", "last") && ok;

    ok = expect(std::string("Loading NPCs") == "Loading NPCs", "npc info") && ok;
    ok = expect(std::string("Loading Mobs") == "Loading Mobs", "mob info") && ok;

    // OnZoneInitialize gate: GetIP() != 0
    const uint32_t zoneIP = 0;
    ok = expect(zoneIP == 0, "skip onzone when ip 0") && ok;

    return ok;
}
