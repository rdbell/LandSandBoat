#include "test_multi_hits_1551.h"

#include "map/multi_hits_capacity.h"

#include <iostream>

namespace
{
using multihitshelpers::ApplyHassoZanshinHit;
using multihitshelpers::CapMultiHits;
using multihitshelpers::ExpandMultiHits;
using multihitshelpers::HassoZanshinChance;
using multihitshelpers::ShouldRejectNullWeapon;
using multihitshelpers::StackMultiHitRates;

auto Check() -> bool
{
    if (!ShouldRejectNullWeapon(true) || ShouldRejectNullWeapon(false))
    {
        return false;
    }
    {
        std::int16_t d = 0;
        std::int16_t t = 0;
        std::int16_t q = 0;
        StackMultiHitRates(10, 5, 2, 5, 5, true, true, d, t, q);
        if (d != 15 || t != 10 || q != 2)
        {
            return false;
        }
    }
    // QA: base 1 + 3 = 4
    if (ExpandMultiHits(1, 15, 10, 50, 0, 0, 0) != 4)
    {
        return false;
    }
    // TA when QA misses
    if (ExpandMultiHits(1, 15, 10, 50, 50, 0, 0) != 3)
    {
        return false;
    }
    // DA
    if (ExpandMultiHits(1, 15, 10, 50, 50, 10, 0) != 2)
    {
        return false;
    }
    if (HassoZanshinChance(40, 8, true) != 12 || HassoZanshinChance(40, 8, false) != 10)
    {
        return false;
    }
    if (ApplyHassoZanshinHit(1, 1, true, 25, 10) != 2 || ApplyHassoZanshinHit(1, 1, true, 25, 25) != 1)
    {
        return false;
    }
    if (CapMultiHits(10) != 8)
    {
        return false;
    }
    return true;
}
} // namespace

auto runMultiHits1551SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "multi_hits_1551 self-tests failed\n";
        return false;
    }
    return true;
}
