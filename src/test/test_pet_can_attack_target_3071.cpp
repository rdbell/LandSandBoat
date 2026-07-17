#include "test_pet_can_attack_target_3071.h"

#include "map/pet_engage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet CanAttackTarget 3071 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AttackTarget null-preflight formula for dual-wire cross-check
// (slice 3071):
//   hasMaster && hasPet && hasTarget
auto inlineCanAttackTarget(const bool hasMaster, const bool hasPet, const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::CanAttackTarget
// (null master/pet/target preflight on AttackTarget; slice 3071).
// Dense 2³ boolean space.
auto runPetCanAttackTarget3071SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;
    using petengagehelpers::CanRetreatToMaster;
    using petengagehelpers::ShouldPetDisengage;
    using petengagehelpers::ShouldPetEngage;

    bool ok = true;

    // Residual 1627 pins still hold under dual-wire.
    ok = expect(CanAttackTarget(true, true, true), "residual all present → admit") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "residual master null → reject") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "residual pet null → reject") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "residual target null → reject") && ok;

    const struct
    {
        bool        hasMaster;
        bool        hasPet;
        bool        hasTarget;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2³ boolean space.
        { false, false, false, false, "all null → reject" },
        { false, false, true, false, "only target → reject" },
        { false, true, false, false, "only pet → reject" },
        { false, true, true, false, "no master → reject" },
        { true, false, false, false, "only master → reject" },
        { true, false, true, false, "no pet → reject" },
        { true, true, false, false, "no target → reject" },
        { true, true, true, true, "all present → admit" },

        // Residual 1627 pins.
        { true, true, true, true, "residual CanAttackTarget(true,true,true)" },
        { false, true, true, false, "residual master null" },
        { true, false, true, false, "residual pet null" },
        { true, true, false, false, "residual target null" },

        // Dual-wire stability repeats.
        { true, true, true, true, "polarity admit when all present" },
        { false, false, false, false, "polarity reject when all null" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAttackTarget(c.hasMaster, c.hasPet, c.hasTarget);
        const bool inlineF = inlineCanAttackTarget(c.hasMaster, c.hasPet, c.hasTarget);
        const bool wantPin = c.hasMaster && c.hasPet && c.hasTarget;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanAttackTarget dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanAttackTarget == pin formula hasMaster&&hasPet&&hasTarget") && ok;
    }

    // Pin composition: admit iff hasMaster && hasPet && hasTarget.
    ok = expect(CanAttackTarget(true, true, true), "all present must admit") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "master null must reject") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "pet null must reject") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "target null must reject") && ok;

    // Dense compose: full 2³ boolean space.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            for (const bool hasTarget : { false, true })
            {
                const bool got  = CanAttackTarget(hasMaster, hasPet, hasTarget);
                const bool want = hasMaster && hasPet && hasTarget;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineCanAttackTarget(hasMaster, hasPet, hasTarget),
                           "compose free == inline") &&
                     ok;
            }
        }
    }

    // Explicit polarity: admit only when all three true.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            for (const bool hasTarget : { false, true })
            {
                const bool got  = CanAttackTarget(hasMaster, hasPet, hasTarget);
                const bool want = hasMaster && hasPet && hasTarget;
                ok              = expect(got == want, "polarity: admit == all present") && ok;
                ok              = expect(!(got && !hasMaster), "polarity: never admit when !hasMaster") && ok;
                ok              = expect(!(got && !hasPet), "polarity: never admit when !hasPet") && ok;
                ok              = expect(!(got && !hasTarget), "polarity: never admit when !hasTarget") && ok;
                ok              = expect(!(!got && hasMaster && hasPet && hasTarget),
                           "polarity: always admit when all present") &&
                     ok;
            }
        }
    }

    // Host-style inject poles: pointer non-null as bools on AttackTarget.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            for (const bool hasTarget : { false, true })
            {
                const bool admit = CanAttackTarget(hasMaster, hasPet, hasTarget);
                ok               = expect(admit == (hasMaster && hasPet && hasTarget),
                            "host inject dual-wire polarity") &&
                     ok;
                ok = expect(admit == inlineCanAttackTarget(hasMaster, hasPet, hasTarget),
                            "host inject free == inline") &&
                     ok;
                ok = expect(!(admit && !(hasMaster && hasPet && hasTarget)),
                            "admit only when all present") &&
                     ok;
                ok = expect(!(!admit && hasMaster && hasPet && hasTarget),
                            "all present must admit") &&
                     ok;
            }
        }
    }

    // Production AttackTarget path semantics:
    // null preflight first; only then ShouldPetEngage.
    ok = expect(CanAttackTarget(true, true, true), "AttackTarget all present → admit path") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "AttackTarget master null → reject path") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "AttackTarget pet null → reject path") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "AttackTarget target null → reject path") && ok;

    // Sibling dual-wire ShouldPetEngage (3051) is orthogonal: null preflight
    // runs first; free engage polarity does not depend on master/pet/target.
    // ShouldPetEngage(hasPrevent) = !hasPrevent.
    ok = expect(ShouldPetEngage(false), "sibling ShouldPetEngage admit when no prevent-action") && ok;
    ok = expect(!ShouldPetEngage(true), "sibling ShouldPetEngage reject when prevent-action") && ok;
    for (const bool hasPrevent : { false, true })
    {
        for (const bool hasMaster : { false, true })
        {
            for (const bool hasPet : { false, true })
            {
                for (const bool hasTarget : { false, true })
                {
                    const bool wantAdmit = hasMaster && hasPet && hasTarget;
                    ok                   = expect(CanAttackTarget(hasMaster, hasPet, hasTarget) == wantAdmit,
                                "preflight vs engage compose admit") &&
                         ok;
                    ok = expect(ShouldPetEngage(hasPrevent) == !hasPrevent,
                                "preflight vs engage compose hasPrevent") &&
                         ok;
                    const bool wouldEngage = wantAdmit && !hasPrevent;
                    const bool gotEngage =
                        CanAttackTarget(hasMaster, hasPet, hasTarget) && ShouldPetEngage(hasPrevent);
                    ok = expect(gotEngage == wouldEngage, "AttackTarget path both gates") && ok;
                }
            }
        }
    }

    // Sibling residual CanRetreatToMaster / ShouldPetDisengage share capacity
    // but are not dual-wired in this slice (RetreatToMaster residual 1627).
    ok = expect(CanRetreatToMaster(true, true), "residual CanRetreatToMaster admit") && ok;
    ok = expect(!CanRetreatToMaster(false, true), "residual CanRetreatToMaster master null") && ok;
    ok = expect(!CanRetreatToMaster(true, false), "residual CanRetreatToMaster pet null") && ok;
    for (const bool hasPrevent : { false, true })
    {
        ok = expect(ShouldPetDisengage(hasPrevent) == !hasPrevent,
                    "residual ShouldPetDisengage polarity") &&
             ok;
    }

    // Explicit dual-wire poles: free == hasMaster && hasPet && hasTarget.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            for (const bool hasTarget : { false, true })
            {
                const bool got = CanAttackTarget(hasMaster, hasPet, hasTarget);
                ok             = expect(got == (hasMaster && hasPet && hasTarget),
                            "host inject dual-wire polarity poles") &&
                     ok;
                ok = expect(got == inlineCanAttackTarget(hasMaster, hasPet, hasTarget),
                            "host inject free == inline poles") &&
                     ok;
            }
        }
    }

    return ok;
}
