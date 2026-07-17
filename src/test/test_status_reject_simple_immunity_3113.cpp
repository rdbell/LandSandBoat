#include "test_status_reject_simple_immunity_3113.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldRejectSimpleImmunity 3113 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CanGainStatusEffect simple-immunity formula for dual-wire cross-check (slice 3113):
//   hasImmunity
auto inlineShouldRejectSimpleImmunity(const bool hasImmunity) -> bool
{
    return hasImmunity;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldRejectSimpleImmunity
// (hasImmunity identity; slice 3113). Dense space: full 2¹.
auto runStatusRejectSimpleImmunity3113SelfTests() -> bool
{
    using statuseffecthelpers::ShouldBlockByBlockId;
    using statuseffecthelpers::ShouldBlockCharmOnPet;
    using statuseffecthelpers::ShouldBlockSpikesDueToAftermath;
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldRejectNullStatusEffect;
    using statuseffecthelpers::ShouldRejectSimpleImmunity;
    using statuseffecthelpers::ShouldRejectSleepImmunity;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1364 identity pins.
    ok = expect(ShouldRejectSimpleImmunity(true), "residual: hasImmunity rejects") && ok;
    ok = expect(!ShouldRejectSimpleImmunity(false), "residual: no immunity proceeds") && ok;

    const struct
    {
        bool        hasImmunity;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "hasImmunity rejects" },
        { false, false, "no immunity proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectSimpleImmunity(c.hasImmunity);
        const bool inlineF = inlineShouldRejectSimpleImmunity(c.hasImmunity);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectSimpleImmunity dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.hasImmunity, "free == hasImmunity (identity)") && ok;
    }

    // Pin composition: identity of hasImmunity only.
    ok = expect(ShouldRejectSimpleImmunity(true), "hasImmunity must reject") && ok;
    ok = expect(!ShouldRejectSimpleImmunity(false), "!hasImmunity must not reject") && ok;

    // Host path: CStatusEffectContainer::CanGainStatusEffect before later gates.
    const struct
    {
        bool        hasImmunity;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "hasImmunity(IMMUNITY_*) → reject CanGain" },
        { false, false, "no immunity → proceed past simple-immunity gate" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectSimpleImmunity(c.hasImmunity);
        const bool inlineF = inlineShouldRejectSimpleImmunity(c.hasImmunity);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.hasImmunity, "host compose free == hasImmunity (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula across full 2¹.
    for (const bool hasImmunity : { false, true })
    {
        const bool got  = ShouldRejectSimpleImmunity(hasImmunity);
        const bool want = hasImmunity;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectSimpleImmunity(hasImmunity),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1364): simple immunity distinct from sleep
    // immunity / charm-on-pet / block-id / aftermath spike gates.
    ok = expect(ShouldRejectSimpleImmunity(true),
                "hasImmunity must reject via ShouldRejectSimpleImmunity") &&
         ok;
    ok = expect(ShouldRejectSleepImmunity(true, true, false, true, false),
                "sleep-immunity residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldBlockCharmOnPet(true, true) && !ShouldBlockCharmOnPet(true, false),
                "charm-on-pet residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldBlockByBlockId(5, true) && !ShouldBlockByBlockId(0, true),
                "block-id residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldBlockSpikesDueToAftermath(true, 8) && !ShouldBlockSpikesDueToAftermath(true, 7),
                "aftermath residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldRejectSimpleImmunity(false),
                "no immunity must proceed past simple-immunity gate") &&
         ok;

    // Orthogonal siblings 3049 / 3069 / 3080 / 3100 left alone — simple
    // immunity independent of expire / tick / null-add / charm-on-pet poles.
    ok = expect(ShouldExpireEffect(true, 100, 100),
                "expire residual still holds (orthogonal sibling 3049)") &&
         ok;
    ok = expect(ShouldTickEffect(true, 0, 1),
                "tick residual still holds (orthogonal sibling 3069)") &&
         ok;
    ok = expect(ShouldRejectNullStatusEffect(true) && !ShouldRejectNullStatusEffect(false),
                "null-add residual still holds (orthogonal sibling 3080)") &&
         ok;
    ok = expect(ShouldBlockCharmOnPet(true, true) && !ShouldBlockCharmOnPet(false, true),
                "charm-on-pet residual still holds (orthogonal sibling 3100)") &&
         ok;

    // Explicit dual-wire poles across dense 2¹.
    for (const bool hasImmunity : { false, true })
    {
        const bool got = ShouldRejectSimpleImmunity(hasImmunity);
        ok             = expect(got == hasImmunity, "host inject dual-wire pin") && ok;
        ok             = expect(got == inlineShouldRejectSimpleImmunity(hasImmunity),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldExpireEffect(true, 100, 100),
                    "expire residual flipped under simple-immunity compose") &&
             ok;
        ok = expect(ShouldTickEffect(true, 0, 1),
                    "tick residual flipped under simple-immunity compose") &&
             ok;
        ok = expect(ShouldRejectNullStatusEffect(true),
                    "null-add residual flipped under simple-immunity compose") &&
             ok;
        ok = expect(ShouldBlockCharmOnPet(true, true),
                    "charm-on-pet residual flipped under simple-immunity compose") &&
             ok;
    }

    return ok;
}
