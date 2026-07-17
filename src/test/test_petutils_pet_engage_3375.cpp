#include "test_petutils_pet_engage_3375.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petutils ShouldPetEngage 3375 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AttackTarget engage formula for dual-wire cross-check (dedicated 3375
// expand residual 3051):
//   !hasPreventActionEffect
auto inlineShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Compact dual-wire pin matching Go pinShouldPetEngage3375 / C++ capacity:
//   !hasPreventActionEffect
auto pinShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::ShouldPetEngage
// (!HasPreventActionEffect on AttackTarget; OmegaXI internal/petutils;
// dedicated slice 3375 expand residual 3051; pure 1627).
//
// Coverage:
//   - free == inline == pin == !hasPreventActionEffect
//   - residual 1627 / 3051 pins still hold
//   - dense 2^1 boolean space
auto runPetutilsPetEngage3375SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 / 3051 pins still hold under dual-wire.
    ok = expect(ShouldPetEngage(false), "residual no prevent-action → engage") && ok;
    ok = expect(!ShouldPetEngage(true), "residual prevent-action → skip engage") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        hasPreventActionEffect;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1627 / 3051 poles (full 2^1 surface).
        { false, true, "residual no prevent-action" },
        { true, false, "residual prevent-action" },

        // Classic dual poles.
        { false, true, "no prevent-action engages" },
        { true, false, "prevent-action skips engage" },

        // Host-style inject poles.
        { false, true, "host no-prevent inject" },
        { true, false, "host prevent inject" },

        // Polarity / dual-wire stability repeats.
        { false, true, "polarity engage when clear" },
        { true, false, "polarity skip when blocked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPetEngage(c.hasPreventActionEffect);
        const bool inlineF = inlineShouldPetEngage(c.hasPreventActionEffect);
        const bool pin     = pinShouldPetEngage(c.hasPreventActionEffect);
        const bool wantPin = !c.hasPreventActionEffect;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula !hasPreventActionEffect") && ok;
    }

    // Pin composition: engage iff !hasPreventActionEffect.
    ok = expect(ShouldPetEngage(false), "hasPreventActionEffect false must engage") && ok;
    ok = expect(!ShouldPetEngage(true), "hasPreventActionEffect true must not engage") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool hasPrevent : { false, true })
    {
        const bool got     = ShouldPetEngage(hasPrevent);
        const bool inlineF = inlineShouldPetEngage(hasPrevent);
        const bool pin     = pinShouldPetEngage(hasPrevent);
        const bool want    = !hasPrevent;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Host-style inject poles: free == inline == pin for AttackTarget.
    for (const bool hasPrevent : { false, true })
    {
        const bool got     = ShouldPetEngage(hasPrevent);
        const bool inlineF = inlineShouldPetEngage(hasPrevent);
        const bool pin     = pinShouldPetEngage(hasPrevent);
        const bool inject  = !hasPrevent;
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        ok                 = expect(!(got && hasPrevent), "engage only when !hasPrevent") && ok;
        ok                 = expect(!(!got && !hasPrevent), "!hasPrevent must engage") && ok;
    }

    // Production AttackTarget path semantics:
    //   !HasPreventActionEffect → Engage; else skip.
    ok = expect(ShouldPetEngage(false), "AttackTarget no prevent-action → engage path") && ok;
    ok = expect(!ShouldPetEngage(true), "AttackTarget prevent-action → skip-engage path") && ok;

    // Sibling dual-wire CanAttackTarget remains orthogonal residual
    // (null preflight; do not thrash can_attack_target surface).
    ok = expect(CanAttackTarget(true, true, true), "sibling CanAttackTarget admit") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "sibling CanAttackTarget reject master null") && ok;

    // Sibling dual-wire ShouldPetDisengage shares !hasPrevent polarity.
    for (const bool hasPrevent : { false, true })
    {
        ok = expect(ShouldPetDisengage(hasPrevent) == !hasPrevent,
                    "sibling ShouldPetDisengage polarity") &&
             ok;
        ok = expect(ShouldPetEngage(hasPrevent) == ShouldPetDisengage(hasPrevent),
                    "engage vs disengage polarity") &&
             ok;
    }

    // Explicit polarity: engage iff NOT prevent-action.
    for (const bool has : { false, true })
    {
        const bool got = ShouldPetEngage(has);
        ok             = expect(got == !has, "polarity: engage == !hasPrevent") && ok;
        ok             = expect(!(got && has), "polarity: must not engage when prevent-action true") && ok;
        ok             = expect(!(!got && !has), "polarity: must engage when prevent-action false") && ok;
    }

    return ok;
}
