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

#include <array>
#include <cmath>
#include <iostream>
#include <string>

namespace
{

constexpr float pi = 3.1415927410125732f;

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

    return ok;
}
