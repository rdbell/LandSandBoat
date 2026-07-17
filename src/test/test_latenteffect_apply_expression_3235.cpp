#include "test_latenteffect_apply_expression_3235.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "latenteffect ShouldApplyLatentExpression 3235 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ProcessLatentEffect latentFound → ApplyLatentEffect gate for dual-wire
// cross-check (dedicated slice 3235 expand residual 2983). Direct return:
//   latentFound
auto inlineShouldApplyLatentExpression(const bool latentFound) -> bool
{
    return latentFound;
}

// Compact dual-wire pin matching Go pinShouldApplyLatentExpression3235 /
// C++ capacity. Direct return:
//   latentFound
auto pinShouldApplyLatentExpression(const bool latentFound) -> bool
{
    return latentFound;
}

} // namespace

// Pure dual-wire expansion for latenthelpers::ShouldApplyLatentExpression
// (latentFound before ApplyLatentEffect; OmegaXI internal/latenteffect;
// dedicated slice 3235 expand residual 2983 / pure 1359).
//
// Coverage:
//   - free == inline == pin (direct return)
//   - residual 1359 / 2983 poles still hold (true/false)
//   - dense full bool domain
//   - host-style ProcessLatentEffect inject poles
auto runLatenteffectApplyExpression3235SelfTests() -> bool
{
    using latenthelpers::ShouldApplyLatentExpression;

    bool ok = true;

    // Residual 1359 / 2983 pins still hold under dual-wire.
    ok = expect(ShouldApplyLatentExpression(true), "residual 2983/1359: latentFound true must apply") && ok;
    ok = expect(!ShouldApplyLatentExpression(false), "residual 2983/1359: latentFound false must skip apply") && ok;

    const struct
    {
        bool        latentFound;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles (identity).
        { true, true, "pole latent found → apply" },
        { false, false, "pole latent not found → skip apply" },

        // Residual 2983 / 1359 re-pins.
        { true, true, "residual found apply" },
        { false, false, "residual not found skip" },

        // Host-style ProcessLatentEffect inject poles.
        { true, true, "known condition: apply with expression" },
        { false, false, "unhandled default: skip ApplyLatentEffect return false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyLatentExpression(c.latentFound);
        const bool inlineF = inlineShouldApplyLatentExpression(c.latentFound);
        const bool pinF    = pinShouldApplyLatentExpression(c.latentFound);
        const bool wantPin = c.latentFound;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyLatentExpression dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldApplyLatentExpression dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyLatentExpression == pin formula latentFound") && ok;
    }

    // Pin composition: pure identity only.
    ok = expect(ShouldApplyLatentExpression(true), "latentFound true must apply") && ok;
    ok = expect(!ShouldApplyLatentExpression(false), "latentFound false must skip apply") && ok;

    // Explicit polarity: apply is exact identity of inject value.
    for (const bool latentFound : { false, true })
    {
        const bool got  = ShouldApplyLatentExpression(latentFound);
        const bool want = latentFound;
        ok              = expect(got == want, "polarity: free == latentFound") && ok;
        ok              = expect(!(got && !latentFound), "polarity: never apply when !latentFound") && ok;
        ok              = expect(!(!got && latentFound), "polarity: always apply when latentFound") && ok;
    }

    // Host-style compose poles: ProcessLatentEffect initializes
    // latentFound = true, sets latentFound = false only on the unhandled
    // default switch case, then gates ApplyLatentEffect with
    // ShouldApplyLatentExpression(latentFound).
    const struct
    {
        bool        latentFound;
        bool        wantApply;
        const char* label;
    } hostCases[] = {
        { true, true, "known condition: apply with expression" },
        { false, false, "unhandled default: skip ApplyLatentEffect return false" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldApplyLatentExpression(c.latentFound);
        const bool inlineF = inlineShouldApplyLatentExpression(c.latentFound);
        const bool pinF    = pinShouldApplyLatentExpression(c.latentFound);

        ok = expect(got == c.wantApply, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == c.latentFound, "host compose free == latentFound (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool latentFound : { false, true })
    {
        const bool got  = ShouldApplyLatentExpression(latentFound);
        const bool want = latentFound;
        ok              = expect(got == want, "compose free == pin formula latentFound") && ok;
        ok              = expect(got == inlineShouldApplyLatentExpression(latentFound),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldApplyLatentExpression(latentFound), "compose free == pin") && ok;
    }

    // Residual 2983 suite still holds under dedicated expand.
    ok = expect(ShouldApplyLatentExpression(true), "residual 2983 found-apply pin under 3235") && ok;
    ok = expect(!ShouldApplyLatentExpression(false), "residual 2983 not-found-skip pin under 3235") && ok;

    return ok;
}
