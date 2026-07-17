#include "test_pet_should_engage_3051.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldPetEngage 3051 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AttackTarget engage formula for dual-wire cross-check (slice 3051):
//   !hasPreventActionEffect
auto inlineShouldPetEngage(const bool hasPreventActionEffect) -> bool
{
    return !hasPreventActionEffect;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::ShouldPetEngage
// (!HasPreventActionEffect on AttackTarget; slice 3051). Dense 2¹ boolean space.
auto runPetShouldEngage3051SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 pins still hold under dual-wire.
    ok = expect(ShouldPetEngage(false), "residual no prevent-action → engage") && ok;
    ok = expect(!ShouldPetEngage(true), "residual prevent-action → skip engage") && ok;

    const struct
    {
        bool        hasPreventActionEffect;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "no prevent-action engages" },
        { true, false, "prevent-action skips engage" },

        // Residual 1627 pins.
        { false, true, "residual ShouldPetEngage(false)" },
        { true, false, "residual ShouldPetEngage(true)" },

        // Polarity / dual-wire stability repeats.
        { false, true, "polarity engage when clear" },
        { true, false, "polarity skip when blocked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPetEngage(c.hasPreventActionEffect);
        const bool inlineF = inlineShouldPetEngage(c.hasPreventActionEffect);
        const bool wantPin = !c.hasPreventActionEffect;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPetEngage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPetEngage == pin formula !hasPreventActionEffect") && ok;
    }

    // Pin composition: engage iff !hasPreventActionEffect.
    ok = expect(ShouldPetEngage(false), "hasPreventActionEffect false must engage") && ok;
    ok = expect(!ShouldPetEngage(true), "hasPreventActionEffect true must not engage") && ok;

    // Dense compose: full 2¹ boolean space.
    for (const bool hasPrevent : { false, true })
    {
        const bool got  = ShouldPetEngage(hasPrevent);
        const bool want = !hasPrevent;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldPetEngage(hasPrevent), "compose free == inline") && ok;
    }

    // Explicit polarity: engage iff NOT prevent-action.
    for (const bool has : { false, true })
    {
        const bool got = ShouldPetEngage(has);
        ok             = expect(got == !has, "polarity: engage == !hasPrevent") && ok;
        ok             = expect(!(got && has), "polarity: must not engage when prevent-action true") && ok;
        ok             = expect(!(!got && !has), "polarity: must engage when prevent-action false") && ok;
    }

    // Host-style inject poles: HasPreventActionEffect as bool on AttackTarget.
    for (const bool hasPrevent : { false, true })
    {
        const bool engage = ShouldPetEngage(hasPrevent);
        ok                = expect(engage == !hasPrevent, "host inject dual-wire polarity") && ok;
        ok                = expect(engage == inlineShouldPetEngage(hasPrevent), "host inject free == inline") && ok;
        ok                = expect(!(engage && hasPrevent), "engage only when !hasPrevent") && ok;
        ok                = expect(!(!engage && !hasPrevent), "!hasPrevent must engage") && ok;
    }

    // Production AttackTarget path semantics:
    // !HasPreventActionEffect → Engage; else skip.
    ok = expect(ShouldPetEngage(false), "AttackTarget no prevent-action → engage path") && ok;
    ok = expect(!ShouldPetEngage(true), "AttackTarget prevent-action → skip-engage path") && ok;

    // Sibling residual CanAttackTarget (1627) is orthogonal: null preflight
    // runs first; free engage polarity does not depend on master/pet/target.
    // CanAttackTarget(m,p,t) = m && p && t.
    ok = expect(CanAttackTarget(true, true, true), "residual CanAttackTarget admit when all present") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "residual CanAttackTarget reject when master null") && ok;
    for (const bool hasPrevent : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool hasPet : { false, true })
            {
                for (const bool hasTarget : { false, true })
                {
                    // Orthogonal: engage iff !hasPrevent; preflight iff all three.
                    ok = expect(ShouldPetEngage(hasPrevent) == !hasPrevent,
                                "engage vs preflight compose hasPrevent") &&
                         ok;
                    const bool wantAdmit = hasMaster && hasPet && hasTarget;
                    ok                   = expect(CanAttackTarget(hasMaster, hasPet, hasTarget) == wantAdmit,
                                "engage vs preflight compose admit") &&
                         ok;
                }
            }
        }
    }

    // Sibling residual ShouldPetDisengage shares the same formula shape but
    // is not dual-wired in this slice (RetreatToMaster residual).
    for (const bool hasPrevent : { false, true })
    {
        ok = expect(ShouldPetDisengage(hasPrevent) == !hasPrevent,
                    "residual ShouldPetDisengage polarity") &&
             ok;
        // Dual-wire engage and residual disengage share !hasPrevent polarity.
        ok = expect(ShouldPetEngage(hasPrevent) == ShouldPetDisengage(hasPrevent),
                    "engage vs residual disengage polarity") &&
             ok;
    }

    // Explicit dual-wire poles: free == !hasPrevent for both bools.
    for (const bool hasPrevent : { false, true })
    {
        const bool got = ShouldPetEngage(hasPrevent);
        ok             = expect(got == !hasPrevent, "host inject dual-wire polarity poles") && ok;
        ok             = expect(got == inlineShouldPetEngage(hasPrevent), "host inject free == inline poles") && ok;
    }

    return ok;
}
