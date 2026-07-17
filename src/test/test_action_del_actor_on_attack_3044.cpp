#include "test_action_del_actor_on_attack_3044.h"

#include "map/action_effect_flags_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "action ShouldDelActorOnAttack 3044 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline processActionEffectFlags actor ON_ATTACK gate for dual-wire
// cross-check (slice 3044):
//   emittedHostile
auto inlineShouldDelActorOnAttack(const bool emittedHostile) -> bool
{
    return emittedHostile;
}

} // namespace

// Pure dual-wire expansion for actioneffectflagshelpers::ShouldDelActorOnAttack
// (emittedHostile identity → actor OnAttack strip; slice 3044).
// Dense 2¹ boolean space. Residual 1631 / 1709 / 2306 pins still hold.
auto runActionDelActorOnAttack3044SelfTests() -> bool
{
    using actioneffectflagshelpers::ShouldDelActorAttackFlag;
    using actioneffectflagshelpers::ShouldDelActorOnAttack;
    using actioneffectflagshelpers::ActionCategoryBasicAttack;

    bool ok = true;

    // Residual 1631 pins still hold under dual-wire.
    ok = expect(ShouldDelActorOnAttack(true), "residual emittedHostile true → del") && ok;
    ok = expect(!ShouldDelActorOnAttack(false), "residual emittedHostile false → no del") && ok;

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
        { true, true, "residual actor on attack true" },
        { false, false, "residual actor on attack false" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity true" },
        { false, false, "identity false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDelActorOnAttack(c.emittedHostile);
        const bool inlineF = inlineShouldDelActorOnAttack(c.emittedHostile);
        const bool wantPin = c.emittedHostile;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDelActorOnAttack dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDelActorOnAttack == pin formula emittedHostile") && ok;
    }

    // Pin composition: identity with emittedHostile only.
    ok = expect(ShouldDelActorOnAttack(true), "emittedHostile true must del") && ok;
    ok = expect(!ShouldDelActorOnAttack(false), "emittedHostile false must not del") && ok;

    // Dense compose: full 2¹ boolean space free == inline == pin.
    for (const bool emitted : { false, true })
    {
        const bool got  = ShouldDelActorOnAttack(emitted);
        const bool want = emitted;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDelActorOnAttack(emitted), "compose free == inline") && ok;
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
    for (const bool emitted : { false, true })
    {
        ok = expect(ShouldDelActorOnAttack(emitted) == emitted, "host inject identity") && ok;
        ok = expect(ShouldDelActorOnAttack(emitted) == inlineShouldDelActorOnAttack(emitted),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Residual sibling ShouldDelActorAttackFlag remains available and is not
    // this dual-wire surface (physical category ATTACK strip).
    // When no hostile emit, both gates are false (independent residual pin).
    ok = expect(!ShouldDelActorAttackFlag(false, ActionCategoryBasicAttack),
                "sibling residual: no emit must not strip ATTACK either") &&
         ok;

    return ok;
}
