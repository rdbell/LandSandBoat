#include "test_status_can_gain_no_existing_3135.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect CanGainWhenNoExisting 3135 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CanGainStatusEffect no-existing residual formula for dual-wire
// cross-check (slice 3135):
//   true
auto inlineCanGainWhenNoExisting() -> bool
{
    return true;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::CanGainWhenNoExisting
// (constant true; slice 3135). Dense space: zero-arg constant.
auto runStatusCanGainNoExisting3135SelfTests() -> bool
{
    using statuseffecthelpers::CanGainOverwrite;
    using statuseffecthelpers::CanGainWhenNoExisting;
    using statuseffecthelpers::OverwriteAlways;
    using statuseffecthelpers::ShouldBlockByBlockId;
    using statuseffecthelpers::ShouldBlockCharmOnPet;
    using statuseffecthelpers::ShouldBlockSpikesDueToAftermath;
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldRejectNullStatusEffect;
    using statuseffecthelpers::ShouldRejectSimpleImmunity;
    using statuseffecthelpers::ShouldRejectSleepImmunity;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1364 pin.
    ok = expect(CanGainWhenNoExisting(), "residual: no existing must allow") && ok;

    // Pin formula: CanGainWhenNoExisting() = true
    {
        const bool got     = CanGainWhenNoExisting();
        const bool inlineF = inlineCanGainWhenNoExisting();

        ok = expect(got, "CanGainWhenNoExisting() want true") && ok;
        ok = expect(got == inlineF, "CanGainWhenNoExisting dual-wire == inline LSB formula") && ok;
        ok = expect(got == true, "free == constant true") && ok;
    }

    // Host path: CStatusEffectContainer::CanGainStatusEffect when no same-ID.
    ok = expect(CanGainWhenNoExisting(), "no same-ID existing → allow CanGain") && ok;
    ok = expect(inlineCanGainWhenNoExisting(), "inline residual allow still holds") && ok;

    // Dense compose: free == inline == pin formula (constant true).
    for (int i = 0; i < 3; ++i)
    {
        const bool got = CanGainWhenNoExisting();
        ok             = expect(got == true, "compose free == pin formula") && ok;
        ok             = expect(got == inlineCanGainWhenNoExisting(),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1364): no-existing allow distinct from overwrite /
    // sleep / charm / block-id / aftermath / simple-immunity gates.
    ok = expect(CanGainWhenNoExisting(),
                "no existing must allow via CanGainWhenNoExisting") &&
         ok;
    ok = expect(CanGainOverwrite(OverwriteAlways, 0, 0, 1, 100),
                "overwrite residual still holds under dual-wire") &&
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
    ok = expect(ShouldRejectSimpleImmunity(true) && !ShouldRejectSimpleImmunity(false),
                "simple-immunity residual still holds under dual-wire") &&
         ok;

    // Orthogonal siblings 3049 / 3069 / 3080 / 3100 / 3113 left alone —
    // no-existing allow independent of expire / tick / null-add / charm /
    // simple-immunity poles.
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
    ok = expect(ShouldRejectSimpleImmunity(true) && !ShouldRejectSimpleImmunity(false),
                "simple-immunity residual still holds (orthogonal sibling 3113)") &&
         ok;

    // Explicit dual-wire poles across repeated constant true.
    for (int i = 0; i < 2; ++i)
    {
        const bool got = CanGainWhenNoExisting();
        ok             = expect(got == true, "host inject dual-wire pin") && ok;
        ok             = expect(got == inlineCanGainWhenNoExisting(),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldExpireEffect(true, 100, 100),
                    "expire residual flipped under no-existing compose") &&
             ok;
        ok = expect(ShouldTickEffect(true, 0, 1),
                    "tick residual flipped under no-existing compose") &&
             ok;
        ok = expect(ShouldRejectNullStatusEffect(true),
                    "null-add residual flipped under no-existing compose") &&
             ok;
        ok = expect(ShouldBlockCharmOnPet(true, true),
                    "charm-on-pet residual flipped under no-existing compose") &&
             ok;
        ok = expect(ShouldRejectSimpleImmunity(true),
                    "simple-immunity residual flipped under no-existing compose") &&
             ok;
    }

    return ok;
}
