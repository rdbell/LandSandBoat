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

#include "test_los_cache.h"

#include "common/timer.h"
#include "map/los_cache.h"

#include <chrono>
#include <iostream>

namespace
{

using namespace std::chrono_literals;

const auto baseTime = timer::time_point{} + 1000s;
const auto src      = Vector3{ 3.0f, 4.0f, 5.0f };
const auto dst      = Vector3{ 9.0f, 10.0f, 11.0f };

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "LOS cache self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectMiss(LineOfSightCache& cache, const Vector3& srcPos, const Vector3& dstPos, uint16 zone, timer::time_point now, const char* label) -> bool
{
    const auto result = cache.get(srcPos, dstPos, zone, now);
    return expectBool(result.has_value(), false, label);
}

auto expectHit(LineOfSightCache& cache, const Vector3& srcPos, const Vector3& dstPos, uint16 zone, timer::time_point now, bool expected, const char* label) -> bool
{
    const auto result = cache.get(srcPos, dstPos, zone, now);
    bool       ok     = true;
    ok                = expectBool(result.has_value(), true, label) && ok;
    if (result.has_value())
    {
        ok = expectBool(*result, expected, label) && ok;
    }
    return ok;
}

auto testLOSCacheEmptyHitExpiryAndUpdate() -> bool
{
    LineOfSightCache cache;

    bool ok = true;
    ok      = expectMiss(cache, src, dst, 42, baseTime, "empty cache miss") && ok;

    cache.put(src, dst, 42, true, baseTime);
    ok = expectHit(cache, src, dst, 42, baseTime + 2499ms, true, "hit before expiry") && ok;
    ok = expectMiss(cache, src, dst, 42, baseTime + 2500ms, "miss at expiry") && ok;

    cache.put(src, dst, 42, false, baseTime + 1s);
    ok = expectHit(cache, src, dst, 42, baseTime + 1s, false, "updated entry result") && ok;
    return ok;
}

auto testLOSCacheQuantizedDirectionalZoneKeys() -> bool
{
    LineOfSightCache cache;
    const auto       srcInCell       = Vector3{ 3.9f, -1.9f, 0.1f };
    const auto       srcSameCell     = Vector3{ 2.1f, -0.1f, 1.9f };
    const auto       srcDifferent    = Vector3{ 4.1f, -2.1f, 0.1f };
    const auto       dstInCell       = Vector3{ 7.9f, 8.1f, -1.9f };
    const auto       dstSameCell     = Vector3{ 6.1f, 9.9f, -0.1f };

    cache.put(srcInCell, dstInCell, 77, true, baseTime);

    bool ok = true;
    ok      = expectHit(cache, srcSameCell, dstSameCell, 77, baseTime, true, "same quantized cells hit") && ok;
    ok      = expectMiss(cache, srcDifferent, dstSameCell, 77, baseTime, "different source cell miss") && ok;
    ok      = expectMiss(cache, srcSameCell, dstSameCell, 78, baseTime, "different zone miss") && ok;
    ok      = expectMiss(cache, dstSameCell, srcSameCell, 77, baseTime, "reversed direction miss") && ok;
    return ok;
}

auto testLOSCacheLRU() -> bool
{
    LineOfSightCache cache;

    for (uint16 zone = 1; zone <= 8; ++zone)
    {
        cache.put(src, dst, zone, zone % 2 == 0, baseTime);
    }

    bool ok = true;
    ok      = expectHit(cache, src, dst, 1, baseTime, false, "refresh oldest entry to MRU") && ok;

    cache.put(src, dst, 9, true, baseTime);

    ok = expectHit(cache, src, dst, 1, baseTime, false, "refreshed entry retained") && ok;
    ok = expectMiss(cache, src, dst, 2, baseTime, "least recently used entry evicted") && ok;
    ok = expectHit(cache, src, dst, 9, baseTime, true, "new entry retained") && ok;
    return ok;
}

auto testLOSCacheUpdatePromotionAndCellBoundaries() -> bool
{
    LineOfSightCache cache;

    for (uint16 zone = 1; zone <= 8; ++zone)
    {
        cache.put(src, dst, zone, true, baseTime);
    }

    // Updating the LRU entry refreshes both its value and expiry, and promotes
    // it before the next insertion chooses an eviction victim.
    cache.put(src, dst, 1, false, baseTime + 1s);
    cache.put(src, dst, 9, true, baseTime + 1s);

    bool ok = true;
    ok      = expectHit(cache, src, dst, 1, baseTime + 3499ms, false, "updated LRU entry retained and refreshed") && ok;
    ok      = expectMiss(cache, src, dst, 2, baseTime + 1s, "update promotion evicts previous successor") && ok;

    const auto boundarySrc = Vector3{ 2.0f, -2.0f, 0.0f };
    const auto insideSrc   = Vector3{ 1.999f, -1.999f, 0.0f };
    const auto sameCell    = Vector3{ 3.999f, -3.999f, 1.999f };
    cache.put(boundarySrc, dst, 10, true, baseTime);

    ok = expectHit(cache, sameCell, dst, 10, baseTime, true, "exact positive and negative cell boundaries") && ok;
    ok = expectMiss(cache, insideSrc, dst, 10, baseTime, "values inside zero cell differ from exact boundaries") && ok;
    return ok;
}

} // namespace

auto runLOSCacheSelfTests() -> bool
{
    bool ok = true;
    ok      = testLOSCacheEmptyHitExpiryAndUpdate() && ok;
    ok      = testLOSCacheQuantizedDirectionalZoneKeys() && ok;
    ok      = testLOSCacheLRU() && ok;
    ok      = testLOSCacheUpdatePromotionAndCellBoundaries() && ok;
    return ok;
}
