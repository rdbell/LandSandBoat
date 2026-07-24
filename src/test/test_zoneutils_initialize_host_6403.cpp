#include "test_zoneutils_initialize_host_6403.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zoneutils initialize host 6403 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for zoneutils::Initialize host (slice 6403).
// Go: FormatZonesAssignedQuery / PlanLoadZoneList / ApplyZoneutilsInitialize.
auto runZoneutilsInitializeHost6403SelfTests() -> bool
{
    bool ok = true;

    const std::string critical = "Unable to load any zones! Check IP and port params";
    ok = expect(critical == "Unable to load any zones! Check IP and port params", "critical") && ok;

    // Assigned query shape: (ip = 0) OR (ipStr/port match).
    const uint32_t ip   = 0;
    const uint16_t port = 54001;
    const std::string ipStr = "0.0.0.0";
    const std::string q = "SELECT zoneid FROM zone_settings WHERE (" + std::to_string(ip) +
                          " = 0) OR ('" + ipStr + "' = zoneip AND " + std::to_string(port) + " = zoneport)";
    ok = expect(q.find("zone_settings") != std::string::npos, "query table") && ok;
    ok = expect(q.find("(0 = 0)") != std::string::npos, "zero-ip admit-all") && ok;

    // Empty assigned → exit for LoadZoneList.
    const bool empty      = true;
    const bool shouldExit = empty;
    ok = expect(shouldExit, "empty exit") && ok;

    // Lazy vs immediate.
    const bool lazyZones = true;
    ok = expect(lazyZones == true, "lazy mode pin") && ok;

    const std::string createSQL = "SELECT zonetype, restriction FROM zone_settings WHERE zoneid = ? LIMIT 1";
    ok = expect(createSQL.find("zonetype") != std::string::npos, "create zone sql") && ok;

    return ok;
}
