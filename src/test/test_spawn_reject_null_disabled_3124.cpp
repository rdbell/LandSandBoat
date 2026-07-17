#include "test_spawn_reject_null_disabled_3124.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectSpawnNullOrDisabled 3124 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline null-or-disabled gate for dual-wire cross-check (slice 3124):
//   mobNull || !allowRespawn
// Positive form avoids QF1001 De Morgan rewrite of !(!mobNull && allowRespawn).
auto inlineShouldRejectSpawnNullOrDisabled(const bool mobNull, const bool allowRespawn) -> bool
{
    return mobNull || !allowRespawn;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectSpawnNullOrDisabled
// (mobNull || !allowRespawn; slice 3124).
auto runSpawnRejectNullDisabled3124SelfTests() -> bool
{
    using spawnhelpers::CanSpawnNowPure;
    using spawnhelpers::ShouldRejectSpawnNullOrDisabled;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;

    bool ok = true;

    const struct
    {
        bool        mobNull;
        bool        allowRespawn;
        bool        want;
        const char* label;
    } cases[] = {
        // Full 2² domain (mobNull × allowRespawn).
        { false, true, false, "present + allow allows" },
        { false, false, true, "present + !allow rejects" },
        { true, true, true, "null + allow rejects" },
        { true, false, true, "null + !allow rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectSpawnNullOrDisabled(c.mobNull, c.allowRespawn);
        const bool inlineF = inlineShouldRejectSpawnNullOrDisabled(c.mobNull, c.allowRespawn);
        const bool wantPin = c.mobNull || !c.allowRespawn;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectSpawnNullOrDisabled dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectSpawnNullOrDisabled == pin formula") && ok;
    }

    // Pin composition: only present+allow passes; either bad leg rejects.
    ok = expect(!ShouldRejectSpawnNullOrDisabled(false, true), "present + allow must not reject") && ok;
    ok = expect(ShouldRejectSpawnNullOrDisabled(true, true), "null + allow must reject") && ok;
    ok = expect(ShouldRejectSpawnNullOrDisabled(false, false), "present + !allow must reject") && ok;
    ok = expect(ShouldRejectSpawnNullOrDisabled(true, false), "null + !allow must reject") && ok;

    // Dense compose: full mobNull × allowRespawn 2² free==inline.
    for (const bool mobNull : { false, true })
    {
        for (const bool allowRespawn : { false, true })
        {
            const bool got  = ShouldRejectSpawnNullOrDisabled(mobNull, allowRespawn);
            const bool want = mobNull || !allowRespawn;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectSpawnNullOrDisabled(mobNull, allowRespawn),
                        "compose free == inline") &&
                 ok;
        }
    }

    // CanSpawnNowPure integration: null/disabled gate still composes via free function.
    ok = expect(CanSpawnNowPure(false, true, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure present + allow must pass null/disabled gate") &&
         ok;
    ok = expect(!CanSpawnNowPure(true, true, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure null + allow must fail") &&
         ok;
    ok = expect(!CanSpawnNowPure(false, false, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure present + !allow must fail") &&
         ok;
    ok = expect(!CanSpawnNowPure(true, false, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure null + !allow must fail") &&
         ok;

    // Sibling dual-wires still independent of null/disabled dual-wire.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + DAY must still fail night residual (3092)") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + NIGHT must still pass night residual (3092)") &&
         ok;
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATEVENING + DAY must still fail evening residual (3107)") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, true, false, false, true),
                "CanSpawnNowPure ATEVENING + EVENING must still pass evening residual (3107)") &&
         ok;

    return ok;
}
