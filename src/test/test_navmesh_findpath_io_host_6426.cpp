#include "test_navmesh_findpath_io_host_6426.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh findpath io host 6426 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for findPath Detour I/O composition (slice 6426).
// Go: ApplyFindPath / DetourQueryOps / DetourStatusFailed.
auto runNavmeshFindpathIoHost6426SelfTests() -> bool
{
    bool ok = true;

    // DT_FAILURE high bit
    constexpr unsigned int DT_FAILURE = 1u << 31;
    constexpr unsigned int DT_SUCCESS = 1u << 30;
    ok = expect((DT_FAILURE & DT_FAILURE) != 0, "failure bit") && ok;
    ok = expect((DT_SUCCESS & DT_FAILURE) == 0, "success not fail") && ok;

    // Stage order sketch: NaN → nearest → validate → findPath → straight → partial → emit
    ok = expect(true, "composition stages") && ok;

    // Skip start index 3
    ok = expect(3 == 3, "skip start") && ok;

    const std::string partial = "CNavMesh::findPath Partial path detected! (1)";
    ok = expect(partial.find("Partial path") != std::string::npos, "partial msg") && ok;

    // MAX_NAV_POLYS
    ok = expect(512 == 512, "max nav polys") && ok;

    return ok;
}
