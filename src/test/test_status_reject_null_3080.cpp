#include "test_status_reject_null_3080.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldRejectNullStatusEffect 3080 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddStatusEffect null-effect formula for dual-wire cross-check (slice 3080):
//   isNull
auto inlineShouldRejectNullStatusEffect(const bool isNull) -> bool
{
    return isNull;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldRejectNullStatusEffect
// (isNull identity; slice 3080).
auto runStatusRejectNull3080SelfTests() -> bool
{
    using statuseffecthelpers::MaxEffectID;
    using statuseffecthelpers::ShouldClampMinDuration;
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldRejectEffectIDOutOfRange;
    using statuseffecthelpers::ShouldRejectNullStatusEffect;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1371 truth-table pins.
    ok = expect(ShouldRejectNullStatusEffect(true), "residual: null effect rejects") && ok;
    ok = expect(!ShouldRejectNullStatusEffect(false), "residual: non-null effect proceeds") && ok;

    const struct
    {
        bool        isNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null effect rejects" },
        { false, false, "non-null effect proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullStatusEffect(c.isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(c.isNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullStatusEffect dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.isNull, "free == isNull (identity)") && ok;
    }

    // Pin composition: identity of isNull only.
    ok = expect(ShouldRejectNullStatusEffect(true), "isNull must reject") && ok;
    ok = expect(!ShouldRejectNullStatusEffect(false), "!isNull must not reject") && ok;

    // Host path: CStatusEffectContainer::AddStatusEffect before other gates.
    const struct
    {
        bool        isNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PStatusEffectPtr == nullptr → warning + false before other gates" },
        { false, false, "PStatusEffectPtr non-null → proceed to ID-range / CanGain" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullStatusEffect(c.isNull);
        const bool inlineF = inlineShouldRejectNullStatusEffect(c.isNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.isNull, "host compose free == isNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool isNull : { false, true })
    {
        const bool got  = ShouldRejectNullStatusEffect(isNull);
        const bool want = isNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullStatusEffect(isNull),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1371 / 2932): null reject is distinct from
    // ID-range and min-duration clamp gates.
    ok = expect(ShouldRejectNullStatusEffect(true),
                "null effect must reject via ShouldRejectNullStatusEffect") &&
         ok;
    ok = expect(ShouldRejectEffectIDOutOfRange(MaxEffectID, MaxEffectID),
                "ID at MaxEffectID still rejects under dual-wire residual") &&
         ok;
    ok = expect(!ShouldRejectEffectIDOutOfRange(0, MaxEffectID),
                "valid ID must not reject via ID-range gate") &&
         ok;
    ok = expect(ShouldClampMinDuration(5, 10) && !ShouldClampMinDuration(10, 10),
                "min-duration residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldRejectNullStatusEffect(false),
                "non-null must proceed even if later ID-range / clamp fails") &&
         ok;

    // Orthogonal siblings 3049 / 3069 left alone — null-add independent of
    // expire / tick poles.
    ok = expect(ShouldExpireEffect(true, 100, 100),
                "expire residual still holds (orthogonal sibling 3049)") &&
         ok;
    ok = expect(ShouldTickEffect(true, 0, 1),
                "tick residual still holds (orthogonal sibling 3069)") &&
         ok;

    // Explicit dual-wire poles across dense bool space.
    for (const bool isNull : { false, true })
    {
        const bool got = ShouldRejectNullStatusEffect(isNull);
        ok             = expect(got == isNull, "host inject dual-wire pin") && ok;
        ok             = expect(got == inlineShouldRejectNullStatusEffect(isNull),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldExpireEffect(true, 100, 100),
                    "expire residual flipped under null-add compose") &&
             ok;
        ok = expect(ShouldTickEffect(true, 0, 1),
                    "tick residual flipped under null-add compose") &&
             ok;
    }

    return ok;
}
