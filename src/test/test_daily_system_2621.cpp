#include "test_daily_system_2621.h"

#include <iostream>
#include <vector>

#include "map/daily_system.h"

auto runDailySystem2621SelfTests() -> bool
{
    const auto pool = std::vector<uint16>{ 1001, 1002, 1003 };
    const bool ok   = daily::SelectItemFromPool(pool, 0) == 1001 &&
                    daily::SelectItemFromPool(pool, 2) == 1003 &&
                    daily::SelectItemFromPool(pool, 3) == 1001 &&
                    daily::SelectItemFromPool(pool, -1) == 1001 &&
                    daily::SelectItemFromPool({}, 0) == 0;
    if (!ok)
    {
        std::cerr << "daily system self-test failed\n";
    }
    return ok;
}
