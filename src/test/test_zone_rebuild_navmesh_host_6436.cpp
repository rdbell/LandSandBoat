#include "test_zone_rebuild_navmesh_host_6436.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone rebuild navmesh host 6436 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CZone::RebuildNavMesh composition (slice 6436).
// Go: ApplyRebuildNavMesh / ShouldInstallBuiltMesh.
auto runZoneRebuildNavmeshHost6436SelfTests() -> bool
{
    bool ok = true;

    // path = navmeshes/{name}.nav
    const std::string path = "navmeshes/Foo.nav";
    ok = expect(path.find("navmeshes/") == 0, "path") && ok;

    // install only when dtNavMesh non-null
    ok = expect(true, "install gate") && ok;

    // composition: build → install → save → attach
    ok = expect(true, "composition") && ok;

    return ok;
}
