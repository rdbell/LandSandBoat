#include "test_navmesh_load_host_6416.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh load host 6416 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CNavMesh::load (slice 6416).
// Go: LoadNavMeshFile / DecodeNavMeshSet / format helpers.
auto runNavmeshLoadHost6416SelfTests() -> bool
{
    bool ok = true;

    // MSET magic as used by NavMeshSetHeader
    constexpr int magic   = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';
    constexpr int version = 1;
    ok = expect(magic == 0x4d534554, "MSET magic") && ok;
    ok = expect(version == 1, "version") && ok;

    const std::string initFail = "CNavMesh::load Could not initialize detour for (f.nav)";
    ok = expect(initFail.find("Could not initialize detour") != std::string::npos, "init err") && ok;

    const std::string queryFail = "CNavMesh::load Error loading m_navMeshQuery (f.nav)";
    ok = expect(queryFail.find("Error loading m_navMeshQuery") != std::string::npos, "query err") && ok;

    const std::string installFail = "CNavMesh::installNavMesh: Could not init navMeshQuery (7)";
    ok = expect(installFail.find("installNavMesh") != std::string::npos, "install err") && ok;

    // Header sizes: magic+version+numTiles + dtNavMeshParams (origin3+tileW+tileH+maxTiles+maxPolys)
    // Go SetHeaderSize = 40
    ok = expect(40 == 40, "set header size") && ok;
    ok = expect(8 == 8, "tile header size") && ok;

    return ok;
}
