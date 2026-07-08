/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_ximesh_structs.h"

#include "map/ximesh/ximesh_structs.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>

namespace
{

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ximesh structs self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ximesh structs self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t N>
auto expectBytes(const T& value, const std::array<std::uint8_t, N>& expected, const char* label) -> bool
{
    std::array<std::uint8_t, sizeof(T)> actual{};
    std::memcpy(actual.data(), &value, sizeof(T));

    bool ok = expectEqualUInt(actual.size(), expected.size(), label);
    for (std::size_t i = 0; i < actual.size() && i < expected.size(); ++i)
    {
        if (actual[i] != expected[i])
        {
            std::cerr << "ximesh structs self-test failed: " << label << " byte "
                      << i << " got " << static_cast<unsigned>(actual[i])
                      << " expected " << static_cast<unsigned>(expected[i]) << '\n';
            ok = false;
        }
    }
    return ok;
}

template <typename T>
auto toWord(const T& value) -> std::uint32_t
{
    std::uint32_t word = 0;
    std::memcpy(&word, &value, sizeof(T));
    return word;
}

template <typename T>
auto fromWord(std::uint32_t word) -> T
{
    T value{};
    std::memcpy(&value, &word, sizeof(T));
    return value;
}

auto testPackedStructLayouts() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(XimeshHeader), 20, "XimeshHeader size") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, gridWidth), 0, "XimeshHeader gridWidth offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, gridHeight), 2, "XimeshHeader gridHeight offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, blockSectionOffset), 4, "XimeshHeader blockSectionOffset offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, placementSectionOffset), 8, "XimeshHeader placementSectionOffset offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, blockCount), 12, "XimeshHeader blockCount offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, placementCount), 14, "XimeshHeader placementCount offset") && ok;
    ok      = expectEqualUInt(offsetof(XimeshHeader, wideSearch), 16, "XimeshHeader wideSearch offset") && ok;

    ok = expectEqualUInt(sizeof(XimeshCellHeader), 6, "XimeshCellHeader size") && ok;
    ok = expectEqualUInt(offsetof(XimeshCellHeader, reserved), 0, "XimeshCellHeader reserved offset") && ok;
    ok = expectEqualUInt(offsetof(XimeshCellHeader, entryCount), 4, "XimeshCellHeader entryCount offset") && ok;

    ok = expectEqualUInt(sizeof(XimeshCellEntry), 8, "XimeshCellEntry size") && ok;
    ok = expectEqualUInt(offsetof(XimeshCellEntry, blockOffset), 0, "XimeshCellEntry blockOffset offset") && ok;
    ok = expectEqualUInt(offsetof(XimeshCellEntry, placementOffset), 4, "XimeshCellEntry placementOffset offset") && ok;
    return ok;
}

auto testPackedStructBytes() -> bool
{
    const auto header = XimeshHeader{
        .gridWidth              = 0x0102,
        .gridHeight             = 0x0304,
        .blockSectionOffset     = 0x05060708,
        .placementSectionOffset = 0x090A0B0C,
        .blockCount             = 0x0D0E,
        .placementCount         = 0x0F10,
        .wideSearch             = 0x11121314,
    };
    const auto cellHeader = XimeshCellHeader{ .reserved = 0x01020304, .entryCount = 0x0506 };
    const auto cellEntry  = XimeshCellEntry{ .blockOffset = 0x0708090A, .placementOffset = 0x0B0C0D0E };

    bool ok = true;
    ok      = expectBytes(header,
                          std::array<std::uint8_t, 20>{
                              0x02, 0x01, 0x04, 0x03, 0x08, 0x07, 0x06, 0x05, 0x0C, 0x0B,
                              0x0A, 0x09, 0x0E, 0x0D, 0x10, 0x0F, 0x14, 0x13, 0x12, 0x11 },
                          "XimeshHeader bytes") &&
         ok;
    ok = expectBytes(cellHeader, std::array<std::uint8_t, 6>{ 0x04, 0x03, 0x02, 0x01, 0x06, 0x05 }, "XimeshCellHeader bytes") && ok;
    ok = expectBytes(cellEntry, std::array<std::uint8_t, 8>{ 0x0A, 0x09, 0x08, 0x07, 0x0E, 0x0D, 0x0C, 0x0B }, "XimeshCellEntry bytes") && ok;
    return ok;
}

auto testBitfields() -> bool
{
    auto meta     = TriangleMeta{};
    meta.material = 0x0B;
    meta.barrier  = 1;
    meta.padding  = 0x05;

    auto decodedMeta = TriangleMeta{};
    auto metaByte    = static_cast<std::uint8_t>(0xD6);
    std::memcpy(&decodedMeta, &metaByte, sizeof(decodedMeta));

    auto flags       = PlacementFlags{};
    flags.padding0   = 3;
    flags.roofed     = 1;
    flags.mapIdLow   = 5;
    flags.padding1   = 0xABCDE;
    flags.mapIdHigh  = 2;
    flags.padding2   = 9;
    auto decodedFlag = fromWord<PlacementFlags>(0x9AAF37AF);

    auto span    = CellSpan{};
    span.offset  = 0x345678;
    span.count   = 0x9A;
    auto decoded = fromWord<CellSpan>(0x9A345678);

    bool ok = true;
    ok      = expectEqualUInt(sizeof(TriangleMeta), 1, "TriangleMeta size") && ok;
    ok      = expectEqualUInt(toWord(meta) & 0xFF, 0xBB, "TriangleMeta bitfield word") && ok;
    ok      = expectEqualUInt(decodedMeta.material, 0x06, "TriangleMeta decoded material") && ok;
    ok      = expectEqualUInt(decodedMeta.barrier, 1, "TriangleMeta decoded barrier") && ok;
    ok      = expectEqualUInt(decodedMeta.padding, 0x06, "TriangleMeta decoded padding") && ok;

    ok = expectEqualUInt(sizeof(PlacementFlags), 4, "PlacementFlags size") && ok;
    ok = expectEqualUInt(toWord(flags), 0x9AAF37AF, "PlacementFlags bitfield word") && ok;
    ok = expectEqualUInt(static_cast<std::uint8_t>(flags.mapIdHigh << 3 | flags.mapIdLow), 0x15, "PlacementFlags map id") && ok;
    ok = expectEqualUInt(decodedFlag.padding0, 3, "PlacementFlags decoded padding0") && ok;
    ok = expectEqualUInt(decodedFlag.roofed, 1, "PlacementFlags decoded roofed") && ok;
    ok = expectEqualUInt(decodedFlag.mapIdLow, 5, "PlacementFlags decoded mapIdLow") && ok;
    ok = expectEqualUInt(decodedFlag.padding1, 0xABCDE, "PlacementFlags decoded padding1") && ok;
    ok = expectEqualUInt(decodedFlag.mapIdHigh, 2, "PlacementFlags decoded mapIdHigh") && ok;
    ok = expectEqualUInt(decodedFlag.padding2, 9, "PlacementFlags decoded padding2") && ok;

    ok = expectEqualUInt(sizeof(CellSpan), 4, "CellSpan size") && ok;
    ok = expectEqualUInt(toWord(span), 0x9A345678, "CellSpan bitfield word") && ok;
    ok = expectEqualUInt(decoded.offset, 0x345678, "CellSpan decoded offset") && ok;
    ok = expectEqualUInt(decoded.count, 0x9A, "CellSpan decoded count") && ok;
    return ok;
}

auto testDefaultConstructors() -> bool
{
    const auto placement = MeshPlacement{};
    const auto hit       = CellHit{};
    const auto yRange    = YRange{};
    const auto rayHit    = RayHitInfo{};

    bool ok = true;
    ok      = expectEqualFloat(placement.yMin, std::numeric_limits<float>::max(), "MeshPlacement yMin") && ok;
    ok      = expectEqualFloat(placement.yMax, std::numeric_limits<float>::lowest(), "MeshPlacement yMax") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(hit.type), static_cast<std::uint8_t>(TerrainType::None), "CellHit type") && ok;
    ok      = expectEqualFloat(yRange.min, std::numeric_limits<float>::max(), "YRange min") && ok;
    ok      = expectEqualFloat(yRange.max, std::numeric_limits<float>::lowest(), "YRange max") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(rayHit.type), static_cast<std::uint8_t>(TerrainType::None), "RayHitInfo type") && ok;
    return ok;
}

} // namespace

auto runXimeshStructsSelfTests() -> bool
{
    bool ok = true;
    ok      = testPackedStructLayouts() && ok;
    ok      = testPackedStructBytes() && ok;
    ok      = testBitfields() && ok;
    ok      = testDefaultConstructors() && ok;
    return ok;
}
