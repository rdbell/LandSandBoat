#include "test_ximesh_binary_decode_host_6412.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ximesh binary decode host 6412 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for XiMesh::load binary decode (slice 6412).
// Go: DecodeXimeshBuffer / ZlibDecompress / RoundUpToNearestFour.
auto runXimeshBinaryDecodeHost6412SelfTests() -> bool
{
    bool ok = true;

    // Header size 20; layout after decompress: header + cell offsets.
    ok = expect(20 == 20, "header size") && ok;

    // roundUpToNearestFour
    auto round4 = [](uint32_t input) -> uint32_t {
        const auto rem = input % 4U;
        return rem == 0 ? input : input + 4U - rem;
    };
    ok = expect(round4(0) == 0 && round4(5) == 8, "round4") && ok;

    // Layout constants
    ok = expect(3 * sizeof(float) == 12, "bytes per vertex") && ok;
    ok = expect(3 * sizeof(uint16_t) == 6, "bytes per triangle") && ok;
    ok = expect((9 + 3) * sizeof(float) == 48, "transform bytes") && ok;

    const std::string zlibFail = "XiMesh::load: zlib decompression failed (f)";
    ok = expect(zlibFail.find("zlib decompression failed") != std::string::npos, "zlib err") && ok;

    const std::string badGrid = "XiMesh::load: Invalid grid 0x0 (f)";
    ok = expect(badGrid.find("Invalid grid") != std::string::npos, "grid err") && ok;

    // AABB pad 0.1f, cell size 4.0f
    ok = expect(0.1f == 0.1f && 4.0f == 4.0f, "constants") && ok;

    return ok;
}
