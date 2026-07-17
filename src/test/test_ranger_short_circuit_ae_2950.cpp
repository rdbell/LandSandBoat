#include "test_ranger_short_circuit_ae_2950.h"

#include "map/ranged_additional_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldShortCircuitAdditionalEffects 2950 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack checkAddEffect dead-target formula for dual-wire
// cross-check (slice 2950):
//   targetIsDead
auto inlineShouldShortCircuitAdditionalEffects(const bool targetIsDead) -> bool
{
    return targetIsDead;
}

} // namespace

// Pure dual-wire expansion for
// rangedadditionaleffecthelpers::ShouldShortCircuitAdditionalEffects
// (targetIsDead; slice 2950). Not registered in CMake/main.
auto runRangerShortCircuitAE2950SelfTests() -> bool
{
    using rangedadditionaleffecthelpers::ShouldShortCircuitAdditionalEffects;

    bool ok = true;

    const struct
    {
        bool        targetIsDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "dead target short-circuits" },
        { false, false, "live target continues AE dispatch" },

        // Residual 1392 pins.
        { true, true, "residual dead short-circuit" },
        { false, false, "residual live continues" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity dead" },
        { false, false, "identity live" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldShortCircuitAdditionalEffects(c.targetIsDead);
        const bool inlineF = inlineShouldShortCircuitAdditionalEffects(c.targetIsDead);
        const bool wantPin = c.targetIsDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldShortCircuitAdditionalEffects dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldShortCircuitAdditionalEffects == pin formula targetIsDead") && ok;
    }

    // Pin composition: identity of targetIsDead.
    ok = expect(ShouldShortCircuitAdditionalEffects(true), "dead target must short-circuit") && ok;
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "live target must not short-circuit") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool targetIsDead : { false, true })
    {
        const bool got  = ShouldShortCircuitAdditionalEffects(targetIsDead);
        const bool want = targetIsDead;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldShortCircuitAdditionalEffects(targetIsDead),
                    "compose free == inline") &&
             ok;
    }

    // --- Production OnRangedAttack checkAddEffect path semantics ---
    // Host injects:
    //   targetIsDead = PTarget->GetHPP() == 0
    // if (ShouldShortCircuitAdditionalEffects(targetIsDead)) return true;
    //   // skip global / item-script AE attempts
    ok = expect(ShouldShortCircuitAdditionalEffects(true), "OnRangedAttack dead HPP → short-circuit AE path") && ok;
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "OnRangedAttack live HPP → continue AE dispatch path") && ok;

    // Residual 1392 pins still hold under dual-wire.
    ok = expect(ShouldShortCircuitAdditionalEffects(true), "residual dead short-circuit") && ok;
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "residual live continues") && ok;

    // Host-style checkAddEffect early-return polarity: short-circuit iff dead.
    const bool shortDead    = ShouldShortCircuitAdditionalEffects(true);
    const bool continueLive = !ShouldShortCircuitAdditionalEffects(false);
    ok                      = expect(shortDead && continueLive, "checkAddEffect short/continue polarity") && ok;
    ok                      = expect(shortDead == inlineShouldShortCircuitAdditionalEffects(true),
                "dead free == inline") &&
         ok;
    ok = expect(ShouldShortCircuitAdditionalEffects(false) == inlineShouldShortCircuitAdditionalEffects(false),
                "live free == inline") &&
         ok;

    return ok;
}
