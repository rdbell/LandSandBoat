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

#include "test_spatial_grid.h"

#include "common/timer.h"
#include "map/entities/base_entity.h"
#include "map/spatial_grid.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace
{

class TestEntity final : public CBaseEntity
{
public:
    TestEntity(uint32 entityId, position_t position)
    {
        id    = entityId;
        loc.p = position;
    }

    auto Tick(timer::time_point) -> Task<void> override
    {
        co_return;
    }

    void PostTick() override
    {
    }
};

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "spatial grid self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto collectIds(const SpatialGrid& grid, position_t center, float radius) -> std::vector<uint32>
{
    std::vector<uint32> ids;
    grid.forEachInRange(center, radius, [&ids](const CBaseEntity* entity)
    {
        ids.push_back(entity->id);
    });
    std::ranges::sort(ids);
    return ids;
}

auto expectIds(const std::vector<uint32>& actual, const std::vector<uint32>& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "spatial grid self-test failed: " << label << " got";
        for (const auto id : actual)
        {
            std::cerr << ' ' << id;
        }
        std::cerr << " expected";
        for (const auto id : expected)
        {
            std::cerr << ' ' << id;
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testCellKeysAndEmptyOperations() -> bool
{
    SpatialGrid grid(10.0f);
    SpatialGrid defaultGrid;

    bool ok = true;
    ok      = expectEqual(grid.size(), static_cast<std::size_t>(0), "initial size") && ok;
    ok      = expectEqual(grid.cellCount(), static_cast<std::size_t>(0), "initial cell count") && ok;

    grid.add(nullptr);
    grid.update(nullptr);
    grid.remove(nullptr);

    ok = expectEqual(grid.size(), static_cast<std::size_t>(0), "nil operations size") && ok;
    ok = expectEqual(grid.cellKeyFor(position_t{ 0.0f, 99.0f, 0.0f, 0, 0 }), static_cast<SpatialGrid::CellKey>(0), "origin cell key ignores y") && ok;
    ok = expectEqual(grid.cellKeyFor(position_t{ 9.99f, 0.0f, 9.99f, 0, 0 }), static_cast<SpatialGrid::CellKey>(0), "positive same cell key") && ok;
    ok = expectEqual(grid.cellKeyFor(position_t{ 10.0f, 0.0f, 0.0f, 0, 0 }), static_cast<SpatialGrid::CellKey>(1ULL << 32), "positive x boundary key") && ok;
    ok = expectEqual(grid.cellKeyFor(position_t{ -0.01f, 0.0f, -0.01f, 0, 0 }), static_cast<SpatialGrid::CellKey>(0xFFFFFFFFFFFFFFFFULL), "negative key wraps coordinates") && ok;
    ok = expectEqual(defaultGrid.cellKeyFor(position_t{ 49.9f, 0.0f, 49.9f, 0, 0 }), static_cast<SpatialGrid::CellKey>(0), "default cell size key") && ok;
    ok = expectEqual(defaultGrid.cellKeyFor(position_t{ 50.0f, 0.0f, 0.0f, 0, 0 }), static_cast<SpatialGrid::CellKey>(1ULL << 32), "default cell size boundary") && ok;
    return ok;
}

auto testAddUpdateRemoveAndClear() -> bool
{
    SpatialGrid grid(10.0f);

    TestEntity first(1001, position_t{ 1.0f, 0.0f, 1.0f, 0, 0 });
    TestEntity second(1002, position_t{ 11.0f, 0.0f, 1.0f, 0, 0 });

    grid.add(&first);
    grid.add(&second);

    bool ok = true;
    ok      = expectEqual(grid.size(), static_cast<std::size_t>(2), "size after add") && ok;
    ok      = expectEqual(grid.cellCount(), static_cast<std::size_t>(2), "cell count after add") && ok;
    ok      = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 10.0f), { 1001, 1002 }, "nearby cells before move") && ok;

    first.loc.p = position_t{ 2.0f, 0.0f, 2.0f, 0, 0 };
    grid.update(&first);
    ok = expectEqual(grid.size(), static_cast<std::size_t>(2), "same-cell update size") && ok;

    first.loc.p = position_t{ 25.0f, 0.0f, 1.0f, 0, 0 };
    grid.update(&first);
    ok = expectEqual(grid.size(), static_cast<std::size_t>(2), "cross-cell update size") && ok;
    ok = expectEqual(grid.cellCount(), static_cast<std::size_t>(3), "cross-cell update keeps old bucket") && ok;
    ok = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 10.0f), { 1002 }, "nearby cells after move") && ok;

    grid.remove(&first);
    ok = expectEqual(grid.size(), static_cast<std::size_t>(1), "size after remove") && ok;
    ok = expectEqual(grid.cellCount(), static_cast<std::size_t>(3), "remove keeps empty bucket") && ok;
    ok = expectIds(collectIds(grid, position_t{ 25.0f, 0.0f, 0.0f, 0, 0 }, 0.0f), {}, "removed entity not visited") && ok;

    grid.clear();
    ok = expectEqual(grid.size(), static_cast<std::size_t>(0), "size after clear") && ok;
    ok = expectEqual(grid.cellCount(), static_cast<std::size_t>(3), "clear keeps allocated buckets") && ok;
    ok = expectIds(collectIds(grid, position_t{ 10.0f, 0.0f, 0.0f, 0, 0 }, 30.0f), {}, "clear empties buckets") && ok;
    return ok;
}

auto testUpdateUntrackedAndBroadRangeScan() -> bool
{
    SpatialGrid grid(10.0f);

    TestEntity near(2001, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 });
    TestEntity overfetched(2002, position_t{ 19.9f, 100.0f, 0.0f, 0, 0 });
    TestEntity outside(2003, position_t{ 20.1f, 0.0f, 0.0f, 0, 0 });
    TestEntity negative(2004, position_t{ -0.1f, 0.0f, -0.1f, 0, 0 });

    grid.update(&near);
    grid.add(&overfetched);
    grid.add(&outside);
    grid.add(&negative);

    bool ok = true;
    ok      = expectEqual(grid.size(), static_cast<std::size_t>(4), "update untracked files entity") && ok;
    ok      = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 10.0f), { 2001, 2002, 2004 }, "range scans cells, not precise distance") && ok;
    ok      = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 0.0f), { 2001 }, "zero radius scans current cell only") && ok;
    return ok;
}

auto testDuplicateAddBehavior() -> bool
{
    SpatialGrid grid(10.0f);

    TestEntity entity(4001, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 });

    grid.add(&entity);
    grid.add(&entity);

    bool ok = true;
    ok      = expectEqual(grid.size(), static_cast<std::size_t>(2), "duplicate add size") && ok;
    ok      = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 0.0f), { 4001, 4001 }, "duplicate add visits duplicate entries") && ok;

    grid.remove(&entity);
    ok = expectEqual(grid.size(), static_cast<std::size_t>(1), "remove duplicate once size") && ok;
    ok = expectIds(collectIds(grid, position_t{ 0.0f, 0.0f, 0.0f, 0, 0 }, 0.0f), { 4001 }, "remove duplicate once leaves one entry") && ok;
    return ok;
}

} // namespace

auto runSpatialGridSelfTests() -> bool
{
    bool ok = true;
    ok      = testCellKeysAndEmptyOperations() && ok;
    ok      = testAddUpdateRemoveAndClear() && ok;
    ok      = testUpdateUntrackedAndBroadRangeScan() && ok;
    ok      = testDuplicateAddBehavior() && ok;
    return ok;
}
