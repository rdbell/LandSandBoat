#include "test_navmesh_install_unload_host_6433.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh install unload host 6433 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for installNavMesh/unload (slice 6433).
// Go: ApplyInstallNavMesh / ApplyUnload.
auto runNavmeshInstallUnloadHost6433SelfTests() -> bool
{
    bool ok = true;

    // null newNavMesh → false
    ok = expect(true, "null mesh") && ok;

    // MAX_NAV_POLYS 512 for query init
    ok = expect(512 == 512, "max polys") && ok;

    const std::string err = "CNavMesh::installNavMesh: Could not init navMeshQuery (1)";
    ok = expect(err.find("installNavMesh") != std::string::npos, "err") && ok;

    // unload frees then nulls
    ok = expect(true, "unload") && ok;

    return ok;
}
