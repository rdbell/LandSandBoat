#include "test_mob_hp_1600.h"

#include "map/mob_hp_capacity.h"

#include <iostream>

namespace
{
using namespace mobhphelpers;

auto Check() -> bool
{
    if (CalculateBaseMobHP(0, 36, 6, 0) != 0)
    {
        return false;
    }
    if (CalculateBaseMobHP(1, 36, 6, 0) != 36 || CalculateBaseMobHP(5, 36, 6, 0) != 94)
    {
        return false;
    }
    if (CalculateBaseMobHP(30, 36, 6, 0) != 694)
    {
        return false;
    }
    if (CalculateBaseMobHP(50, 36, 6, 1) != 2202 || CalculateBaseMobHP(75, 36, 6, 1) != 4102)
    {
        return false;
    }

    // uint8 job-scale operands are promoted before addition in the C++
    // formula. These cases pin the non-wrapping behavior at the upper input
    // boundary for both jobScale + 5 and jobScale + 6.
    if (CalculateBaseMobHP(2, 0, 255, 0) != 260 || CalculateBaseMobHP(31, 0, 255, 0) != 7942 ||
        CalculateBaseMobHP(32, 0, 255, 0) != 8266)
    {
        return false;
    }

    if (CalculateSubjobHP(20, 6, 1) != 0 || CalculateSubjobHP(25, 6, 1) != 15)
    {
        return false;
    }
    if (CalculateSubjobHP(30, 6, 1) != 18 || CalculateSubjobHP(35, 6, 1) != 52)
    {
        return false;
    }
    if (CalculateSubjobHP(40, 6, 1) != 97 || CalculateSubjobHP(45, 6, 1) != 109)
    {
        return false;
    }
    if (CalculateSubjobHP(50, 6, 1) != 177 || CalculateSubjobHP(75, 6, 1) != 292)
    {
        return false;
    }
    return true;
}
} // namespace

auto runMobHP1600SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "mob_hp_1600 self-tests failed\n";
        return false;
    }
    return true;
}
