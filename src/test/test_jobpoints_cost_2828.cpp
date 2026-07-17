#include "test_jobpoints_cost_2828.h"

#include "map/job_points_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jobpoints cost 2828 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU8(const uint8 got, const uint8 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "jobpoints cost 2828 self-test failed: " << label
                  << " got=" << static_cast<unsigned>(got)
                  << " want=" << static_cast<unsigned>(want) << '\n';
        return false;
    }
    return true;
}

auto expectU16(const uint16 got, const uint16 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "jobpoints cost 2828 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runJobPointsCost2828SelfTests() -> bool
{
    using namespace jobpointshelpers;

    bool ok = true;

    // --- JobPointCost pure form (display / raise cost) ---
    ok = expectU8(JobPointCost(0), 1, "cost(0)") && ok;
    ok = expectU8(JobPointCost(1), 2, "cost(1)") && ok;
    ok = expectU8(JobPointCost(9), 10, "cost(9)") && ok;
    ok = expectU8(JobPointCost(19), 20, "cost(19)") && ok;
    ok = expectU8(JobPointCost(20), 0, "cost(20) blocks raise") && ok;
    ok = expectU8(JobPointCost(21), 1, "cost(21) wraps") && ok;

    // --- GetJobPointCost is the macro-safe dual-wire alias ---
    ok = expectU8(GetJobPointCost(0), JobPointCost(0), "get==cost(0)") && ok;
    ok = expectU8(GetJobPointCost(19), JobPointCost(19), "get==cost(19)") && ok;
    ok = expectU8(GetJobPointCost(20), JobPointCost(20), "get==cost(20)") && ok;
    ok = expectU8(GetJobPointCost(21), JobPointCost(21), "get==cost(21)") && ok;

    // --- ShouldRaiseAffordable (value+jp gate, no presence check) ---
    ok = expect(ShouldRaiseAffordable(0, 1), "afford value0 jp1") && ok;
    ok = expect(ShouldRaiseAffordable(0, 50), "afford value0 surplus") && ok;
    ok = expect(!ShouldRaiseAffordable(0, 0), "not afford value0 jp0") && ok;
    ok = expect(!ShouldRaiseAffordable(9, 5), "not afford value9 cost10") && ok;
    ok = expect(ShouldRaiseAffordable(9, 10), "afford value9 exact") && ok;
    ok = expect(ShouldRaiseAffordable(19, 20), "afford value19 exact") && ok;
    ok = expect(!ShouldRaiseAffordable(19, 19), "not afford value19 short") && ok;
    ok = expect(!ShouldRaiseAffordable(20, 500), "cap value20 cost0") && ok;
    ok = expect(ShouldRaiseAffordable(4, 5), "afford value4 exact") && ok;

    // Cross-check: ShouldRaiseAffordable == ShouldApplyRaiseJobPoint(cost, jp)
    for (uint8 value = 0; value <= 21; ++value)
    {
        const uint8  cost = JobPointCost(value);
        const uint16 jp   = 10;
        const bool   want = ShouldApplyRaiseJobPoint(cost, jp);
        if (ShouldRaiseAffordable(value, jp) != want)
        {
            ok = expect(false, "affordable matches apply gate") && ok;
            break;
        }
    }

    // --- TotalCostToLevel closed form ---
    ok = expectU8(kMaxTypeLevel, 20, "kMaxTypeLevel") && ok;
    ok = expectU16(TotalCostToLevel(0), 0, "total 0") && ok;
    ok = expectU16(TotalCostToLevel(1), 1, "total 1") && ok;
    ok = expectU16(TotalCostToLevel(5), 15, "total 5") && ok;
    ok = expectU16(TotalCostToLevel(20), 210, "total 20") && ok;
    ok = expectU16(TotalCostToLevel(21), 210, "total 21 clamps") && ok;
    ok = expectU16(TotalCostToLevel(255), 210, "total 255 clamps") && ok;

    // Running sum of JobPointCost(0..level-1) matches TotalCostToLevel
    {
        uint16 sum = 0;
        for (uint8 level = 0; level <= kMaxTypeLevel; ++level)
        {
            if (level > 0)
            {
                sum = static_cast<uint16>(sum + JobPointCost(static_cast<uint8>(level - 1)));
            }
            if (TotalCostToLevel(level) != sum)
            {
                ok = expect(false, "total matches running cost sum") && ok;
                break;
            }
        }
    }

    return ok;
}
