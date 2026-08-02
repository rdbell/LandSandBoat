#include "test_targetfind_radius_3924.h"

#include "map/ai/helpers/targetfind_radius_capacity.h"

#include <iostream>

auto runTargetfindRadius3924SelfTests() -> bool
{
    using targetfindradiushelpers::IsWithinRadius;

    const position_t origin{};
    const bool ok = IsWithinRadius(origin, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 0.0f) &&
                    IsWithinRadius(origin, position_t{ 3.0f, 0.0f, 4.0f, 0, 0 }, 5.0f) &&
                    IsWithinRadius(origin, position_t{ 0.0f, 3.0f, 4.0f, 0, 0 }, 5.0f) &&
                    !IsWithinRadius(origin, position_t{ 3.0f, 0.0f, 4.0f, 0, 0 }, 4.9f) &&
                    !IsWithinRadius(origin, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, -1.0f) &&
                    !IsWithinRadius(origin,
                                    position_t{ 6179.25146484375f, -5063.8037109375f, 2342.068603515625f, 0, 0 },
                                    8325.294921875f);
    if (!ok)
    {
        std::cerr << "targetfind radius 3924 self-test failed\n";
    }
    return ok;
}
