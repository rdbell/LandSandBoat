#include "test_status_block_charm_pet_3100.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldBlockCharmOnPet 3100 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CanGainStatusEffect charm-on-pet formula for dual-wire cross-check (slice 3100):
//   isCharmEffect && hasMaster
auto inlineShouldBlockCharmOnPet(const bool isCharmEffect, const bool hasMaster) -> bool
{
    return isCharmEffect && hasMaster;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldBlockCharmOnPet
// (isCharmEffect && hasMaster; slice 3100). Dense space: full 2².
auto runStatusBlockCharmPet3100SelfTests() -> bool
{
    using statuseffecthelpers::ShouldBlockByBlockId;
    using statuseffecthelpers::ShouldBlockCharmOnPet;
    using statuseffecthelpers::ShouldBlockSpikesDueToAftermath;
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldRejectNullStatusEffect;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1364 truth-table pins.
    ok = expect(ShouldBlockCharmOnPet(true, true), "residual: charm + master blocks") && ok;
    ok = expect(!ShouldBlockCharmOnPet(true, false), "residual: charm without master proceeds") && ok;

    const struct
    {
        bool        isCharmEffect;
        bool        hasMaster;
        bool        want;
        const char* label;
    } cases[] = {
        { false, false, false, "not charm no master proceeds" },
        { false, true, false, "not charm with master proceeds" },
        { true, false, false, "charm no master proceeds" },
        { true, true, true, "charm + master blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBlockCharmOnPet(c.isCharmEffect, c.hasMaster);
        const bool inlineF = inlineShouldBlockCharmOnPet(c.isCharmEffect, c.hasMaster);
        const bool wantPin = c.isCharmEffect && c.hasMaster;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldBlockCharmOnPet dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "free == isCharmEffect && hasMaster") && ok;
    }

    // Pin composition: block only when both poles are true.
    ok = expect(ShouldBlockCharmOnPet(true, true), "charm + master must block") && ok;
    ok = expect(!ShouldBlockCharmOnPet(true, false), "charm without master must not block") && ok;
    ok = expect(!ShouldBlockCharmOnPet(false, true), "non-charm with master must not block") && ok;
    ok = expect(!ShouldBlockCharmOnPet(false, false), "non-charm without master must not block") && ok;

    // Host path: CStatusEffectContainer::CanGainStatusEffect before later gates.
    const struct
    {
        bool        isCharmEffect;
        bool        hasMaster;
        bool        wantBlock;
        const char* label;
    } hostCases[] = {
        { true, true, true, "CharmI/Ii + PMaster → reject CanGain" },
        { true, false, false, "CharmI/Ii without master → proceed past pet charm gate" },
        { false, true, false, "non-charm with master → proceed (not charm gate)" },
        { false, false, false, "non-charm without master → proceed" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldBlockCharmOnPet(c.isCharmEffect, c.hasMaster);
        const bool inlineF = inlineShouldBlockCharmOnPet(c.isCharmEffect, c.hasMaster);

        ok = expect(got == c.wantBlock, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == (c.isCharmEffect && c.hasMaster),
                    "host compose free == isCharmEffect && hasMaster") &&
             ok;
    }

    // Dense compose: free == inline == pin formula across full 2².
    for (const bool isCharmEffect : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            const bool got  = ShouldBlockCharmOnPet(isCharmEffect, hasMaster);
            const bool want = isCharmEffect && hasMaster;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldBlockCharmOnPet(isCharmEffect, hasMaster),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (1364): charm-on-pet distinct from block-id /
    // aftermath spike gates.
    ok = expect(ShouldBlockCharmOnPet(true, true),
                "charm + master must block via ShouldBlockCharmOnPet") &&
         ok;
    ok = expect(ShouldBlockByBlockId(5, true) && !ShouldBlockByBlockId(0, true),
                "block-id residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldBlockSpikesDueToAftermath(true, 8) && !ShouldBlockSpikesDueToAftermath(true, 7),
                "aftermath residual still holds under dual-wire") &&
         ok;
    ok = expect(!ShouldBlockCharmOnPet(true, false),
                "charm without master must proceed past pet charm gate") &&
         ok;
    ok = expect(!ShouldBlockCharmOnPet(false, true),
                "non-charm with master must proceed past pet charm gate") &&
         ok;

    // Orthogonal siblings 3049 / 3069 / 3080 left alone — charm-on-pet
    // independent of expire / tick / null-add poles.
    ok = expect(ShouldExpireEffect(true, 100, 100),
                "expire residual still holds (orthogonal sibling 3049)") &&
         ok;
    ok = expect(ShouldTickEffect(true, 0, 1),
                "tick residual still holds (orthogonal sibling 3069)") &&
         ok;
    ok = expect(ShouldRejectNullStatusEffect(true) && !ShouldRejectNullStatusEffect(false),
                "null-add residual still holds (orthogonal sibling 3080)") &&
         ok;

    // Explicit dual-wire poles across dense 2².
    for (const bool isCharmEffect : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            const bool got = ShouldBlockCharmOnPet(isCharmEffect, hasMaster);
            ok             = expect(got == (isCharmEffect && hasMaster), "host inject dual-wire pin") && ok;
            ok             = expect(got == inlineShouldBlockCharmOnPet(isCharmEffect, hasMaster),
                        "host inject free == inline") &&
                 ok;
            ok = expect(ShouldExpireEffect(true, 100, 100),
                        "expire residual flipped under charm-on-pet compose") &&
                 ok;
            ok = expect(ShouldTickEffect(true, 0, 1),
                        "tick residual flipped under charm-on-pet compose") &&
                 ok;
            ok = expect(ShouldRejectNullStatusEffect(true),
                        "null-add residual flipped under charm-on-pet compose") &&
                 ok;
        }
    }

    return ok;
}
