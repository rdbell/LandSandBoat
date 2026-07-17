#include "test_ranger_short_circuit_additional_effects_3203.h"

#include "map/ranged_additional_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranger ShouldShortCircuitAdditionalEffects 3203 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack checkAddEffect dead-target formula for dual-wire
// cross-check (dedicated 3203):
//   targetIsDead
auto inlineShouldShortCircuitAdditionalEffects3203(const bool targetIsDead) -> bool
{
    return targetIsDead;
}

// Compact dual-wire pin matching Go pinShouldShortCircuitAdditionalEffects3203 /
// C++ capacity form (formula unchanged from 1392 / 2950 — identity):
//   targetIsDead
auto pinShouldShortCircuitAdditionalEffects3203(const bool targetIsDead) -> bool
{
    return targetIsDead;
}

} // namespace

// Pure dual-wire expansion for
// rangedadditionaleffecthelpers::ShouldShortCircuitAdditionalEffects
// (targetIsDead identity; OmegaXI internal/ranger; dedicated slice 3203;
// residual expand 2950 / pure 1392).
//
// Coverage:
//   - free == inline == pin
//   - residual 2950 / 1392 pins still hold
//   - poles true/false (dead/live)
//   - host inject poles (GetHPP()==0)
//
// Siblings left alone: camouflage retain suite (3174 etc.).
// Not registered in CMake/main.
auto runRangerShortCircuitAdditionalEffects3203SelfTests() -> bool
{
    using rangedadditionaleffecthelpers::HasConflictingAdditionalEffectConfig;
    using rangedadditionaleffecthelpers::ShouldShortCircuitAdditionalEffects;
    using rangedadditionaleffecthelpers::ShouldTryGlobalAdditionalEffect;

    bool ok = true;

    // Residual 1392 pins still hold under dedicated dual-wire.
    ok = expect(ShouldShortCircuitAdditionalEffects(true), "residual 1392: dead target must short-circuit") && ok;
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "residual 1392: live target must not short-circuit") && ok;

    // Residual 2950 poles still hold under dedicated dual-wire.
    ok = expect(ShouldShortCircuitAdditionalEffects(true), "residual 2950: dead short-circuits") && ok;
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "residual 2950: live continues AE dispatch") && ok;

    // --- Core poles: free == inline == pin (required dead/live + stability) ---
    const struct
    {
        bool        targetIsDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dedicated poles.
        { true, true, "pole dead short-circuits" },
        { false, false, "pole live continues AE dispatch" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity dead" },
        { false, false, "identity live" },

        // Residual 1392 / 2950 re-pins.
        { true, true, "residual dead short-circuit" },
        { false, false, "residual live continues" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldShortCircuitAdditionalEffects(c.targetIsDead);
        const bool inlineF = inlineShouldShortCircuitAdditionalEffects3203(c.targetIsDead);
        const bool pin     = pinShouldShortCircuitAdditionalEffects3203(c.targetIsDead);
        // Identity form pin composition (explicit targetIsDead).
        const bool wantPin = c.targetIsDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldShortCircuitAdditionalEffects free == inline == pin") &&
             ok;
    }

    // Free == pin across required poles true/false.
    for (const bool targetIsDead : { false, true })
    {
        const bool got     = ShouldShortCircuitAdditionalEffects(targetIsDead);
        const bool pin     = pinShouldShortCircuitAdditionalEffects3203(targetIsDead);
        const bool inlineF = inlineShouldShortCircuitAdditionalEffects3203(targetIsDead);
        const bool want    = targetIsDead;
        ok                 = expect(got == want, "required pole free == pin formula") && ok;
        ok                 = expect(got == pin && got == inlineF, "required pole free == inline == pin") && ok;
    }

    // Dense compose over full 2^1 boolean space: free == inline == pin.
    for (const bool targetIsDead : { false, true })
    {
        const bool got     = ShouldShortCircuitAdditionalEffects(targetIsDead);
        const bool inlineF = inlineShouldShortCircuitAdditionalEffects3203(targetIsDead);
        const bool pin     = pinShouldShortCircuitAdditionalEffects3203(targetIsDead);
        const bool want    = targetIsDead;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Host-style inject poles: targetIsDead = PTarget->GetHPP() == 0.
    // Production OnRangedAttack checkAddEffect:
    //   if ShouldShortCircuitAdditionalEffects(targetIsDead) → return true
    //     (skip global / item-script AE attempts)
    //   else continue AE dispatch
    const struct
    {
        bool        targetIsDead;
        bool        wantShort;
        const char* label;
    } hostPoles[] = {
        { true, true, "GetHPP()==0 → short-circuit AE path" },
        { false, false, "GetHPP()!=0 → continue AE dispatch path" },
    };
    for (const auto& h : hostPoles)
    {
        const bool got     = ShouldShortCircuitAdditionalEffects(h.targetIsDead);
        const bool inlineF = inlineShouldShortCircuitAdditionalEffects3203(h.targetIsDead);
        const bool pin     = pinShouldShortCircuitAdditionalEffects3203(h.targetIsDead);
        ok                 = expect(got == h.wantShort, h.label) && ok;
        ok                 = expect(got == h.targetIsDead, "host inject free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production inject pin: checkAddEffect short-circuit iff dead.
    const bool shortDead    = ShouldShortCircuitAdditionalEffects(true);
    const bool continueLive = !ShouldShortCircuitAdditionalEffects(false);
    ok                      = expect(shortDead && continueLive, "checkAddEffect short/continue polarity") && ok;
    ok                      = expect(shortDead == inlineShouldShortCircuitAdditionalEffects3203(true) &&
                    shortDead == pinShouldShortCircuitAdditionalEffects3203(true),
                "dead free == inline/pin") &&
         ok;
    ok = expect(ShouldShortCircuitAdditionalEffects(false) == inlineShouldShortCircuitAdditionalEffects3203(false) &&
                    ShouldShortCircuitAdditionalEffects(false) == pinShouldShortCircuitAdditionalEffects3203(false),
                "live free == inline/pin") &&
         ok;

    // Live path still reaches residual AE policy siblings (1392).
    ok = expect(!ShouldShortCircuitAdditionalEffects(false), "live target must reach AE policy siblings") && ok;
    ok = expect(HasConflictingAdditionalEffectConfig(true, true), "sibling conflict pin still holds") && ok;
    ok = expect(ShouldTryGlobalAdditionalEffect(true), "sibling global try pin still holds") && ok;

    return ok;
}
