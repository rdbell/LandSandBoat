#include "test_pet_should_disengage_3081.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldPetDisengage 3081 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RetreatToMaster disengage formula for dual-wire cross-check
// (slice 3081):
//   !hasPreventActionEffect
auto inlineShouldPetDisengage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::ShouldPetDisengage
// (!HasPreventActionEffect on RetreatToMaster; slice 3081). Dense 2¹ boolean
// space.
auto runPetShouldDisengage3081SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::CanRetreatToMaster;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 pins still hold under dual-wire.
    ok = expect(ShouldPetDisengage(false), "residual no prevent-action → disengage") && ok;
    ok = expect(!ShouldPetDisengage(true), "residual prevent-action → skip disengage") && ok;

    const struct
    {
        bool        hasPreventActionEffect;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "no prevent-action disengages" },
        { true, false, "prevent-action skips disengage" },

        // Residual 1627 pins.
        { false, true, "residual ShouldPetDisengage(false)" },
        { true, false, "residual ShouldPetDisengage(true)" },

        // Polarity / dual-wire stability repeats.
        { false, true, "polarity disengage when clear" },
        { true, false, "polarity skip when blocked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPetDisengage(c.hasPreventActionEffect);
        const bool inlineF = inlineShouldPetDisengage(c.hasPreventActionEffect);
        const bool wantPin = !c.hasPreventActionEffect;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPetDisengage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPetDisengage == pin formula !hasPreventActionEffect") && ok;
    }

    // Pin composition: disengage iff !hasPreventActionEffect.
    ok = expect(ShouldPetDisengage(false), "hasPreventActionEffect false must disengage") && ok;
    ok = expect(!ShouldPetDisengage(true), "hasPreventActionEffect true must not disengage") && ok;

    // Dense compose: full 2¹ boolean space.
    for (const bool hasPrevent : { false, true })
    {
        const bool got  = ShouldPetDisengage(hasPrevent);
        const bool want = !hasPrevent;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldPetDisengage(hasPrevent), "compose free == inline") && ok;
    }

    // Explicit polarity: disengage iff NOT prevent-action.
    for (const bool has : { false, true })
    {
        const bool got = ShouldPetDisengage(has);
        ok             = expect(got == !has, "polarity: disengage == !hasPrevent") && ok;
        ok             = expect(!(got && has), "polarity: must not disengage when prevent-action true") && ok;
        ok             = expect(!(!got && !has), "polarity: must disengage when prevent-action false") && ok;
    }

    // Host-style inject poles: HasPreventActionEffect as bool on RetreatToMaster.
    for (const bool hasPrevent : { false, true })
    {
        const bool disengage = ShouldPetDisengage(hasPrevent);
        ok                   = expect(disengage == !hasPrevent, "host inject dual-wire polarity") && ok;
        ok                   = expect(disengage == inlineShouldPetDisengage(hasPrevent), "host inject free == inline") && ok;
        ok                   = expect(!(disengage && hasPrevent), "disengage only when !hasPrevent") && ok;
        ok                   = expect(!(!disengage && !hasPrevent), "!hasPrevent must disengage") && ok;
    }

    // Production RetreatToMaster path semantics:
    // !HasPreventActionEffect → Disengage; else skip.
    ok = expect(ShouldPetDisengage(false), "RetreatToMaster no prevent-action → disengage path") && ok;
    ok = expect(!ShouldPetDisengage(true), "RetreatToMaster prevent-action → skip-disengage path") && ok;

    // Sibling residual CanRetreatToMaster (1627) is orthogonal: null preflight
    // runs first; free disengage polarity does not depend on master/pet.
    // CanRetreatToMaster(m,p) = m && p.
    ok = expect(CanRetreatToMaster(true, true), "residual CanRetreatToMaster admit when master+pet") && ok;
    ok = expect(!CanRetreatToMaster(false, true), "residual CanRetreatToMaster reject when master null") && ok;
    ok = expect(!CanRetreatToMaster(true, false), "residual CanRetreatToMaster reject when pet null") && ok;
    for (const bool hasPrevent : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool hasPet : { false, true })
            {
                // Orthogonal: disengage iff !hasPrevent; preflight iff master+pet.
                ok = expect(ShouldPetDisengage(hasPrevent) == !hasPrevent,
                            "disengage vs preflight compose hasPrevent") &&
                     ok;
                const bool wantAdmit = hasMaster && hasPet;
                ok                   = expect(CanRetreatToMaster(hasMaster, hasPet) == wantAdmit,
                            "disengage vs preflight compose admit") &&
                     ok;
                const bool wouldDisengage = wantAdmit && !hasPrevent;
                const bool gotDisengage =
                    CanRetreatToMaster(hasMaster, hasPet) && ShouldPetDisengage(hasPrevent);
                ok = expect(gotDisengage == wouldDisengage, "RetreatToMaster path both gates") && ok;
            }
        }
    }

    // Sibling dual-wire ShouldPetEngage (3051) shares the same formula shape
    // but is AttackTarget's gate (leave 3051 surface alone).
    for (const bool hasPrevent : { false, true })
    {
        ok = expect(ShouldPetEngage(hasPrevent) == !hasPrevent,
                    "sibling ShouldPetEngage polarity") &&
             ok;
        // Dual-wire disengage and dual-wire engage share !hasPrevent polarity.
        ok = expect(ShouldPetDisengage(hasPrevent) == ShouldPetEngage(hasPrevent),
                    "disengage vs sibling engage polarity") &&
             ok;
    }

    // Sibling dual-wire CanAttackTarget (3071) is orthogonal AttackTarget
    // null preflight (leave alone).
    ok = expect(CanAttackTarget(true, true, true), "sibling CanAttackTarget admit when all present") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "sibling CanAttackTarget reject when master null") && ok;

    // Explicit dual-wire poles: free == !hasPrevent for both bools.
    for (const bool hasPrevent : { false, true })
    {
        const bool got = ShouldPetDisengage(hasPrevent);
        ok             = expect(got == !hasPrevent, "host inject dual-wire polarity poles") && ok;
        ok             = expect(got == inlineShouldPetDisengage(hasPrevent), "host inject free == inline poles") && ok;
    }

    return ok;
}
