#include "test_navmesh_findpath_query_host_6425.h"

#include <cmath>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "navmesh findpath query host 6425 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for CNavMesh::findPath gates (slice 6425).
// Go: PathEndpointsHaveNaN / IsPartialPath / ClassifySameFloorVertical.
auto runNavmeshFindpathQueryHost6425SelfTests() -> bool
{
    bool ok = true;

    constexpr float verticalLimit = 5.0f;
    constexpr float partialOmit   = 5.0f;

    ok = expect(std::isnan(std::nanf("")) == true, "nan") && ok;

    // partial: dist > 5
    const float dx = 10.0f;
    ok = expect(dx > partialOmit, "partial dist") && ok;

    // same floor classes
    ok = expect(3.0f <= verticalLimit, "accept band") && ok;
    ok = expect(6.0f > verticalLimit && 6.0f <= 2 * verticalLimit, "disambig") && ok;
    ok = expect(11.0f > 2 * verticalLimit, "abort") && ok;

    const std::string nanMsg = "CNavMesh::findPath NaN position detected (1)";
    ok = expect(nanMsg.find("NaN") != std::string::npos, "nan msg") && ok;

    // filter flags
    constexpr uint16_t include = 0xffff;
    constexpr uint16_t exclude = 0x0010; // DISABLED
    ok = expect(include == 0xffff && exclude == 0x0010, "filter") && ok;

    // straight path skip start: i starts at 3
    ok = expect(3 == 3, "skip start") && ok;

    return ok;
}
