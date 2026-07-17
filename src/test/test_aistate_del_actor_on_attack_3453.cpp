#include "test_aistate_del_actor_on_attack_3453.h"

#include "map/action_effect_flags_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aistate ShouldDelActorOnAttack 3453 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline processActionEffectFlags actor ON_ATTACK gate for dual-wire
// cross-check (dedicated 3453 expand residual 3044):
//   emittedHostile
auto inlineShouldDelActorOnAttack3453(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

// Compact dual-wire pin matching Go pinShouldDelActorOnAttack3453 / C++ capacity
// identity form (formula unchanged from 1631 / 3044 / 3178 / 3401):
//   emittedHostile
auto pinShouldDelActorOnAttack3453(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

} // namespace

// Pure dual-wire expansion for actioneffectflagshelpers::ShouldDelActorOnAttack
// (emittedHostile identity → actor OnAttack strip;
// OmegaXI internal/aistate; dedicated slice 3453 expand residual 3044;
// prior dedicated 3401 / 3178; pure 1631 / 1709 / 2306). Formula unchanged.
//
// Coverage:
//   - free == inline == pin
//   - residual 3044 / 1631 / prior 3178 / prior 3401 pins still hold
//   - dense 2¹ boolean space
//   - host inject processActionEffectFlags path semantics
//   - residual sibling ShouldDelActorAttackFlag no-emit pin
auto runAistateDelActorOnAttack3453SelfTests() -> bool
{
    using actioneffectflagshelpers::ShouldDelActorAttackFlag;
    using actioneffectflagshelpers::ShouldDelActorOnAttack;
    using actioneffectflagshelpers::ActionCategoryBasicAttack;

    bool ok = true;

    // Residual 1631 / 3044 / prior 3178 / prior 3401 pins still hold under dedicated dual-wire.
    ok = expect(ShouldDelActorOnAttack(true), "residual 1631/3044/3178/3401: emittedHostile true → del") && ok;
    ok = expect(!ShouldDelActorOnAttack(false), "residual 1631/3044/3178/3401: emittedHostile false → no del") && ok;

    // --- Core poles: free == inline == pin ---
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
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDelActorOnAttack(c.emittedHostile);
        const bool inlineF = inlineShouldDelActorOnAttack3453(c.emittedHostile);
        const bool pin     = pinShouldDelActorOnAttack3453(c.emittedHostile);
        // Positive form pin composition (explicit identity).
        const bool wantPin = c.emittedHostile;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldDelActorOnAttack free == inline == pin") &&
             ok;
    }

    // Free == pin across residual poles + identity composition.
    ok = expect(ShouldDelActorOnAttack(true), "emittedHostile true must del") && ok;
    ok = expect(ShouldDelActorOnAttack(true) == pinShouldDelActorOnAttack3453(true),
                "free == pin for emittedHostile true") &&
         ok;
    ok = expect(!ShouldDelActorOnAttack(false), "emittedHostile false must not del") && ok;
    ok = expect(ShouldDelActorOnAttack(false) == pinShouldDelActorOnAttack3453(false),
                "free == pin for emittedHostile false") &&
         ok;

    // Dense compose: full 2¹ boolean space free == inline == pin.
    for (const bool emitted : { false, true })
    {
        const bool got     = ShouldDelActorOnAttack(emitted);
        const bool inlineF = inlineShouldDelActorOnAttack3453(emitted);
        const bool pin     = pinShouldDelActorOnAttack3453(emitted);
        const bool want    = emitted;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
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
    // free == inline == pin for host inject poles.
    for (const bool emitted : { false, true })
    {
        const bool got     = ShouldDelActorOnAttack(emitted);
        const bool inlineF = inlineShouldDelActorOnAttack3453(emitted);
        const bool pin     = pinShouldDelActorOnAttack3453(emitted);
        ok                 = expect(got == emitted, "host inject identity") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Residual sibling ShouldDelActorAttackFlag remains available and is not
    // this dual-wire surface (physical category ATTACK strip).
    // When no hostile emit, both gates are false (independent residual pin).
    ok = expect(!ShouldDelActorAttackFlag(false, ActionCategoryBasicAttack),
                "sibling residual: no emit must not strip ATTACK either") &&
         ok;

    return ok;
}
