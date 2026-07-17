#include "test_latent_apply_expression_2983.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent ShouldApplyLatentExpression 2983 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ProcessLatentEffect latentFound → ApplyLatentEffect gate for dual-wire
// cross-check (slice 2983):
//   latentFound
auto inlineShouldApplyLatentExpression(const bool latentFound) -> bool
{
    return latentFound;
}

} // namespace

// Pure dual-wire expansion for latenthelpers::ShouldApplyLatentExpression
// (latentFound before ApplyLatentEffect; slice 2983).
auto runLatentApplyExpression2983SelfTests() -> bool
{
    using latenthelpers::ShouldApplyLatentExpression;

    bool ok = true;

    const struct
    {
        bool        latentFound;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles (identity).
        { true, true, "latent found → apply" },
        { false, false, "latent not found → skip apply" },

        // Residual 1359 pin.
        { true, true, "residual found apply" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyLatentExpression(c.latentFound);
        const bool inlineF = inlineShouldApplyLatentExpression(c.latentFound);
        const bool wantPin = c.latentFound;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyLatentExpression dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyLatentExpression == pin formula latentFound") && ok;
    }

    // Pin composition: pure identity only.
    ok = expect(ShouldApplyLatentExpression(true), "latentFound true must apply") && ok;
    ok = expect(!ShouldApplyLatentExpression(false), "latentFound false must skip apply") && ok;

    // Dense compose: full bool domain.
    for (const bool latentFound : { false, true })
    {
        const bool got = ShouldApplyLatentExpression(latentFound);
        ok             = expect(got == latentFound, "compose free == pin formula latentFound") && ok;
        ok             = expect(got == inlineShouldApplyLatentExpression(latentFound),
                    "compose free == inline") &&
             ok;
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
    } composeCases[] = {
        { true, true, "known condition: apply with expression" },
        { false, false, "unhandled default: skip ApplyLatentEffect return false" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldApplyLatentExpression(c.latentFound);
        ok             = expect(got == c.wantApply, c.label) && ok;
        ok             = expect(got == c.latentFound, "compose free == latentFound pin") && ok;
        ok             = expect(got == inlineShouldApplyLatentExpression(c.latentFound), "compose free == inline") && ok;
    }

    return ok;
}
