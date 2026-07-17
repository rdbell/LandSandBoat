#include "test_petutils_can_attack_target_3269.h"

#include "map/pet_engage_capacity.h"

#include <iostream>
#include <tuple>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petutils CanAttackTarget 3269 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AttackTarget null-preflight for dual-wire cross-check (dedicated 3269):
//   hasMaster && hasPet && hasTarget
auto inlineCanAttackTarget(
    const bool hasMaster,
    const bool hasPet,
    const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

// Compact dual-wire pin matching Go pinCanAttackTarget3269 / C++ capacity:
//   hasMaster && hasPet && hasTarget
auto pinCanAttackTarget(
    const bool hasMaster,
    const bool hasPet,
    const bool hasTarget) -> bool
{
    return hasMaster && hasPet && hasTarget;
}

} // namespace

// Pure dual-wire expansion for petengagehelpers::CanAttackTarget
// (AttackTarget null master/pet/target preflight; OmegaXI internal/petutils;
// dedicated slice 3269 expand residual 3071; pure 1627).
//
// Coverage:
//   - free == inline == pin == (hasMaster && hasPet && hasTarget)
//   - residual 1627 / 3071 pins still hold
//   - dense 2^3 boolean space
auto runPetutilsCanAttackTarget3269SelfTests() -> bool
{
    using petengagehelpers::CanAttackTarget;

    bool ok = true;

    // Residual 1627 / 3071 pins still hold under dual-wire.
    ok = expect(CanAttackTarget(true, true, true), "residual all present → admit") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "residual master null → reject") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "residual pet null → reject") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "residual target null → reject") && ok;
    ok = expect(!CanAttackTarget(false, false, false), "residual all null → reject") && ok;
    ok = expect(!CanAttackTarget(true, false, false), "residual only master → reject") && ok;
    ok = expect(!CanAttackTarget(false, true, false), "residual only pet → reject") && ok;
    ok = expect(!CanAttackTarget(false, false, true), "residual only target → reject") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        hasMaster;
        bool        hasPet;
        bool        hasTarget;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1627 / 3071 poles (full 2^3 surface).
        { true, true, true, true, "residual all present" },
        { false, true, true, false, "residual master null" },
        { true, false, true, false, "residual pet null" },
        { true, true, false, false, "residual target null" },
        { false, false, false, false, "residual all null" },
        { false, false, true, false, "residual only target" },
        { true, false, false, false, "residual only master" },
        { false, true, false, false, "residual only pet" },

        // Classic dual poles.
        { true, true, true, true, "all present accepts" },
        { false, true, true, false, "master null rejects" },
        { true, false, true, false, "pet null rejects" },
        { true, true, false, false, "target null rejects" },

        // Host-style inject poles.
        { false, false, false, false, "host all-null inject" },
        { true, false, false, false, "host master-only inject" },
        { true, true, false, false, "host no-target inject" },
        { true, true, true, true, "host admit inject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAttackTarget(c.hasMaster, c.hasPet, c.hasTarget);
        const bool inlineF = inlineCanAttackTarget(c.hasMaster, c.hasPet, c.hasTarget);
        const bool pin     = pinCanAttackTarget(c.hasMaster, c.hasPet, c.hasTarget);
        const bool wantPin = c.hasMaster && c.hasPet && c.hasTarget;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula hasMaster&&hasPet&&hasTarget") && ok;
    }

    // Pin composition: only the all-present triple admits.
    ok = expect(CanAttackTarget(true, true, true), "all present must admit") && ok;
    ok = expect(!CanAttackTarget(false, true, true), "master null must reject") && ok;
    ok = expect(!CanAttackTarget(true, false, true), "pet null must reject") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "target null must reject") && ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool hasMaster : { false, true })
    {
        for (const bool hasPet : { false, true })
        {
            for (const bool hasTarget : { false, true })
            {
                const bool got     = CanAttackTarget(hasMaster, hasPet, hasTarget);
                const bool inlineF = inlineCanAttackTarget(hasMaster, hasPet, hasTarget);
                const bool pin     = pinCanAttackTarget(hasMaster, hasPet, hasTarget);
                const bool want    = hasMaster && hasPet && hasTarget;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
            }
        }
    }

    // Host-style inject poles: free == inline == pin for AttackTarget.
    for (const auto& pole : {
             // hasMaster, hasPet, hasTarget
             std::tuple<bool, bool, bool>{ false, false, false }, // all null
             std::tuple<bool, bool, bool>{ true, false, false },  // master only
             std::tuple<bool, bool, bool>{ true, true, false },   // no target
             std::tuple<bool, bool, bool>{ true, true, true },    // admit
             std::tuple<bool, bool, bool>{ false, true, true },
             std::tuple<bool, bool, bool>{ true, false, true },
             std::tuple<bool, bool, bool>{ false, false, true },
             std::tuple<bool, bool, bool>{ false, true, false },
         })
    {
        const bool hasMaster = std::get<0>(pole);
        const bool hasPet    = std::get<1>(pole);
        const bool hasTarget = std::get<2>(pole);
        const bool got       = CanAttackTarget(hasMaster, hasPet, hasTarget);
        const bool inlineF   = inlineCanAttackTarget(hasMaster, hasPet, hasTarget);
        const bool pin       = pinCanAttackTarget(hasMaster, hasPet, hasTarget);
        const bool inject    = hasMaster && hasPet && hasTarget;
        ok                   = expect(got == inject, "host inject dual-wire identity") && ok;
        ok = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production AttackTarget path semantics:
    //   all present → admit path (proceed to ShouldPetEngage)
    //   else → ShowWarning + return
    ok = expect(CanAttackTarget(true, true, true), "AttackTarget all present → admit path") && ok;
    ok = expect(!CanAttackTarget(false, false, false), "AttackTarget all-null inject → reject path") && ok;
    ok = expect(!CanAttackTarget(true, false, false), "AttackTarget master-only inject → reject path") && ok;
    ok = expect(!CanAttackTarget(true, true, false), "AttackTarget no-target inject → reject path") && ok;

    return ok;
}
