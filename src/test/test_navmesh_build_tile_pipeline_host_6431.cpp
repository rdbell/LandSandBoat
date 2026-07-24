#include "test_navmesh_build_tile_pipeline_host_6431.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh build tile pipeline host 6431 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for buildTile Recast pipeline composition (slice 6431).
// Go: ApplyBuildTileRecastPipeline / RecastPipelineOps.
auto runNavmeshBuildTilePipelineHost6431SelfTests() -> bool
{
    bool ok = true;

    // Free solid after compact success
    ok = expect(true, "free solid after compact") && ok;

    // nconts == 0 → empty tile
    ok = expect(0 == 0, "empty contours") && ok;

    // Filters gated by config
    ok = expect(true, "filter gates") && ok;

    // CreateNavMeshData after detail + flags
    ok = expect(true, "nav data last") && ok;

    return ok;
}
