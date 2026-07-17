#include "test_clamming_bucket_overload_2869.h"

#include "map/clamming_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "clamming bucket overload 2869 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua nodeOnEventUpdate weight gate for dual-wire checks:
// kitWeight + itemWeight > kitSize
auto inlineBucketOverloaded(const int32 kitWeight, const int32 itemWeight, const int32 kitSize) -> bool
{
    return kitWeight + itemWeight > kitSize;
}

// Inline always-run kit weight update:
// kitWeight + itemWeight
auto inlineTotalWeight(const int32 kitWeight, const int32 itemWeight) -> int32
{
    return kitWeight + itemWeight;
}

} // namespace

// Pure dual-wire expansion for clamminghelpers::BucketOverloaded /
// TotalWeight (Lua nodeOnEventUpdate kit weight / overload gate).
auto runClammingBucketOverload2869SelfTests() -> bool
{
    using clamminghelpers::BucketOverloaded;
    using clamminghelpers::TotalWeight;

    bool ok = true;

    const struct
    {
        int32       kitWeight;
        int32       itemWeight;
        int32       kitSize;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 0, 0, false, "all zero not overloaded" },
        { 0, 0, 50, false, "empty kit empty item" },
        { 0, 50, 50, false, "exact capacity not overloaded (strict >)" },
        { 45, 5, 50, false, "exact residual 1013 pin 45+5==50" },
        { 45, 6, 50, true, "over by 1 residual 1013 pin 45+6>50" },
        { 50, 0, 50, false, "full kit zero item exact" },
        { 50, 1, 50, true, "full kit any positive item" },
        { 20, 35, 50, true, "igneous rock into nearly full 50 kit" },
        { 0, 51, 50, true, "single item over capacity" },
        { 100, 1, 100, true, "upgrade-size kit full +1" },
        { 95, 5, 100, false, "exact 100 capacity" },
        { 95, 6, 100, true, "over 100 by 1" },
        { 0, 1, 0, true, "zero capacity any item" },
        { -1, 0, 0, false, "defensive negative kit exact" },
        { -1, 2, 0, true, "defensive negative kit over" },
        { 10, -5, 10, false, "defensive negative item under" },
        { 10, -5, 4, true, "defensive negative item still over size" },
        { 40, 11, 50, true, "jacknife weight into 40 of 50" },
        { 40, 10, 50, false, "exact 50 with 10 weight" },
    };

    for (const auto& c : cases)
    {
        const bool  got     = BucketOverloaded(c.kitWeight, c.itemWeight, c.kitSize);
        const bool  inlineF = inlineBucketOverloaded(c.kitWeight, c.itemWeight, c.kitSize);
        const bool  pure    = TotalWeight(c.kitWeight, c.itemWeight) > c.kitSize;
        const int32 weight  = TotalWeight(c.kitWeight, c.itemWeight);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == TotalWeight > kitSize") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(weight == inlineTotalWeight(c.kitWeight, c.itemWeight), "TotalWeight free == inline") && ok;
    }

    // Residual 1013 pins.
    ok = expect(TotalWeight(40, 11) == 51, "residual TotalWeight(40,11)==51") && ok;
    ok = expect(!BucketOverloaded(45, 5, 50), "residual exact capacity not overloaded") && ok;
    ok = expect(BucketOverloaded(45, 6, 50), "residual over by 1 overloaded") && ok;
    ok = expect(!BucketOverloaded(0, 50, 50), "residual empty+50 exact not overloaded") && ok;
    ok = expect(BucketOverloaded(20, 35, 50), "residual igneous nearly full overloaded") && ok;

    // Dense compose range identity (kit sizes 50/100/150/200 + edge 0).
    for (const int32 kitSize : { 0, 50, 100, 150, 200 })
    {
        for (int32 kitWeight = -2; kitWeight <= kitSize + 5; ++kitWeight)
        {
            for (int32 itemWeight = -2; itemWeight <= 40; ++itemWeight)
            {
                const bool got  = BucketOverloaded(kitWeight, itemWeight, kitSize);
                const bool want = kitWeight + itemWeight > kitSize;
                ok = expect(got == want, "compose range free == kitWeight+itemWeight>kitSize") && ok;
                ok = expect(got == inlineBucketOverloaded(kitWeight, itemWeight, kitSize),
                            "compose range free == inline") &&
                     ok;
                ok = expect(TotalWeight(kitWeight, itemWeight) == kitWeight + itemWeight,
                            "compose TotalWeight free == sum") &&
                     ok;
            }
        }
    }

    return ok;
}
