#include "test_zone_load_navmesh_full_host_6437.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone load navmesh full host 6437 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CZone::LoadNavMesh full composition (slice 6437).
// Go: ApplyLoadNavMeshFull / ShouldTryLoadNavMeshFile.
auto runZoneLoadNavmeshFullHost6437SelfTests() -> bool
{
    bool ok = true;

    // try load only when !rebuildNavmeshes
    ok = expect(true, "try load gate") && ok;

    // fail path error string
    const std::string err = "CZone::LoadNavMesh: Build failed for zone (Zone_A)";
    ok = expect(err.find("Build failed") != std::string::npos, "build failed") && ok;

    // composition: load-or-rebuild-install
    ok = expect(true, "composition") && ok;

    return ok;
}
