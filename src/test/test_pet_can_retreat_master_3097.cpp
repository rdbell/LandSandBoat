#include "test_pet_can_retreat_master_3097.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet CanRetreatToMaster 3097 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RetreatToMaster null-preflight formula for dual-wire cross-check
// (slice 3097):
//   hasMaster && hasPet
auto inlineCanRetreatToMaster(const bool hasMaster, const bool hasPet) -> bool
{
    return hasMaster && hasPet;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::CanRetreatToMaster
// (null master/pet preflight on RetreatToMaster; slice 3097).
// Dense 2² boolean space.
auto runPetCanRetreatMaster3097SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::CanRetreatToMaster;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 pins still hold under dual-wire.
    ok = expect(CanRetreatToMaster(true, true), "residual master+pet present → admit") && ok;
    ok = expect(!CanRetreatToMaster(false, true), "residual master null → reject") && ok;
    ok = expect(!CanRetreatToMaster(true, false), "residual pet null → reject") && ok;
    ok = expect(!CanRetreatToMaster(false, false), "residual both null → reject") && ok;

    const struct
    {
        bool        hasMaster;
        bool        hasPet;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2² boolean space.
        { false, false, false, "both null → reject" },
        { false, true, false, "only pet → reject" },
        { true, false, false, "only master → reject" },
        { true, true, true, "master+pet present → admit" },

        // Residual 1627 pins.
        { true, true, true, "residual CanRetreatToMaster(true,true)" },
        { false, true, false, "residual master null" },
        { true, false, false, "residual pet null" },
        { false, false, false, "residual both null" },

        // Dual-wire stability repeats.
        { true, true, true, "polarity admit when both present" },
        { false, false, false, "polarity reject when both null" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRetreatToMaster(c.hasMaster, c.hasPet);
        const bool inlineF = inlineCanRetreatToMaster(c.hasMaster, c.hasPet);
        const bool wantPin = c.hasMaster && c.hasPet;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRetreatToMaster dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanRetreatToMaster == pin formula hasMaster&&hasPet") && ok;
    }

    // Pin composition: admit iff hasMaster && hasPet.
    ok = expect(CanRetreatToMaster(true, true), "both present must admit") && ok;
    ok = expect(!CanRetreatToMaster(false, true), "master null must reject") && ok;
    ok = expect(!CanRetreatToMaster(true, false), "pet null must reject") && ok;
    ok = expect(!CanRetreatToMaster(false, false), "both null must reject") && ok;

    // Dense compose: full 2² boolean space.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            const bool got  = CanRetreatToMaster(hasMaster, hasPet);
            const bool want = hasMaster && hasPet;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanRetreatToMaster(hasMaster, hasPet),
                       "compose free == inline") &&
                 ok;
        }
    }

    // Explicit polarity: admit only when both true.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            const bool got  = CanRetreatToMaster(hasMaster, hasPet);
            const bool want = hasMaster && hasPet;
            ok              = expect(got == want, "polarity: admit == both present") && ok;
            ok              = expect(!(got && !hasMaster), "polarity: never admit when !hasMaster") && ok;
            ok              = expect(!(got && !hasPet), "polarity: never admit when !hasPet") && ok;
            ok              = expect(!(!got && hasMaster && hasPet),
                       "polarity: always admit when both present") &&
                 ok;
        }
    }

    // Host-style inject poles: pointer non-null as bools on RetreatToMaster.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            const bool admit = CanRetreatToMaster(hasMaster, hasPet);
            ok               = expect(admit == (hasMaster && hasPet),
                        "host inject dual-wire polarity") &&
                 ok;
            ok = expect(admit == inlineCanRetreatToMaster(hasMaster, hasPet),
                        "host inject free == inline") &&
                 ok;
            ok = expect(!(admit && !(hasMaster && hasPet)),
                        "admit only when both present") &&
                 ok;
            ok = expect(!(!admit && hasMaster && hasPet),
                        "both present must admit") &&
                 ok;
        }
    }

    // Production RetreatToMaster path semantics:
    // null preflight first; only then ShouldPetDisengage.
    ok = expect(CanRetreatToMaster(true, true), "RetreatToMaster both present → admit path") && ok;
    ok = expect(!CanRetreatToMaster(false, true), "RetreatToMaster master null → reject path") && ok;
    ok = expect(!CanRetreatToMaster(true, false), "RetreatToMaster pet null → reject path") && ok;
    ok = expect(!CanRetreatToMaster(false, false), "RetreatToMaster both null → reject path") && ok;

    // Sibling dual-wire ShouldPetDisengage (3081) is orthogonal: null preflight
    // runs first; free disengage polarity does not depend on master/pet.
    // ShouldPetDisengage(hasPrevent) = !hasPrevent.
    ok = expect(ShouldPetDisengage(false), "sibling ShouldPetDisengage admit when no prevent-action") && ok;
    ok = expect(!ShouldPetDisengage(true), "sibling ShouldPetDisengage reject when prevent-action") && ok;
    for (const bool hasPrevent : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool hasPet : { false, true })
            {
                const bool wantAdmit = hasMaster && hasPet;
                ok                   = expect(CanRetreatToMaster(hasMaster, hasPet) == wantAdmit,
                            "preflight vs disengage compose admit") &&
                     ok;
                ok = expect(ShouldPetDisengage(hasPrevent) == !hasPrevent,
                            "preflight vs disengage compose hasPrevent") &&
                     ok;
                const bool wouldDisengage = wantAdmit && !hasPrevent;
                const bool gotDisengage =
                    CanRetreatToMaster(hasMaster, hasPet) && ShouldPetDisengage(hasPrevent);
                ok = expect(gotDisengage == wouldDisengage, "RetreatToMaster path both gates") && ok;
            }
        }
    }

    // Sibling dual-wire ShouldPetEngage (3051) shares !hasPrevent polarity on
    // AttackTarget; leave 3051 surface alone.
    for (const bool hasPrevent : { false, true })
    {
        ok = expect(ShouldPetEngage(hasPrevent) == !hasPrevent,
                    "sibling ShouldPetEngage polarity") &&
             ok;
    }

    // Sibling dual-wire CanAttackTarget (3071) is orthogonal AttackTarget
    // null preflight (three injects; leave alone).
    ok = expect(CanAttackTarget(true, true, true), "sibling CanAttackTarget admit when all present") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "sibling CanAttackTarget reject when master null") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "sibling CanAttackTarget reject when pet null") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "sibling CanAttackTarget reject when target null") && ok;

    // Explicit dual-wire poles: free == hasMaster && hasPet.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            const bool got = CanRetreatToMaster(hasMaster, hasPet);
            ok             = expect(got == (hasMaster && hasPet),
                        "host inject dual-wire polarity poles") &&
                 ok;
            ok = expect(got == inlineCanRetreatToMaster(hasMaster, hasPet),
                        "host inject free == inline poles") &&
                 ok;
        }
    }

    return ok;
}
