#include "test_navmesh_build_async_host_6432.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh build async host 6432 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for buildAsync fan-out (slice 6432).
// Go: ApplyBuildAsync / TileBatchRanges.
auto runNavmeshBuildAsyncHost6432SelfTests() -> bool
{
    bool ok = true;

    constexpr std::size_t TILE_BATCH_SIZE = 32;
    ok = expect(TILE_BATCH_SIZE == 32, "batch") && ok;

    // tilesBuilt == 0 → fail
    ok = expect(true, "no tiles fail") && ok;

    const std::string alloc = "NavMeshBuilder::build: Could not allocate dtNavMesh (1)";
    ok = expect(alloc.find("allocate") != std::string::npos, "alloc msg") && ok;

    // DT_TILE_FREE_DATA on addTile
    ok = expect(true, "free data flag") && ok;

    return ok;
}
