#include "test_targetfind_cone_3925.h"

#include "map/ai/helpers/targetfind_cone_capacity.h"

#include <iostream>

auto runTargetfindCone3925SelfTests() -> bool
{
    using targetfindconehelpers::IsWithinTriangle;
    const position_t origin{};
    const position_t b{ 1.0f, 0.0f, 0.0f, 0, 0 };
    const position_t c{ 0.0f, 0.0f, 1.0f, 0, 0 };
    const bool ok = IsWithinTriangle(origin, b, c, 1.0f, position_t{ 0.2f, 0.0f, 0.2f, 0, 0 }) &&
                    IsWithinTriangle(origin, b, c, 1.0f, b) &&
                    IsWithinTriangle(origin, b, c, 1.0f, position_t{ 0.5f, 0.0f, 0.5f, 0, 0 }) &&
                    !IsWithinTriangle(origin, b, c, 1.0f, position_t{ 0.8f, 0.0f, 0.8f, 0, 0 }) &&
                    !IsWithinTriangle(origin, b, c, 1.0f, position_t{ -0.1f, 0.0f, 0.0f, 0, 0 });
    if (!ok) std::cerr << "targetfind cone 3925 self-test failed\n";
    return ok;
}
