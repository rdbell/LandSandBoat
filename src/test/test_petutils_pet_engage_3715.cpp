#include "test_petutils_pet_engage_3715.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petutils ShouldPetEngage 3715 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AttackTarget engage formula for dual-wire cross-check (dedicated 3715
// expand residual 3051; prior dedicated 3670 / 3625 / 3580 / 3536 / 3484 / 3429 / 3375):
//   !hasPreventActionEffect
auto inlineShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Compact dual-wire pin matching Go pinShouldPetEngage3715 / C++ capacity:
//   !hasPreventActionEffect
auto pinShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3375 inline/pin for independence cross-check.
auto inlineShouldPetEngage3375(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto pinShouldPetEngage3375(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3429 inline/pin for independence cross-check.
auto inlineShouldPetEngage3429(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto pinShouldPetEngage3429(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3484 inline/pin for independence cross-check.
auto inlineShouldPetEngage3484(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto pinShouldPetEngage3484(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3536 inline/pin for independence cross-check.
auto inlineShouldPetEngage3536(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto pinShouldPetEngage3536(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3580 inline/pin for independence cross-check.
auto pinShouldPetEngage3580(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto inlineShouldPetEngage3580(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3625 inline/pin for independence cross-check.
auto pinShouldPetEngage3625(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto inlineShouldPetEngage3625(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

// Prior dedicated 3670 pin for free == inline == pin == pin3670 cross-check.
auto pinShouldPetEngage3670(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

auto inlineShouldPetEngage3670(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::ShouldPetEngage
// (!HasPreventActionEffect on AttackTarget; OmegaXI internal/petutils;
// dedicated slice 3715 expand residual 3051; prior dedicated 3670 / 3625 / 3580 / 3536 / 3484 / 3429 / 3375; pure 1627).
//
// Coverage:
//   - free == inline == pin == pin3670 == !hasPreventActionEffect
//   - residual 1627 / 3051 pins still hold
//   - prior 3375 dedicated poles still hold
//   - prior 3429 dedicated poles still hold
//   - prior 3484 dedicated poles still hold
//   - prior 3536 dedicated poles still hold
//   - prior 3580 dedicated poles still hold
//   - prior 3625 dedicated poles still hold
//   - prior 3670 dedicated poles still hold
//   - dense 2^1 boolean space
// Formula unchanged — not registered in CMake/main.
auto runPetutilsPetEngage3715SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 / 3051 pins still hold under dual-wire.
    ok = expect(ShouldPetEngage(false), "residual no prevent-action → engage") && ok;
    ok = expect(!ShouldPetEngage(true), "residual prevent-action → skip engage") && ok;

    // --- Composition table: free == inline == pin == pin3670 ---
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

        // Prior dedicated 3375 poles still hold.
        { false, true, "prior 3375 no prevent-action" },
        { true, false, "prior 3375 prevent-action" },

        // Prior dedicated 3429 poles still hold.
        { false, true, "prior 3429 no prevent-action" },
        { true, false, "prior 3429 prevent-action" },

        // Prior dedicated 3484 poles still hold.
        { false, true, "prior 3484 no prevent-action" },
        { true, false, "prior 3484 prevent-action" },

        // Prior dedicated 3536 poles still hold.
        { false, true, "prior 3536 no prevent-action" },
        { true, false, "prior 3536 prevent-action" },

        // Prior dedicated 3580 poles still hold.
        { false, true, "prior 3580 no prevent-action" },
        { true, false, "prior 3580 prevent-action" },

        // Prior dedicated 3625 poles still hold.
        { false, true, "prior 3625 no prevent-action" },
        { true, false, "prior 3625 prevent-action" },

        // Prior dedicated 3670 poles still hold.
        { false, true, "prior 3670 no prevent-action" },
        { true, false, "prior 3670 prevent-action" },

        // Polarity / dual-wire stability repeats.
        { false, true, "polarity engage when clear" },
        { true, false, "polarity skip when blocked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPetEngage(c.hasPreventActionEffect);
        const bool inlineF = inlineShouldPetEngage(c.hasPreventActionEffect);
        const bool pin     = pinShouldPetEngage(c.hasPreventActionEffect);
        const bool pin3670 = pinShouldPetEngage3670(c.hasPreventActionEffect);
        const bool wantPin = !c.hasPreventActionEffect;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin == pin3670.
        ok = expect(got == inlineF && got == pin && got == pin3670,
                    "dual-wire free == inline == pin == pin3670") &&
             ok;
        ok = expect(got == wantPin, "free == pin formula !hasPreventActionEffect") && ok;
    }

    // Free == pin == pin3670 across residual poles.
    ok = expect(ShouldPetEngage(false) == pinShouldPetEngage(false), "free == pin no prevent") && ok;
    ok = expect(ShouldPetEngage(true) == pinShouldPetEngage(true), "free == pin prevent") && ok;
    ok = expect(ShouldPetEngage(false) == pinShouldPetEngage3670(false), "free == pin3670 no prevent") && ok;
    ok = expect(ShouldPetEngage(true) == pinShouldPetEngage3670(true), "free == pin3670 prevent") && ok;

    // Prior dedicated 3375 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3375(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3375(false),
                "prior 3375 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3375(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3375(true),
                "prior 3375 independence prevent") &&
         ok;

    // Prior dedicated 3429 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3429(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3429(false),
                "prior 3429 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3429(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3429(true),
                "prior 3429 independence prevent") &&
         ok;

    // Prior dedicated 3484 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3484(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3484(false),
                "prior 3484 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3484(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3484(true),
                "prior 3484 independence prevent") &&
         ok;

    // Prior dedicated 3536 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3536(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3536(false),
                "prior 3536 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3536(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3536(true),
                "prior 3536 independence prevent") &&
         ok;

    // Prior dedicated 3580 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3580(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3580(false),
                "prior 3580 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3580(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3580(true),
                "prior 3580 independence prevent") &&
         ok;

    // Prior dedicated 3625 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3625(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3625(false),
                "prior 3625 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3625(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3625(true),
                "prior 3625 independence prevent") &&
         ok;

    // Prior dedicated 3670 independence: free still matches prior inline/pin.
    ok = expect(ShouldPetEngage(false) == inlineShouldPetEngage3670(false) &&
                    ShouldPetEngage(false) == pinShouldPetEngage3670(false),
                "prior 3670 independence no prevent") &&
         ok;
    ok = expect(ShouldPetEngage(true) == inlineShouldPetEngage3670(true) &&
                    ShouldPetEngage(true) == pinShouldPetEngage3670(true),
                "prior 3670 independence prevent") &&
         ok;

    // Pin composition: engage iff !hasPreventActionEffect.
    ok = expect(ShouldPetEngage(false), "hasPreventActionEffect false must engage") && ok;
    ok = expect(!ShouldPetEngage(true), "hasPreventActionEffect true must not engage") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == pin3670.
    for (const bool hasPrevent : { false, true })
    {
        const bool got     = ShouldPetEngage(hasPrevent);
        const bool inlineF = inlineShouldPetEngage(hasPrevent);
        const bool pin     = pinShouldPetEngage(hasPrevent);
        const bool pin3670 = pinShouldPetEngage3670(hasPrevent);
        const bool want    = !hasPrevent;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3670,
                    "compose free == inline == pin == pin3670") &&
             ok;
    }

    // Host-style inject poles: free == inline == pin == pin3670 for AttackTarget.
    for (const bool hasPrevent : { false, true })
    {
        const bool got     = ShouldPetEngage(hasPrevent);
        const bool inlineF = inlineShouldPetEngage(hasPrevent);
        const bool pin     = pinShouldPetEngage(hasPrevent);
        const bool pin3670 = pinShouldPetEngage3670(hasPrevent);
        const bool inject  = !hasPrevent;
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3670,
                    "host inject free == inline == pin == pin3670") &&
             ok;
        ok = expect(!(got && hasPrevent), "engage only when !hasPrevent") && ok;
        ok = expect(!(!got && !hasPrevent), "!hasPrevent must engage") && ok;
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
