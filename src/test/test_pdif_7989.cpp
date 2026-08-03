#include "map/pdif_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

using namespace pdifhelpers;

auto almostEqual(const double left, const double right) -> bool
{
    return std::fabs(left - right) < 1e-12;
}

} // namespace

auto runPDIF7989SelfTests() -> bool
{
    if (!almostEqual(FinishMeleePDIF(1.5, 0, false, 0, 0), 1.5) ||
        !almostEqual(FinishMeleePDIF(1.5, 5, false, 0, 0), 1.5 * 1.05) ||
        !almostEqual(FinishMeleePDIF(-1.0, -1, false, 0, 0), -1.0) ||
        !almostEqual(FinishMeleePDIF(1.5, 6, true, 20, 5), 1.5 * 1.05 * 1.15))
    {
        std::cerr << "pDIF melee finish 7989 self-test failed\n";
        return false;
    }

    if (!almostEqual(FinishRangedPDIF(-1.0, false, 0, 0), 0.0) ||
        !almostEqual(FinishRangedPDIF(1.0, true, 0, 0), 1.25) ||
        !almostEqual(FinishRangedPDIF(1.0, true, 20, 5), 1.25 * 1.15))
    {
        std::cerr << "pDIF ranged finish 7989 self-test failed\n";
        return false;
    }

    int low  = 0;
    int high = 0;
    if (!RatioRollRange(0.1259, 1.9999, low, high) || low != 125 || high != 1999 ||
        !RatioRollRange(2.4, 1.1, low, high) || low != 1100 || high != 2400 ||
        !RatioRollRange(-0.001, 0.001, low, high) || low != -1 || high != 1 ||
        RatioRollRange(-2.0, 0.0, low, high))
    {
        std::cerr << "pDIF ratio roll range 7989 self-test failed\n";
        return false;
    }

    return true;
}
