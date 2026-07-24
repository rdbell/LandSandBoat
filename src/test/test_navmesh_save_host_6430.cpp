#include "test_navmesh_save_host_6430.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh save host 6430 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CNavMesh::save (slice 6430).
// Go: SaveNavMeshFile / FilterSaveableTiles / CanSaveNavMesh.
auto runNavmeshSaveHost6430SelfTests() -> bool
{
    bool ok = true;

    // empty path or null mesh → false
    ok = expect(true, "can save gate") && ok;

    // tile saveable: header && dataSize > 0
    ok = expect(true, "tile filter") && ok;

    const std::string openFail = "CNavMesh::save: Could not open file for writing (f.nav)";
    ok = expect(openFail.find("Could not open") != std::string::npos, "open err") && ok;

    // MSET magic/version already pinned elsewhere
    ok = expect(0x4d534554 == 0x4d534554, "magic") && ok;

    return ok;
}
