#include "test_char_death_time_save_plan_6912.h"

#include "map/char_death_time_save_plan.h"

#include <iostream>
#include <limits>

auto runCharDeathTimeSavePlan6912SelfTests() -> bool
{
    using deathtimesavehelpers::MakePlan;
    using deathtimesavehelpers::Plan;

    const bool ok = MakePlan(2) == Plan{ true, 2 } &&
                    MakePlan(-1) == Plan{ true, std::numeric_limits<uint32>::max() };

    if (!ok)
    {
        std::cerr << "death time save plan 6912 self-test failed\n";
    }

    return ok;
}
