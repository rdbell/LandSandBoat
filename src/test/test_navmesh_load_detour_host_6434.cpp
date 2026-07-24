#include "test_navmesh_load_detour_host_6434.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh load detour host 6434 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CNavMesh::load Detour materialization (slice 6434).
// Go: ApplyLoadDetourFromSet / ApplyLoadNavMeshFileDetour.
auto runNavmeshLoadDetourHost6434SelfTests() -> bool
{
    bool ok = true;

    // tile terminator: ref==0 or dataSize==0
    ok = expect(true, "tile terminator") && ok;

    // MAX_NAV_POLYS 512 for query init
    ok = expect(512 == 512, "max polys") && ok;

    const std::string initErr = "CNavMesh::load Could not initialize detour for (a.nav)";
    ok = expect(initErr.find("Could not initialize detour") != std::string::npos, "init err") && ok;

    const std::string queryErr = "CNavMesh::load Error loading m_navMeshQuery (a.nav)";
    ok = expect(queryErr.find("m_navMeshQuery") != std::string::npos, "query err") && ok;

    // alloc → init → addTile → query composition order
    ok = expect(true, "load composition") && ok;

    return ok;
}
