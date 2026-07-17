#include "test_clamming_bucket_overloaded_3180.h"

#include "map/clamming_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "clamming BucketOverloaded 3180 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua nodeOnEventUpdate weight gate for dual-wire cross-checks
// (dedicated 3180):
//   kitWeight + itemWeight > kitSize
auto inlineBucketOverloaded3180(const int32 kitWeight, const int32 itemWeight, const int32 kitSize) -> bool
{
    return kitWeight + itemWeight > kitSize;
}

// Compact dual-wire pin matching Go pinBucketOverloaded3180 / C++ capacity
// TotalWeight form (formula unchanged from 1013 / 2869):
//   TotalWeight(kitWeight, itemWeight) > kitSize
auto pinBucketOverloaded3180(const int32 kitWeight, const int32 itemWeight, const int32 kitSize) -> bool
{
    return clamminghelpers::TotalWeight(kitWeight, itemWeight) > kitSize;
}

// Inline always-run kit weight update for dual-wire cross-checks (dedicated 3180):
//   kitWeight + itemWeight
auto inlineTotalWeight3180(const int32 kitWeight, const int32 itemWeight) -> int32
{
    return kitWeight + itemWeight;
}

} // namespace

// Pure dual-wire expansion for clamminghelpers::BucketOverloaded /
// TotalWeight (Lua nodeOnEventUpdate kit weight / overload gate;
// OmegaXI internal/clamming; dedicated slice 3180; residual expand 2869 /
// pure 1013).
//
// Coverage:
//   - free == inline == pin
//   - residual 2869 / 1013 pins still hold
//   - poles: exact capacity allowed (strict >), overflow
//   - dense compose free == inline == pin
//   - host inject nodeOnEventUpdate path semantics
auto runClammingBucketOverloaded3180SelfTests() -> bool
{
    using clamminghelpers::BucketOverloaded;
    using clamminghelpers::TotalWeight;

    bool ok = true;

    // Residual 1013 / 2869 pins still hold under dedicated dual-wire.
    ok = expect(!BucketOverloaded(45, 5, 50), "residual 1013/2869: exact capacity 45+5==50 not overloaded") && ok;
    ok = expect(BucketOverloaded(45, 6, 50), "residual 1013/2869: over by 1 45+6>50 overloaded") && ok;
    ok = expect(!BucketOverloaded(0, 50, 50), "residual 1013/2869: empty+50 exact not overloaded") && ok;
    ok = expect(BucketOverloaded(20, 35, 50), "residual 1013/2869: igneous nearly full overloaded") && ok;
    ok = expect(TotalWeight(40, 11) == 51, "residual 1013 TotalWeight(40,11)==51") && ok;

    // --- Core poles: free == inline == pin (exact capacity allowed, overflow) ---
    const struct
    {
        int32       kitWeight;
        int32       itemWeight;
        int32       kitSize;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic exact-capacity / overflow poles (strict >).
        { 0, 0, 0, false, "all zero not overloaded" },
        { 0, 50, 50, false, "exact capacity not overloaded (strict >)" },
        { 45, 5, 50, false, "exact residual 1013 pin 45+5==50" },
        { 45, 6, 50, true, "over by 1 residual 1013 pin 45+6>50" },
        { 50, 0, 50, false, "full kit zero item exact" },
        { 50, 1, 50, true, "full kit any positive item overflow" },
        { 20, 35, 50, true, "igneous rock into nearly full 50 kit" },
        { 0, 51, 50, true, "single item over capacity overflow" },
        { 95, 5, 100, false, "exact 100 capacity allowed" },
        { 95, 6, 100, true, "over 100 by 1 overflow" },
        { 100, 1, 100, true, "upgrade-size kit full +1 overflow" },
        { 40, 10, 50, false, "exact 50 with 10 weight allowed" },
        { 40, 11, 50, true, "jacknife weight into 40 of 50 overflow" },

        // Residual 2869 defensive / edge poles.
        { 0, 0, 50, false, "residual 2869 empty kit empty item" },
        { 0, 1, 0, true, "residual 2869 zero capacity any item" },
        { -1, 0, 0, false, "residual 2869 defensive negative kit exact" },
        { -1, 2, 0, true, "residual 2869 defensive negative kit over" },
        { 10, -5, 10, false, "residual 2869 defensive negative item under" },
        { 10, -5, 4, true, "residual 2869 defensive negative item still over" },
    };

    for (const auto& c : cases)
    {
        const bool  got     = BucketOverloaded(c.kitWeight, c.itemWeight, c.kitSize);
        const bool  inlineF = inlineBucketOverloaded3180(c.kitWeight, c.itemWeight, c.kitSize);
        const bool  pin     = pinBucketOverloaded3180(c.kitWeight, c.itemWeight, c.kitSize);
        // Positive form pin composition (explicit TotalWeight > kitSize).
        const bool  wantPin = TotalWeight(c.kitWeight, c.itemWeight) > c.kitSize;
        const int32 weight  = TotalWeight(c.kitWeight, c.itemWeight);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "BucketOverloaded free == inline == pin") &&
             ok;
        ok = expect(weight == inlineTotalWeight3180(c.kitWeight, c.itemWeight),
                    "TotalWeight free == inline") &&
             ok;
    }

    // Free == pin across exact-capacity / overflow poles.
    ok = expect(!BucketOverloaded(45, 5, 50), "exact capacity must be allowed") && ok;
    ok = expect(BucketOverloaded(45, 5, 50) == pinBucketOverloaded3180(45, 5, 50),
                "free == pin for exact capacity 45+5==50") &&
         ok;
    ok = expect(BucketOverloaded(45, 6, 50), "overflow by 1 must overload") && ok;
    ok = expect(BucketOverloaded(45, 6, 50) == pinBucketOverloaded3180(45, 6, 50),
                "free == pin for overflow 45+6>50") &&
         ok;

    // Residual 1013 TotalWeight dual-wire free == inline.
    ok = expect(TotalWeight(40, 11) == inlineTotalWeight3180(40, 11), "TotalWeight free == inline 3180") && ok;
    ok = expect(TotalWeight(40, 11) == 51, "TotalWeight(40,11)==51") && ok;

    // Dense compose: free == inline == pin across kit sizes / weights.
    for (const int32 kitSize : { 0, 50, 100, 150, 200 })
    {
        for (int32 kitWeight = -2; kitWeight <= kitSize + 5; ++kitWeight)
        {
            for (int32 itemWeight = -2; itemWeight <= 40; ++itemWeight)
            {
                const bool got     = BucketOverloaded(kitWeight, itemWeight, kitSize);
                const bool inlineF = inlineBucketOverloaded3180(kitWeight, itemWeight, kitSize);
                const bool pin     = pinBucketOverloaded3180(kitWeight, itemWeight, kitSize);
                const bool want    = kitWeight + itemWeight > kitSize;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
                ok                 = expect(TotalWeight(kitWeight, itemWeight) == kitWeight + itemWeight,
                                            "compose TotalWeight free == sum") &&
                     ok;
            }
        }
    }

    // --- Production nodeOnEventUpdate path semantics ---
    // Host injects kitWeight / itemWeight / kitSize into BucketOverloaded:
    //   true  → KitBroken + emptyBucket
    //   false → add item to bucket locals
    // Always: kitWeight = TotalWeight(kitWeight, itemWeight)
    // free == inline == pin for host inject poles.
    const struct
    {
        int32       kitWeight;
        int32       itemWeight;
        int32       kitSize;
        bool        want;
        const char* label;
    } hostCases[] = {
        { 45, 5, 50, false, "host exact capacity → add item path" },
        { 45, 6, 50, true, "host overflow → KitBroken path" },
        { 0, 50, 50, false, "host empty fill exact → add item path" },
        { 20, 35, 50, true, "host igneous overload → KitBroken path" },
    };
    for (const auto& c : hostCases)
    {
        const bool got     = BucketOverloaded(c.kitWeight, c.itemWeight, c.kitSize);
        const bool inlineF = inlineBucketOverloaded3180(c.kitWeight, c.itemWeight, c.kitSize);
        const bool pin     = pinBucketOverloaded3180(c.kitWeight, c.itemWeight, c.kitSize);
        ok                 = expect(got == c.want, c.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        // Always-run weight update independent of overload outcome.
        ok = expect(TotalWeight(c.kitWeight, c.itemWeight) == c.kitWeight + c.itemWeight,
                    "host always-run TotalWeight") &&
             ok;
    }

    return ok;
}
