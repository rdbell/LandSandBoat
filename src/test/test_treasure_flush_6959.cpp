/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_treasure_flush_6959.h"

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
        std::cerr << "treasure flush 6959 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::flush characterization (slice 6959). Empty item IDs
// keep per-slot resolution inert; m_count selects the real flush branch, and
// m_Tick proves flush does not reuse the periodic checkItems scheduler state.
auto runTreasureFlush6959SelfTests() -> bool
{
    bool ok = true;

    CTreasurePool pool(TreasurePoolType::Solo);
    pool.m_Tick = timer::time_point{} + 7s;
    pool.flush();
    ok = expect(pool.m_Tick == timer::time_point{} + 7s, "empty pool leaves periodic tick unchanged") && ok;

    pool.m_count = 1;
    pool.flush();
    ok = expect(pool.m_Tick == timer::time_point{} + 7s, "nonempty flush leaves periodic tick unchanged") && ok;

    return ok;
}
