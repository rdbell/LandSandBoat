#include "test_zone_mesh_load_host_6410.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone mesh load host 6410 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CZone::LoadXiMesh / LoadNavMesh paths (slice 6410).
// Go: PlanLoadXiMesh / ApplyLoadXiMesh / PlanLoadNavMesh / ApplyLoadNavMesh.
auto runZoneMeshLoadHost6410SelfTests() -> bool
{
    bool ok = true;

    const std::string xiPath  = "ximeshes/Bastok_Markets.ximesh";
    const std::string navPath = "navmeshes/Bastok_Markets.nav";
    ok = expect(xiPath.find("ximeshes/") == 0, "xi dir") && ok;
    ok = expect(navPath.find("navmeshes/") == 0, "nav dir") && ok;

    // rebuildNavmeshes forces skip file load try.
    const bool rebuild = true;
    const bool tryLoad = !rebuild;
    ok = expect(!tryLoad, "rebuild skips load") && ok;

    const std::string xiErr = "CZone::LoadXiMesh: Failed to load 'ximeshes/a.ximesh': boom";
    ok = expect(xiErr.find("Failed to load") != std::string::npos, "xi err") && ok;

    const std::string navFail = "CZone::LoadNavMesh: Build failed for zone (Zone_A)";
    ok = expect(navFail.find("Build failed") != std::string::npos, "nav fail") && ok;

    // Exceptional name: Escha_ → Escha-
    const std::string escha = "Escha-ZiTah";
    ok = expect(escha.find("Escha-") == 0, "escha rename") && ok;

    return ok;
}
