/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_spatial_orientation.h"

#include "common/utils.h"
#include "map/navmesh/inavmesh.h"
#include "map/navmesh/navmesh.h"
#include "map/navmesh/navmesh_config.h"

#include <DetourNavMesh.h>

#include <array>
#include <cstddef>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

constexpr float pi = 3.1415927410125732f;

// Mirrors the anonymous serialization structs in navmesh.cpp. LSB writes these
// native objects directly, so their ABI is part of the MSET file format.
struct NativeNavMeshSetHeader
{
    int             magic;
    int             version;
    int             numTiles;
    dtNavMeshParams params;
};

struct NativeNavMeshTileHeader
{
    dtTileRef tileRef;
    int       dataSize;
};

static_assert(sizeof(int) == 4);
static_assert(sizeof(float) == 4);
static_assert(sizeof(dtTileRef) == 4); // RECASTNAVIGATION_DT_POLYREF64 is OFF.
static_assert(sizeof(dtNavMeshParams) == 28);
static_assert(offsetof(dtNavMeshParams, orig) == 0);
static_assert(offsetof(dtNavMeshParams, tileWidth) == 12);
static_assert(offsetof(dtNavMeshParams, tileHeight) == 16);
static_assert(offsetof(dtNavMeshParams, maxTiles) == 20);
static_assert(offsetof(dtNavMeshParams, maxPolys) == 24);
static_assert(sizeof(NativeNavMeshSetHeader) == 40);
static_assert(offsetof(NativeNavMeshSetHeader, params) == 12);
static_assert(sizeof(NativeNavMeshTileHeader) == 8);
static_assert(offsetof(NativeNavMeshTileHeader, dataSize) == 4);

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "spatial orientation self-test failed: " << label << " expected " << static_cast<long long>(expected)
                  << ", got " << static_cast<long long>(actual) << '\n';
        return false;
    }

    return true;
}

auto expectNear(float actual, float expected, float epsilon, const std::string& label) -> bool
{
    if (std::fabs(actual - expected) > epsilon)
    {
        std::cerr << "spatial orientation self-test failed: " << label << " expected " << expected << ", got " << actual << '\n';
        return false;
    }

    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "spatial orientation self-test failed: " << label << " expected \"" << expected << "\", got \"" << actual << "\"\n";
        return false;
    }

    return true;
}

auto expectPosition(const position_t& actual, const position_t& expected, const std::string& label) -> bool
{
    bool ok = true;

    ok = expectNear(actual.x, expected.x, 0.00001f, label + " x") && ok;
    ok = expectNear(actual.y, expected.y, 0.00001f, label + " y") && ok;
    ok = expectNear(actual.z, expected.z, 0.00001f, label + " z") && ok;
    ok = expectEqual(actual.moving, expected.moving, label + " moving") && ok;
    ok = expectEqual(actual.rotation, expected.rotation, label + " rotation") && ok;

    return ok;
}

struct RelationCase
{
    std::string label;
    position_t  a;
    position_t  b;
    float       distSquared;
    float       flatDistSquared;
    float       distance;
    bool        within5;
    bool        within5Flat;
    uint8       worldAngleValue;
    int16       facingAngleValue;
    bool        facingValue;
    bool        infrontValue;
    bool        behindValue;
    bool        besideValue;
    bool        leftValue;
    bool        rightValue;
};

auto runRelationCase(const RelationCase& testCase, uint8 facingCone, uint8 relationCone) -> bool
{
    bool ok = true;

    ok = expectNear(distanceSquared(testCase.a, testCase.b, false), testCase.distSquared, 0.00001f, testCase.label + " distanceSquared") && ok;
    ok = expectNear(distanceSquared(testCase.a, testCase.b, true), testCase.flatDistSquared, 0.00001f, testCase.label + " flat distanceSquared") && ok;
    ok = expectNear(distance(testCase.a, testCase.b, false), testCase.distance, 0.00001f, testCase.label + " distance") && ok;
    ok = expectEqual(isWithinDistance(testCase.a, testCase.b, 5.0f, false), testCase.within5, testCase.label + " within") && ok;
    ok = expectEqual(isWithinDistance(testCase.a, testCase.b, 5.0f, true), testCase.within5Flat, testCase.label + " flat within") && ok;
    ok = expectEqual(worldAngle(testCase.a, testCase.b), testCase.worldAngleValue, testCase.label + " worldAngle") && ok;
    ok = expectEqual(facingAngle(testCase.a, testCase.b), testCase.facingAngleValue, testCase.label + " facingAngle") && ok;
    ok = expectEqual(facing(testCase.a, testCase.b, facingCone), testCase.facingValue, testCase.label + " facing") && ok;
    ok = expectEqual(infront(testCase.a, testCase.b, relationCone), testCase.infrontValue, testCase.label + " infront") && ok;
    ok = expectEqual(behind(testCase.a, testCase.b, relationCone), testCase.behindValue, testCase.label + " behind") && ok;
    ok = expectEqual(beside(testCase.a, testCase.b, relationCone), testCase.besideValue, testCase.label + " beside") && ok;
    ok = expectEqual(toEntitysLeft(testCase.a, testCase.b, relationCone), testCase.leftValue, testCase.label + " left") && ok;
    ok = expectEqual(toEntitysRight(testCase.a, testCase.b, relationCone), testCase.rightValue, testCase.label + " right") && ok;

    return ok;
}

} // namespace

auto runSpatialOrientationSelfTests() -> bool
{
    bool ok = true;

    const auto nativeHeader = NativeNavMeshSetHeader{
        .magic   = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T',
        .version = 1,
        .numTiles = 2,
        .params = {
            .orig = { 1.0f, -2.0f, 3.5f },
            .tileWidth = 4.0f,
            .tileHeight = 5.5f,
            .maxTiles = 6,
            .maxPolys = 7,
        },
    };
    auto nativeHeaderBytes = std::array<uint8, sizeof(nativeHeader)>{};
    std::memcpy(nativeHeaderBytes.data(), &nativeHeader, sizeof(nativeHeader));
    constexpr auto expectedHeaderBytes = std::array<uint8, 40>{
        0x54, 0x45, 0x53, 0x4D, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x60, 0x40,
        0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0xB0, 0x40, 0x06, 0x00, 0x00, 0x00,
        0x07, 0x00, 0x00, 0x00,
    };
    for (size_t i = 0; i < expectedHeaderBytes.size(); ++i)
    {
        ok = expectEqual(nativeHeaderBytes[i], expectedHeaderBytes[i], "navmesh native MSET header byte " + std::to_string(i)) && ok;
    }

    constexpr uint8 facingCone   = 90;
    constexpr uint8 relationCone = 64;

    ok = expectNear(rotationToRadian(0), 0.0f, 0.0f, "rotationToRadian 0") && ok;
    ok = expectNear(rotationToRadian(64), 1.570796371f, 0.000001f, "rotationToRadian 64") && ok;
    ok = expectNear(rotationToRadian(128), 3.141592741f, 0.000001f, "rotationToRadian 128") && ok;
    ok = expectNear(rotationToRadian(255), 6.258641720f, 0.000001f, "rotationToRadian 255") && ok;

    ok = expectEqual(radianToRotation(0.0f), 0, "radianToRotation 0") && ok;
    ok = expectEqual(radianToRotation(pi / 2.0f), 64, "radianToRotation pi/2") && ok;
    ok = expectEqual(radianToRotation(pi), 128, "radianToRotation pi") && ok;
    ok = expectEqual(relativeAngle(10, -20), 246, "relativeAngle negative") && ok;
    ok = expectEqual(relativeAngle(250, 20), 14, "relativeAngle wrap") && ok;
    ok = expectEqual(relativeAngle(0, -1), 255, "relativeAngle minus one") && ok;

    ok = expectEqual(angleDifference(10, 250), 16, "angleDifference positive wrap") && ok;
    ok = expectEqual(angleDifference(250, 10), -16, "angleDifference negative wrap") && ok;
    ok = expectEqual(angleDifference(128, 0), 128, "angleDifference positive half") && ok;
    ok = expectEqual(angleDifference(0, 128), -128, "angleDifference negative half") && ok;

    const auto relationCases = std::array<RelationCase, 7>{ {
        { "origin east", position_t(), position_t(5.0f, 0.0f, 0.0f, 0, 0), 25.0f, 25.0f, 5.0f, true, true, 0, 0, true, false, true, false, false, false },
        { "origin west", position_t(), position_t(-5.0f, 0.0f, 0.0f, 0, 0), 25.0f, 25.0f, 5.0f, true, true, 129, -127, false, true, false, false, false, false },
        { "origin north", position_t(), position_t(0.0f, 0.0f, 5.0f, 0, 0), 25.0f, 25.0f, 5.0f, true, true, 192, -64, false, false, false, true, false, true },
        { "origin south", position_t(), position_t(0.0f, 0.0f, -5.0f, 0, 0), 25.0f, 25.0f, 5.0f, true, true, 64, 64, false, false, false, true, true, false },
        { "origin diagonal", position_t(), position_t(3.0f, 4.0f, 4.0f, 0, 0), 41.0f, 25.0f, 6.403124332f, false, true, 219, -37, true, false, false, true, false, true },
        { "origin close flat", position_t(), position_t(0.05f, 99.0f, 0.05f, 0, 0), 9801.005859375f, 0.005f, 99.000030518f, false, true, 0, 0, true, true, false, false, false, false },
        { "rot north to east", position_t(0.0f, 0.0f, 0.0f, 0, 192), position_t(5.0f, 0.0f, 0.0f, 0, 0), 25.0f, 25.0f, 5.0f, true, true, 0, 64, false, false, true, false, false, false },
    } };

    for (const auto& testCase : relationCases)
    {
        ok = runRelationCase(testCase, facingCone, relationCone) && ok;
    }

    const auto nearOrigin = position_t(10.0f, 2.0f, -3.0f, 12, 0);
    ok = expectPosition(nearPosition(nearOrigin, 2.5f, 0.0f), position_t(12.5f, 2.0f, -2.999999523f, 12, 0), "near forward") && ok;
    ok = expectPosition(nearPosition(nearOrigin, 2.5f, pi), position_t(7.5f, 2.0f, -2.999999523f, 12, 0), "near behind") && ok;
    ok = expectPosition(nearPosition(position_t(1.0f, 2.0f, 1.0f, 9, 64), 2.0f, 0.0f), position_t(1.0f, 2.0f, -1.0f, 9, 64), "near rot64") && ok;

    const auto navPosition = position_t(1.25f, -2.5f, 4.75f, 19, 237);
    float      navTriple[3]{};
    CNavMesh::ToDetourPos(&navPosition, navTriple);
    ok = expectNear(navTriple[0], 1.25f, 0.0f, "navmesh position to Detour x") && ok;
    ok = expectNear(navTriple[1], 2.5f, 0.0f, "navmesh position to Detour y") && ok;
    ok = expectNear(navTriple[2], -4.75f, 0.0f, "navmesh position to Detour z") && ok;
    CNavMesh::ToFFXIPos(navTriple);
    ok = expectNear(navTriple[0], 1.25f, 0.0f, "navmesh triple round trip x") && ok;
    ok = expectNear(navTriple[1], -2.5f, 0.0f, "navmesh triple round trip y") && ok;
    ok = expectNear(navTriple[2], 4.75f, 0.0f, "navmesh triple round trip z") && ok;

    const auto detourPosition = position_t(1.25f, 2.5f, -4.75f, 19, 237);
    CNavMesh::ToFFXIPos(&detourPosition, navTriple);
    ok = expectNear(navTriple[0], 1.25f, 0.0f, "navmesh position to FFXI x") && ok;
    ok = expectNear(navTriple[1], -2.5f, 0.0f, "navmesh position to FFXI y") && ok;
    ok = expectNear(navTriple[2], 4.75f, 0.0f, "navmesh position to FFXI z") && ok;

    auto navPositionInPlace = navPosition;
    CNavMesh::ToDetourPos(&navPositionInPlace);
    CNavMesh::ToFFXIPos(&navPositionInPlace);
    ok = expectPosition(navPositionInPlace, navPosition, "navmesh position in-place round trip") && ok;

    const auto config = NavMeshConfig{};
    ok = expectNear(config.cellSize, 0.5f, 0.0f, "navmesh config cellSize") && ok;
    ok = expectNear(config.cellHeight, 0.4f, 0.0f, "navmesh config cellHeight") && ok;
    ok = expectNear(config.walkableSlopeAngle, 46.0f, 0.0f, "navmesh config walkableSlopeAngle") && ok;
    ok = expectNear(config.agentHeight, 2.0f, 0.0f, "navmesh config agentHeight") && ok;
    ok = expectNear(config.agentRadius, 0.5f, 0.0f, "navmesh config agentRadius") && ok;
    ok = expectNear(config.agentMaxClimb, 0.6f, 0.0f, "navmesh config agentMaxClimb") && ok;
    ok = expectNear(config.maxEdgeLen, 0.0f, 0.0f, "navmesh config maxEdgeLen") && ok;
    ok = expectNear(config.maxSimplificationError, 1.3f, 0.0f, "navmesh config maxSimplificationError") && ok;
    ok = expectEqual(config.minRegionArea, 8, "navmesh config minRegionArea") && ok;
    ok = expectEqual(config.mergeRegionArea, 20, "navmesh config mergeRegionArea") && ok;
    ok = expectEqual(config.maxVertsPerPoly, 6, "navmesh config maxVertsPerPoly") && ok;
    ok = expectNear(config.detailSampleDist, 6.0f, 0.0f, "navmesh config detailSampleDist") && ok;
    ok = expectNear(config.detailSampleMaxError, 1.0f, 0.0f, "navmesh config detailSampleMaxError") && ok;
    ok = expectEqual(config.tileSize, 64, "navmesh config tileSize") && ok;
    ok = expectEqual(config.filterLowHangingObstacles, true, "navmesh config low obstacles") && ok;
    ok = expectEqual(config.filterLedgeSpans, true, "navmesh config ledges") && ok;
    ok = expectEqual(config.filterWalkableLowHeightSpans, true, "navmesh config low height") && ok;

    NullNavMesh nullNavMesh;
    ok = expectEqual(nullNavMesh.findPath(navPosition, position_t()).empty(), true, "null navmesh empty path") && ok;
    const auto [randomStatus, randomPosition] = nullNavMesh.findRandomPosition(navPosition, -100.0f);
    ok = expectEqual(randomStatus, 0, "null navmesh random status") && ok;
    ok = expectPosition(randomPosition, navPosition, "null navmesh random copy") && ok;
    ok = expectEqual(nullNavMesh.raycast(navPosition, position_t()), true, "null navmesh raycast") && ok;
    ok = expectEqual(nullNavMesh.validPosition(navPosition), true, "null navmesh valid position") && ok;
    float untouched[3]{ 9.0f, 8.0f, 7.0f };
    ok = expectEqual(nullNavMesh.findClosestValidPoint(navPosition, untouched), false, "null navmesh closest") && ok;
    ok = expectEqual(nullNavMesh.findFurthestValidPoint(navPosition, position_t(), untouched), false, "null navmesh furthest") && ok;
    ok = expectNear(untouched[0], 9.0f, 0.0f, "null navmesh output untouched x") && ok;
    auto unsnapped = navPosition;
    nullNavMesh.snapToValidPosition(unsnapped);
    ok = expectPosition(unsnapped, navPosition, "null navmesh snap noop") && ok;

    ok = expectString(CNavMesh::detourStatusString(0), "", "navmesh empty Detour status") && ok;
    ok = expectString(
             CNavMesh::detourStatusString(DT_FAILURE | DT_SUCCESS | DT_IN_PROGRESS | DT_WRONG_MAGIC | DT_WRONG_VERSION |
                                          DT_OUT_OF_MEMORY | DT_INVALID_PARAM | DT_BUFFER_TOO_SMALL | DT_OUT_OF_NODES |
                                          DT_PARTIAL_RESULT | DT_ALREADY_OCCUPIED),
             "DT_FAILURE: Operation failed. DT_SUCCESS: Operation succeeded. DT_IN_PROGRESS: Operation still in progress. "
             "DT_WRONG_MAGIC: Input data is not recognized. DT_WRONG_VERSION: Input data is in wrong version. "
             "DT_OUT_OF_MEMORY: Operation ran out of memory. DT_INVALID_PARAM: An input parameter was invalid. "
             "DT_BUFFER_TOO_SMALL: Result buffer for the query was too small to store all results. "
             "DT_OUT_OF_NODES: Query ran out of nodes during search. "
             "DT_PARTIAL_RESULT: Query did not reach the end location, returning best guess. "
             "DT_ALREADY_OCCUPIED: A tile has already been assigned to the given x, y coordinate. ",
             "navmesh Detour status decomposition") && ok;

    return ok;
}
