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

#include "test_ximesh_math.h"

#include "map/ximesh/transformation_matrix.h"
#include "map/ximesh/vector3.h"

#include <cmath>
#include <cstddef>
#include <iostream>

namespace
{

auto nearlyEqual(float actual, float expected) -> bool
{
    return std::fabs(actual - expected) <= 0.00001f;
}

auto expectNear(float actual, float expected, const char* label) -> bool
{
    if (!nearlyEqual(actual, expected))
    {
        std::cerr << "ximesh math self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectVector(const Vector3& actual, const Vector3& expected, const char* label) -> bool
{
    bool ok = true;
    ok      = expectNear(actual.x, expected.x, label) && ok;
    ok      = expectNear(actual.y, expected.y, label) && ok;
    ok      = expectNear(actual.z, expected.z, label) && ok;
    return ok;
}

auto expectMatrix(const TransformationMatrix& actual, const TransformationMatrix& expected, const char* label) -> bool
{
    bool ok = true;
    for (std::size_t row = 0; row < 4; ++row)
    {
        for (std::size_t column = 0; column < 3; ++column)
        {
            ok = expectNear(actual.elements[row][column], expected.elements[row][column], label) && ok;
        }
    }
    return ok;
}

auto testVectorArithmetic() -> bool
{
    const auto a = Vector3{ 1.0f, 2.0f, 3.0f };
    const auto b = Vector3{ -4.0f, 5.0f, -6.0f };

    bool ok = true;
    ok      = expectVector(a + b, Vector3{ -3.0f, 7.0f, -3.0f }, "vector addition") && ok;
    ok      = expectVector(a - b, Vector3{ 5.0f, -3.0f, 9.0f }, "vector subtraction") && ok;
    ok      = expectVector(a * 2.5f, Vector3{ 2.5f, 5.0f, 7.5f }, "vector multiply") && ok;
    ok      = expectVector(b / -2.0f, Vector3{ 2.0f, -2.5f, 3.0f }, "vector divide") && ok;
    ok      = expectVector(a.crossProduct(b), Vector3{ -27.0f, -6.0f, 13.0f }, "cross product") && ok;
    ok      = expectNear(a.dotProduct(b), -12.0f, "dot product") && ok;
    ok      = expectNear(a.magnitudeSquared(), 14.0f, "magnitude squared") && ok;
    ok      = expectNear(a.magnitude(), std::sqrt(14.0f), "magnitude") && ok;

    auto mutableVector = a;
    mutableVector += b;
    ok = expectVector(mutableVector, Vector3{ -3.0f, 7.0f, -3.0f }, "plus assign") && ok;
    mutableVector -= b;
    ok = expectVector(mutableVector, a, "minus assign") && ok;
    mutableVector *= 3.0f;
    ok = expectVector(mutableVector, Vector3{ 3.0f, 6.0f, 9.0f }, "multiply assign") && ok;
    mutableVector /= 3.0f;
    ok = expectVector(mutableVector, a, "divide assign") && ok;
    return ok;
}

auto testTransformationMatrix() -> bool
{
    const auto matrix = TransformationMatrix{
        {
            { 2.0f, 0.0f, 0.0f },
            { 0.0f, 3.0f, 0.0f },
            { 0.0f, 0.0f, 4.0f },
            { 5.0f, 6.0f, 7.0f },
        },
    };
    const auto point = Vector3{ 1.0f, 2.0f, 3.0f };

    bool ok = true;
    ok      = expectNear(matrix.determinant(), 24.0f, "determinant") && ok;
    ok      = expectVector(matrix.applyToCopy(point), Vector3{ 7.0f, 12.0f, 19.0f }, "apply copy") && ok;
    ok      = expectNear(matrix.applyGetY(point), 12.0f, "apply get y") && ok;

    auto mutablePoint = point;
    matrix.applyTo(mutablePoint);
    ok = expectVector(mutablePoint, Vector3{ 7.0f, 12.0f, 19.0f }, "apply mutable") && ok;

    const auto inverted = matrix.getInverted();
    ok                  = expectNear(inverted.determinant(), 1.0f / 24.0f, "inverse determinant") && ok;
    ok                  = expectVector(inverted.applyToCopy(matrix.applyToCopy(point)), point, "inverse round trip") && ok;
    ok                  = expectVector(matrix.applyToCopy(inverted.applyToCopy(point)), point, "forward round trip") && ok;
    return ok;
}

auto testTransformationMatrixNonDiagonal() -> bool
{
    const auto matrix = TransformationMatrix{
        {
            { 1.0f, 2.0f, 0.0f },
            { 0.0f, 1.0f, 3.0f },
            { 4.0f, 0.0f, 1.0f },
            { -2.0f, 5.0f, 7.0f },
        },
    };
    const auto point = Vector3{ 2.0f, -1.0f, 0.5f };

    bool ok = true;
    ok      = expectNear(matrix.determinant(), 25.0f, "non-diagonal determinant") && ok;
    ok      = expectVector(matrix.applyToCopy(point), Vector3{ 2.0f, 8.0f, 4.5f }, "non-diagonal apply copy") && ok;
    ok      = expectNear(matrix.applyGetY(point), 8.0f, "non-diagonal apply get y") && ok;

    const auto inverted = matrix.getInverted();
    const auto expected = TransformationMatrix{
        {
            { 0.04f, -0.08f, 0.24f },
            { 0.48f, 0.04f, -0.12f },
            { -0.16f, 0.32f, 0.04f },
            { -1.2f, -2.6f, 0.8f },
        },
    };
    ok = expectMatrix(inverted, expected, "non-diagonal inverse") && ok;
    ok = expectNear(inverted.determinant(), 0.04f, "non-diagonal inverse determinant") && ok;
    ok = expectVector(inverted.applyToCopy(matrix.applyToCopy(point)), point, "non-diagonal inverse round trip") && ok;
    ok = expectVector(matrix.applyToCopy(inverted.applyToCopy(point)), point, "non-diagonal forward round trip") && ok;
    return ok;
}

} // namespace

auto runXimeshMathSelfTests() -> bool
{
    bool ok = true;
    ok      = testVectorArithmetic() && ok;
    ok      = testTransformationMatrix() && ok;
    ok      = testTransformationMatrixNonDiagonal() && ok;
    return ok;
}
