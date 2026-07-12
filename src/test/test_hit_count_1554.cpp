#include "test_hit_count_1554.h"

#include "map/hit_count_capacity.h"

#include <iostream>

namespace
{
using hitcounthelpers::GetHitCount;

auto Check() -> bool
{
    // hits 0/1 always 1
    if (GetHitCount(0, 0) != 1 || GetHitCount(0, 99) != 1)
    {
        return false;
    }
    if (GetHitCount(1, 0) != 1 || GetHitCount(1, 99) != 1)
    {
        return false;
    }

    // hits 2: cdf 55,100
    if (GetHitCount(2, 54) != 1 || GetHitCount(2, 55) != 2 || GetHitCount(2, 99) != 2)
    {
        return false;
    }

    // hits 3: cdf 30,80,100
    if (GetHitCount(3, 29) != 1 || GetHitCount(3, 30) != 2 || GetHitCount(3, 79) != 2 ||
        GetHitCount(3, 80) != 3 || GetHitCount(3, 99) != 3)
    {
        return false;
    }

    // hits 4: cdf 20,50,80,100
    if (GetHitCount(4, 19) != 1 || GetHitCount(4, 20) != 2 || GetHitCount(4, 49) != 2 ||
        GetHitCount(4, 50) != 3 || GetHitCount(4, 79) != 3 || GetHitCount(4, 80) != 4 ||
        GetHitCount(4, 99) != 4)
    {
        return false;
    }

    // hits 5: cdf 10,30,60,90,100
    if (GetHitCount(5, 9) != 1 || GetHitCount(5, 10) != 2 || GetHitCount(5, 29) != 2 ||
        GetHitCount(5, 30) != 3 || GetHitCount(5, 59) != 3 || GetHitCount(5, 60) != 4 ||
        GetHitCount(5, 89) != 4 || GetHitCount(5, 90) != 5 || GetHitCount(5, 99) != 5)
    {
        return false;
    }

    // hits 6: cdf 10,30,50,70,90,100
    if (GetHitCount(6, 9) != 1 || GetHitCount(6, 10) != 2 || GetHitCount(6, 29) != 2 ||
        GetHitCount(6, 30) != 3 || GetHitCount(6, 49) != 3 || GetHitCount(6, 50) != 4 ||
        GetHitCount(6, 69) != 4 || GetHitCount(6, 70) != 5 || GetHitCount(6, 89) != 5 ||
        GetHitCount(6, 90) != 6 || GetHitCount(6, 99) != 6)
    {
        return false;
    }

    // hits 7: cdf 5,20,45,70,85,95,100
    if (GetHitCount(7, 4) != 1 || GetHitCount(7, 5) != 2 || GetHitCount(7, 19) != 2 ||
        GetHitCount(7, 20) != 3 || GetHitCount(7, 44) != 3 || GetHitCount(7, 45) != 4 ||
        GetHitCount(7, 69) != 4 || GetHitCount(7, 70) != 5 || GetHitCount(7, 84) != 5 ||
        GetHitCount(7, 85) != 6 || GetHitCount(7, 94) != 6 || GetHitCount(7, 95) != 7 ||
        GetHitCount(7, 99) != 7)
    {
        return false;
    }

    // hits 8: cdf 5,20,45,70,85,95,98,100
    if (GetHitCount(8, 4) != 1 || GetHitCount(8, 5) != 2 || GetHitCount(8, 19) != 2 ||
        GetHitCount(8, 20) != 3 || GetHitCount(8, 44) != 3 || GetHitCount(8, 45) != 4 ||
        GetHitCount(8, 69) != 4 || GetHitCount(8, 70) != 5 || GetHitCount(8, 84) != 5 ||
        GetHitCount(8, 85) != 6 || GetHitCount(8, 94) != 6 || GetHitCount(8, 95) != 7 ||
        GetHitCount(8, 97) != 7 || GetHitCount(8, 98) != 8 || GetHitCount(8, 99) != 8)
    {
        return false;
    }

    // hits > 8 → 1
    if (GetHitCount(9, 99) != 1 || GetHitCount(99, 0) != 1)
    {
        return false;
    }

    return true;
}
} // namespace

auto runHitCount1554SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "hit_count_1554 self-tests failed\n";
        return false;
    }
    return true;
}
