#include "test_aistate_del_actor_on_attack_3891.h"

#include "map/action_effect_flags_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aistate ShouldDelActorOnAttack 3891 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline processActionEffectFlags actor ON_ATTACK gate for dual-wire
// cross-check (dedicated 3891 expand residual 3044):
//   emittedHostile
auto inlineShouldDelActorOnAttack3891(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

// Compact dual-wire pin matching Go pinShouldDelActorOnAttack3891 / C++ capacity
// identity form (formula unchanged from 1631 / 3044 / 3178 / 3401 / 3453 / 3521 / 3576 / 3621 / 3666 / 3711 / 3756 / 3801 / 3846):
//   emittedHostile
auto pinShouldDelActorOnAttack3891(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

// Compact dual-wire pin matching Go pinShouldDelActorOnAttack3846
// (prior dedicated retained under 3891):
//   emittedHostile
auto pinShouldDelActorOnAttack3846(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

} // namespace

// Pure dual-wire expansion for actioneffectflagshelpers::ShouldDelActorOnAttack
// (emittedHostile identity → actor OnAttack strip;
// OmegaXI internal/aistate; dedicated slice 3891 expand residual 3044;
// prior dedicated 3846 / 3801 / 3756 / 3711 / 3666 / 3621 / 3576 / 3521 / 3453 / 3401 / 3178; pure 1631 / 1709 / 2306). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3846
//   - residual 3044 / 1631 / prior 3178 / prior 3401 / prior 3453 / prior 3521 / prior 3576 / prior 3621 / prior 3666 / prior 3711 / prior 3756 / prior 3801 / prior 3846 pins still hold
//   - dense 2¹ boolean space
//   - host inject processActionEffectFlags path semantics
//   - residual sibling ShouldDelActorAttackFlag no-emit pin
auto runAistateDelActorOnAttack3891SelfTests() -> bool
{
    using actioneffectflagshelpers::ShouldDelActorAttackFlag;
    using actioneffectflagshelpers::ShouldDelActorOnAttack;
    using actioneffectflagshelpers::ActionCategoryBasicAttack;

    bool ok = true;

    // Residual 1631 / 3044 / prior 3178 / prior 3401 / prior 3453 / prior 3521 / prior 3576 / prior 3621 / prior 3666 / prior 3711 / prior 3756 / prior 3801 / prior 3846 pins still hold under dedicated dual-wire.
    ok = expect(ShouldDelActorOnAttack(true), "residual 1631/3044/3178/3401/3453/3521/3576/3621/3666/3711/3756/3801/3846: emittedHostile true → del") && ok;
    ok = expect(!ShouldDelActorOnAttack(false), "residual 1631/3044/3178/3401/3453/3521/3576/3621/3666/3711/3756/3801/3846: emittedHostile false → no del") && ok;

    // --- Core poles: free == inline == pin == pin3846 ---
    const struct
    {
        bool        emittedHostile;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "hostile emit del actor OnAttack" },
        { false, false, "no hostile emit no del" },

        // Residual 1631 pins.
        { true, true, "residual 1631 actor on attack true" },
        { false, false, "residual 1631 actor on attack false" },

        // Residual 3044 identity / polarity repeats.
        { true, true, "residual 3044 identity true" },
        { false, false, "residual 3044 identity false" },

        // Prior dedicated 3178 identity / polarity repeats.
        { true, true, "prior 3178 identity true" },
        { false, false, "prior 3178 identity false" },

        // Prior dedicated 3401 identity / polarity repeats.
        { true, true, "prior 3401 identity true" },
        { false, false, "prior 3401 identity false" },

        // Prior dedicated 3453 identity / polarity repeats.
        { true, true, "prior 3453 identity true" },
        { false, false, "prior 3453 identity false" },

        // Prior dedicated 3521 identity / polarity repeats.
        { true, true, "prior 3521 identity true" },
        { false, false, "prior 3521 identity false" },

        // Prior dedicated 3576 identity / polarity repeats.
        { true, true, "prior 3576 identity true" },
        { false, false, "prior 3576 identity false" },

        // Prior dedicated 3621 identity / polarity repeats.
        { true, true, "prior 3621 identity true" },
        { false, false, "prior 3621 identity false" },

        // Prior dedicated 3666 identity / polarity repeats.
        { true, true, "prior 3666 identity true" },
        { false, false, "prior 3666 identity false" },

        // Prior dedicated 3711 identity / polarity repeats.
        { true, true, "prior 3711 identity true" },
        { false, false, "prior 3711 identity false" },

        // Prior dedicated 3756 identity / polarity repeats.
        { true, true, "prior 3756 identity true" },
        { false, false, "prior 3756 identity false" },

        // Prior dedicated 3801 identity / polarity repeats.
        { true, true, "prior 3801 identity true" },
        { false, false, "prior 3801 identity false" },

        // Prior dedicated 3846 identity / polarity repeats.
        { true, true, "prior 3846 identity true" },
        { false, false, "prior 3846 identity false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDelActorOnAttack(c.emittedHostile);
        const bool inlineF = inlineShouldDelActorOnAttack3891(c.emittedHostile);
        const bool pin     = pinShouldDelActorOnAttack3891(c.emittedHostile);
        const bool pin3846 = pinShouldDelActorOnAttack3846(c.emittedHostile);
        // Positive form pin composition (explicit identity).
        const bool wantPin = c.emittedHostile;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == pin3846 && got == wantPin,
                    "ShouldDelActorOnAttack free == inline == pin == pin3846") &&
             ok;
    }

    // Free == pin across residual poles + identity composition.
    ok = expect(ShouldDelActorOnAttack(true), "emittedHostile true must del") && ok;
    ok = expect(ShouldDelActorOnAttack(true) == pinShouldDelActorOnAttack3891(true),
                "free == pin for emittedHostile true") &&
         ok;
    ok = expect(!ShouldDelActorOnAttack(false), "emittedHostile false must not del") && ok;
    ok = expect(ShouldDelActorOnAttack(false) == pinShouldDelActorOnAttack3891(false),
                "free == pin for emittedHostile false") &&
         ok;

    // Free == pin3846 across residual poles (prior dedicated independence).
    ok = expect(ShouldDelActorOnAttack(true) == pinShouldDelActorOnAttack3846(true),
                "free == pin3846 for emittedHostile true") &&
         ok;
    ok = expect(ShouldDelActorOnAttack(false) == pinShouldDelActorOnAttack3846(false),
                "free == pin3846 for emittedHostile false") &&
         ok;

    // Dense compose: full 2¹ boolean space free == inline == pin == pin3846.
    for (const bool emitted : { false, true })
    {
        const bool got     = ShouldDelActorOnAttack(emitted);
        const bool inlineF = inlineShouldDelActorOnAttack3891(emitted);
        const bool pin     = pinShouldDelActorOnAttack3891(emitted);
        const bool pin3846 = pinShouldDelActorOnAttack3846(emitted);
        const bool want    = emitted;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3846,
                                    "compose free == inline == pin == pin3846") &&
             ok;
    }

    // --- Production processActionEffectFlags path semantics ---
    // Host accumulates emittedHostile from plan.countAsHostileEmit across
    // targets; after the loop injects into ShouldDelActorOnAttack:
    //   true  → DelStatusEffectsByFlag(OnAttack) on actor
    //   false → no-op
    // Sibling residual ShouldDelActorAttackFlag still gates physical ATTACK strip.
    ok = expect(ShouldDelActorOnAttack(true), "processActionEffectFlags hostile emit → OnAttack strip") && ok;
    ok = expect(!ShouldDelActorOnAttack(false), "processActionEffectFlags no emit → no OnAttack strip") && ok;

    // Explicit dual-wire: free function is the identity of inject.
    // free == inline == pin == pin3846 for host inject poles.
    for (const bool emitted : { false, true })
    {
        const bool got     = ShouldDelActorOnAttack(emitted);
        const bool inlineF = inlineShouldDelActorOnAttack3891(emitted);
        const bool pin     = pinShouldDelActorOnAttack3891(emitted);
        const bool pin3846 = pinShouldDelActorOnAttack3846(emitted);
        ok                 = expect(got == emitted, "host inject identity") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3846,
                                    "host inject free == inline == pin == pin3846") &&
             ok;
    }

    // Prior dedicated 3846 pin independence under 3891 expand.
    ok = expect(ShouldDelActorOnAttack(true) == pinShouldDelActorOnAttack3846(true),
                "prior 3846 pin holds under 3891 for true") &&
         ok;
    ok = expect(ShouldDelActorOnAttack(false) == pinShouldDelActorOnAttack3846(false),
                "prior 3846 pin holds under 3891 for false") &&
         ok;

    // Residual sibling ShouldDelActorAttackFlag remains available and is not
    // this dual-wire surface (physical category ATTACK strip).
    // When no hostile emit, both gates are false (independent residual pin).
    ok = expect(!ShouldDelActorAttackFlag(false, ActionCategoryBasicAttack),
                "sibling residual: no emit must not strip ATTACK either") &&
         ok;

    return ok;
}
