/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_treasure_check_items_6958.h"

#include "common/timer.h"

#include <iostream>

#define private public
#include "map/treasure_pool.h"
#undef private

namespace
{

using namespace std::chrono_literals;

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure checkItems 6958 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::checkItems characterization (slice 6958). The pool
// item IDs remain empty so the due loop has no item/inventory dependencies;
// observing m_Tick pins the actual public method's empty, strict-time, and
// successful-loop behavior.
auto runTreasureCheckItems6958SelfTests() -> bool
{
    bool ok = true;

    CTreasurePool pool(TreasurePoolType::Solo);

    pool.m_Tick = timer::time_point{} + 1s;
    pool.checkItems(timer::time_point{} + 10s);
    ok = expect(pool.m_Tick == timer::time_point{} + 1s, "empty pool preserves tick") && ok;

    pool.m_count = 1;
    pool.m_Tick  = timer::time_point{};
    pool.checkItems(timer::time_point{} + 3s);
    ok = expect(pool.m_Tick == timer::time_point{}, "exactly three seconds is not due") && ok;

    const auto dueTick = timer::time_point{} + 3s + 1ns;
    pool.checkItems(dueTick);
    ok = expect(pool.m_Tick == dueTick, "nonempty due pool advances tick after all slots") && ok;

    pool.checkItems(dueTick + 3s);
    ok = expect(pool.m_Tick == dueTick, "next exact three seconds is not due") && ok;

    return ok;
}
