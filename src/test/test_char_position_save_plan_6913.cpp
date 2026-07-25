#include "test_char_position_save_plan_6913.h"

#include "map/char_position_save_plan.h"

#include <iostream>

auto runCharPositionSavePlan6913SelfTests() -> bool
{
    const bool ok = positionsavehelpers::MakePlan(positionsavehelpers::kStatusDisappear) == positionsavehelpers::Plan{ false } &&
                    positionsavehelpers::MakePlan(1) == positionsavehelpers::Plan{ true };

    if (!ok)
    {
        std::cerr << "position save plan 6913 self-test failed\n";
    }

    return ok;
}
