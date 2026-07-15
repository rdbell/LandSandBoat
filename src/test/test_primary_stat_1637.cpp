#include "test_primary_stat_1637.h"

#include "map/primary_stat_capacity.h"

#include <iostream>

namespace
{
using namespace primarystathelpers;

auto Check() -> bool
{
    // Clamp low: negative sum → 0
    if (ClampPrimaryStat(-1) != 0 || ClampPrimaryStat(-9999) != 0)
    {
        return false;
    }
    // Clamp high: >999 → 999
    if (ClampPrimaryStat(1000) != 999 || ClampPrimaryStat(50000) != 999)
    {
        return false;
    }
    // Clamp mid / bounds
    if (ClampPrimaryStat(0) != 0 || ClampPrimaryStat(999) != 999 || ClampPrimaryStat(42) != 42)
    {
        return false;
    }

    // Plain primary: base + mod (DEX/VIT/AGI/INT/MND/CHR pattern)
    if (ResolvePrimaryStat(50, 10) != 60)
    {
        return false;
    }
    if (ResolvePrimaryStat(50, -20) != 30)
    {
        return false;
    }
    if (ResolvePrimaryStat(5, -20) != 0)
    {
        return false;
    }
    if (ResolvePrimaryStat(900, 200) != 999)
    {
        return false;
    }
    if (ResolvePrimaryStat(0, 0) != 0)
    {
        return false;
    }

    // STR two-handed path includes TWOHAND_STR
    if (ResolveSTR(50, 10, 25, true) != 85)
    {
        return false;
    }
    // STR non-twohand does not include TWOHAND_STR
    if (ResolveSTR(50, 10, 25, false) != 60)
    {
        return false;
    }
    // STR two-hand clamp high / low
    if (ResolveSTR(900, 50, 100, true) != 999)
    {
        return false;
    }
    if (ResolveSTR(10, -5, -20, true) != 0)
    {
        return false;
    }
    // Zero TWOHAND_STR on two-hand matches plain
    if (ResolveSTR(70, 5, 0, true) != ResolvePrimaryStat(70, 5))
    {
        return false;
    }

    return true;
}
} // namespace

auto runPrimaryStat1637SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "primary_stat_1637 self-tests failed\n";
        return false;
    }
    return true;
}
