#include "test_subjob_level_1657.h"

#include "map/subjob_level_capacity.h"

#include <iostream>

namespace
{
using namespace subjoblevelhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "subjob level 1657 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSubjobLevel1657SelfTests() -> bool
{
    bool ok = true;

    // --- ratio pins ---
    ok = expect(RatioNone == 0 && RatioHalf == 1 && RatioTwoThirds == 2 && RatioEqual == 3,
                "ratio pins 0/1/2/3") &&
         ok;

    // --- ResolveMainLevel ---
    ok = expect(ResolveMainLevel(0) == 1, "main 0 → 1") && ok;
    ok = expect(ResolveMainLevel(1) == 1, "main 1 stays 1") && ok;
    ok = expect(ResolveMainLevel(99) == 99, "main 99") && ok;
    ok = expect(ResolveMainLevel(255) == 255, "main 255") && ok;

    // --- CapHalf (retail 75/37, 99/49; mlvl==1 special) ---
    ok = expect(CapHalf(0) == 0, "half 0") && ok;
    ok = expect(CapHalf(1) == 1, "half 1 special") && ok;
    ok = expect(CapHalf(2) == 1, "half 2") && ok;
    ok = expect(CapHalf(75) == 37, "half 75") && ok;
    ok = expect(CapHalf(99) == 49, "half 99") && ok;

    // --- CapTwoThirds (retail 75/50, 99/66; mlvl==1 special) ---
    ok = expect(CapTwoThirds(0) == 0, "2/3 0") && ok;
    ok = expect(CapTwoThirds(1) == 1, "2/3 1 special") && ok;
    ok = expect(CapTwoThirds(75) == 50, "2/3 75") && ok;
    ok = expect(CapTwoThirds(99) == 66, "2/3 99") && ok;
    ok = expect(CapTwoThirds(255) == 170, "2/3 255 widened") && ok;

    // --- CapEqual ---
    ok = expect(CapEqual(1) == 1, "equal 1") && ok;
    ok = expect(CapEqual(99) == 99, "equal 99") && ok;

    // --- ratio 0: no SJ ---
    ok = expect(ResolveSubLevel(99, 50, false, false, RatioNone) == 0, "ratio0 forces 0") && ok;
    ok = expect(ResolveSubLevel(1, 1, false, false, RatioNone) == 0, "ratio0 even at 1") && ok;

    // --- ratio 1: 1/2 ---
    ok = expect(ResolveSubLevel(75, 50, false, false, RatioHalf) == 37, "half 75/50") && ok;
    ok = expect(ResolveSubLevel(99, 50, false, false, RatioHalf) == 49, "half 99/50") && ok;
    ok = expect(ResolveSubLevel(99, 49, false, false, RatioHalf) == 49, "half 99/49 exact") && ok;
    ok = expect(ResolveSubLevel(99, 30, false, false, RatioHalf) == 30, "half under cap") && ok;
    ok = expect(ResolveSubLevel(1, 99, false, false, RatioHalf) == 1, "half mlvl1 edge") && ok;
    ok = expect(ResolveSubLevel(1, 0, false, false, RatioHalf) == 0, "half mlvl1 slvl0") && ok;

    // --- ratio 2: 2/3 ---
    ok = expect(ResolveSubLevel(75, 50, false, false, RatioTwoThirds) == 50, "2/3 75/50") && ok;
    ok = expect(ResolveSubLevel(75, 51, false, false, RatioTwoThirds) == 50, "2/3 75/51 clamp") && ok;
    ok = expect(ResolveSubLevel(99, 70, false, false, RatioTwoThirds) == 66, "2/3 99/70") && ok;
    ok = expect(ResolveSubLevel(1, 50, false, false, RatioTwoThirds) == 1, "2/3 mlvl1 edge") && ok;

    // --- ratio 3: equal ---
    ok = expect(ResolveSubLevel(75, 75, false, false, RatioEqual) == 75, "equal 75/75") && ok;
    ok = expect(ResolveSubLevel(99, 100, false, false, RatioEqual) == 99, "equal clamp") && ok;
    ok = expect(ResolveSubLevel(50, 40, false, false, RatioEqual) == 40, "equal under") && ok;
    ok = expect(ResolveSubLevel(1, 5, false, false, RatioEqual) == 1, "equal mlvl1") && ok;

    // --- invalid ratio: leave requested ---
    ok = expect(ResolveSubLevel(99, 40, false, false, 4) == 40, "invalid ratio leaves requested") && ok;
    ok = expect(ResolveSubLevel(50, 12, false, false, 255) == 12, "invalid 255 leaves requested") && ok;

    // --- mob without INCLUDE_MOB_SJ: 1:1 main (default retail) ---
    ok = expect(ResolveSubLevel(50, 1, true, false, RatioHalf) == 50, "mob !SJ 50") && ok;
    ok = expect(ResolveSubLevel(99, 10, true, false, RatioHalf) == 99, "mob !SJ ignores slvl") && ok;
    ok = expect(ResolveSubLevel(75, 0, true, false, RatioNone) == 75, "mob !SJ ignores ratio0") && ok;
    ok = expect(ResolveSubLevel(1, 99, true, false, RatioEqual) == 1, "mob !SJ mlvl1") && ok;

    // --- mob with INCLUDE_MOB_SJ: follows ratio ---
    ok = expect(ResolveSubLevel(99, 50, true, true, RatioHalf) == 49, "mob SJ half") && ok;
    ok = expect(ResolveSubLevel(75, 50, true, true, RatioTwoThirds) == 50, "mob SJ 2/3") && ok;
    ok = expect(ResolveSubLevel(99, 80, true, true, RatioNone) == 0, "mob SJ ratio0") && ok;
    ok = expect(ResolveSubLevel(50, 60, true, true, RatioEqual) == 50, "mob SJ equal") && ok;

    // --- non-mob always uses ratio (INCLUDE_MOB_SJ irrelevant) ---
    ok = expect(ResolveSubLevel(99, 50, false, false, RatioHalf) == 49, "non-mob half") && ok;
    ok = expect(ResolveSubLevel(99, 50, false, true, RatioHalf) == 49, "non-mob half include flag") && ok;

    // --- ClampToCap unit ---
    ok = expect(ClampToCap(30, 49) == 30, "clamp under") && ok;
    ok = expect(ClampToCap(50, 49) == 49, "clamp over") && ok;
    ok = expect(ClampToCap(49, 49) == 49, "clamp exact") && ok;

    return ok;
}
