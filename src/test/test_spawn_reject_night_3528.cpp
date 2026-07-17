#include "test_spawn_reject_night_3528.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectAtNightSpawn 3528 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ATNIGHT spawn-type TOTD gate for dedicated dual-wire expand residual
// 3092 cross-check (slice 3528; prior dedicated 3483/3435/3387 retained):
//   (spawnType & SPAWNTYPE_ATNIGHT) != 0 && !(totd == NIGHT || totd == MIDNIGHT)
//   // SPAWNTYPE_ATNIGHT = 0x01
//   // night window: NIGHT || MIDNIGHT
//   // TOTD: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
// Positive form avoids De Morgan rewrite of !(A||B).
auto inlineShouldRejectAtNightSpawn3528(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    const bool hasNightFlag   = (spawnType & 0x01) != 0;
    const bool inNightWindow  = totd == vanadiel_time::TOTD::NIGHT || totd == vanadiel_time::TOTD::MIDNIGHT;
    return hasNightFlag && !inNightWindow;
}

// Compact dual-wire pin matching Go pinShouldRejectAtNightSpawn3528 / C++
// capacity positive form (formula unchanged from 1362 / 3092 / 3387 / 3435 / 3483):
//   HasSpawnTypeFlag(spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(totd)
auto pinShouldRejectAtNightSpawn3528(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return spawnhelpers::HasSpawnTypeFlag(spawnType, spawnhelpers::SpawnTypeAtNight) &&
           !spawnhelpers::IsNightTotdWindow(totd);
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectAtNightSpawn
// (ATNIGHT flag && not NIGHT/MIDNIGHT TOTD;
// OmegaXI internal/spawnslot; dedicated dual-wire expand residual 3092 slice 3528;
// prior dedicated 3483/3435/3387 retained; residual expand 3092 / pure 1362).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 3092 / 1362 and prior 3483 / 3435 / 3387 pins still hold
//   - poles: ATNIGHT + DAY rejects vs night window allows
//   - dense spawnType×TOTD free == inline == pin
//   - host inject poles
auto runSpawnRejectNight3528SelfTests() -> bool
{
    using spawnhelpers::CanSpawnNowPure;
    using spawnhelpers::HasSpawnTypeFlag;
    using spawnhelpers::IsNightTotdWindow;
    using spawnhelpers::ShouldRejectAtNightSpawn;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;
    using spawnhelpers::SpawnTypeFog;
    using spawnhelpers::SpawnTypeWeather;

    bool ok = true;

    // Enum / bit pins (formula depends on these exact values).
    ok = expect(SpawnTypeAtNight == static_cast<uint8>(0x01), "SpawnTypeAtNight pin 0x01") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NONE) == 0, "TOTD::NONE pin 0") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::MIDNIGHT) == 1, "TOTD::MIDNIGHT pin 1") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NEWDAY) == 2, "TOTD::NEWDAY pin 2") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DAWN) == 3, "TOTD::DAWN pin 3") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DAY) == 4, "TOTD::DAY pin 4") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DUSK) == 5, "TOTD::DUSK pin 5") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::EVENING) == 6, "TOTD::EVENING pin 6") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NIGHT) == 7, "TOTD::NIGHT pin 7") && ok;
    ok = expect(IsNightTotdWindow(vanadiel_time::TOTD::NIGHT), "IsNightTotdWindow NIGHT") && ok;
    ok = expect(IsNightTotdWindow(vanadiel_time::TOTD::MIDNIGHT), "IsNightTotdWindow MIDNIGHT") && ok;
    ok = expect(!IsNightTotdWindow(vanadiel_time::TOTD::DAY), "IsNightTotdWindow not DAY") && ok;
    ok = expect(!IsNightTotdWindow(vanadiel_time::TOTD::EVENING), "IsNightTotdWindow not EVENING") && ok;
    ok = expect(!IsNightTotdWindow(vanadiel_time::TOTD::NEWDAY), "IsNightTotdWindow not NEWDAY") && ok;

    // Residual 3092 / 1362 / prior 3483 / 3435 / 3387 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::DAY),
                "residual ATNIGHT + DAY rejects") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT),
                "residual ATNIGHT + NIGHT allows") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT),
                "residual ATNIGHT + MIDNIGHT allows") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::DAY), "residual no ATNIGHT + DAY allows") && ok;
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::NIGHT),
                "residual no ATNIGHT + NIGHT allows") &&
         ok;

    // --- Core poles: free == inline == pin positive form ---
    // ATNIGHT-required + non-night window vs night window.
    const struct
    {
        uint8               spawnType;
        vanadiel_time::TOTD totd;
        bool                want;
        const char*         label;
    } cases[] = {
        // ATNIGHT-required classic reject: ATNIGHT flag + DAY.
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAY, true, "ATNIGHT-required + DAY rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::NONE, true, "ATNIGHT-required + NONE rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::NEWDAY, true, "ATNIGHT-required + NEWDAY rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAWN, true, "ATNIGHT-required + DAWN rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::DUSK, true, "ATNIGHT-required + DUSK rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::EVENING, true, "ATNIGHT-required + EVENING rejects" },
        // Night window allows ATNIGHT-type spawn.
        { SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, false, "ATNIGHT-required + NIGHT allows" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT, false, "ATNIGHT-required + MIDNIGHT allows" },

        // Flag required: night window alone never rejects.
        { 0x00, vanadiel_time::TOTD::DAY, false, "no ATNIGHT + DAY allows" },
        { 0x00, vanadiel_time::TOTD::NIGHT, false, "no ATNIGHT + NIGHT allows" },
        { 0x00, vanadiel_time::TOTD::MIDNIGHT, false, "no ATNIGHT + MIDNIGHT allows" },

        // Sibling spawn-type bits alone do not trip the night gate.
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, false, "ATEVENING only DAY allows (night gate)" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, false, "ATEVENING only NIGHT allows (night gate)" },
        { SpawnTypeFog, vanadiel_time::TOTD::DAY, false, "FOG only DAY allows (night gate)" },
        { SpawnTypeWeather, vanadiel_time::TOTD::DAY, false, "WEATHER only DAY allows (night gate)" },

        // Combined flags: ATNIGHT bit still gates.
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|ATEVENING DAY rejects night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::NIGHT, false,
          "ATNIGHT|ATEVENING NIGHT allows night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeFog), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|FOG DAY rejects night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeFog), vanadiel_time::TOTD::NIGHT, false,
          "ATNIGHT|FOG NIGHT allows night gate" },

        // Bit isolation: 0x01 vs neighbors 0x02 / 0x00.
        { 0x01, vanadiel_time::TOTD::DAY, true, "0x01 ATNIGHT alone DAY rejects" },
        { 0x01, vanadiel_time::TOTD::NIGHT, false, "0x01 ATNIGHT alone NIGHT allows" },
        { 0x02, vanadiel_time::TOTD::DAY, false, "0x02 ATEVENING alone DAY allows night gate" },
        { 0x03, vanadiel_time::TOTD::DAY, true, "0x03 ATNIGHT|ATEVENING DAY rejects night gate" },
        { 0x03, vanadiel_time::TOTD::NIGHT, false, "0x03 ATNIGHT|ATEVENING NIGHT allows night gate" },
        { 0xFF, vanadiel_time::TOTD::DAY, true, "all flags DAY rejects night gate" },
        { 0xFF, vanadiel_time::TOTD::NIGHT, false, "all flags NIGHT allows night gate" },
        { 0xFF, vanadiel_time::TOTD::MIDNIGHT, false, "all flags MIDNIGHT allows night gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAtNightSpawn(c.spawnType, c.totd);
        const bool inlineF = inlineShouldRejectAtNightSpawn3528(c.spawnType, c.totd);
        const bool pin     = pinShouldRejectAtNightSpawn3528(c.spawnType, c.totd);
        // Positive form pin composition (ATNIGHT flag && !night window).
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(c.totd);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRejectAtNightSpawn free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual ATNIGHT-required poles.
    ok = expect(ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::DAY) ==
                    pinShouldRejectAtNightSpawn3528(SpawnTypeAtNight, vanadiel_time::TOTD::DAY),
                "free == pin ATNIGHT-required + DAY") &&
         ok;
    ok = expect(ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT) ==
                    pinShouldRejectAtNightSpawn3528(SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT),
                "free == pin ATNIGHT-required + NIGHT") &&
         ok;
    ok = expect(ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::DAY) ==
                    pinShouldRejectAtNightSpawn3528(0, vanadiel_time::TOTD::DAY),
                "free == pin no ATNIGHT + DAY") &&
         ok;
    ok = expect(ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::NIGHT) ==
                    pinShouldRejectAtNightSpawn3528(0, vanadiel_time::TOTD::NIGHT),
                "free == pin no ATNIGHT + NIGHT") &&
         ok;

    // Dense compose: spawnType bit space × full TOTD domain free == inline == pin.
    const vanadiel_time::TOTD totds[] = {
        vanadiel_time::TOTD::NONE,
        vanadiel_time::TOTD::MIDNIGHT,
        vanadiel_time::TOTD::NEWDAY,
        vanadiel_time::TOTD::DAWN,
        vanadiel_time::TOTD::DAY,
        vanadiel_time::TOTD::DUSK,
        vanadiel_time::TOTD::EVENING,
        vanadiel_time::TOTD::NIGHT,
    };
    for (int spawnType = 0; spawnType <= 0xFF; ++spawnType)
    {
        for (const auto totd : totds)
        {
            const auto st      = static_cast<uint8>(spawnType);
            const bool got     = ShouldRejectAtNightSpawn(st, totd);
            const bool inlineF = inlineShouldRejectAtNightSpawn3528(st, totd);
            const bool pin     = pinShouldRejectAtNightSpawn3528(st, totd);
            const bool want    = HasSpawnTypeFlag(st, SpawnTypeAtNight) && !IsNightTotdWindow(totd);
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production canSpawnNow path semantics ---
    // Host injects:
    //   totd = current vanadiel_time::TOTD
    // when true (ATNIGHT + !night window) → reject spawn
    // when false                          → night gate passes
    ok = expect(ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::DAY) &&
                    pinShouldRejectAtNightSpawn3528(SpawnTypeAtNight, vanadiel_time::TOTD::DAY),
                "host ATNIGHT + DAY → reject path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT),
                "host ATNIGHT + NIGHT → allow") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT),
                "host ATNIGHT + MIDNIGHT → allow") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::DAY), "host no ATNIGHT + DAY → allow") && ok;
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::NIGHT), "host no ATNIGHT + NIGHT → allow") && ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        uint8               spawnType;
        vanadiel_time::TOTD totd;
        const char*         label;
    } hostPoles[] = {
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAY, "ATNIGHT + DAY reject" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, "ATNIGHT + NIGHT allow" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT, "ATNIGHT + MIDNIGHT allow" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::EVENING, "ATNIGHT + EVENING reject" },
        { 0x00, vanadiel_time::TOTD::DAY, "no ATNIGHT + DAY allow" },
        { 0x00, vanadiel_time::TOTD::NIGHT, "no ATNIGHT + NIGHT allow" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectAtNightSpawn(p.spawnType, p.totd);
        const bool inlineF = inlineShouldRejectAtNightSpawn3528(p.spawnType, p.totd);
        const bool pin     = pinShouldRejectAtNightSpawn3528(p.spawnType, p.totd);
        const bool want    = HasSpawnTypeFlag(p.spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(p.totd);
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // CanSpawnNowPure integration: night gate still composes via free function.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + DAY must fail") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + NIGHT must pass night gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + MIDNIGHT must pass night gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure normal + DAY must pass") &&
         ok;
    // Sibling evening dual-wire still independent of night dual-wire.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATEVENING + DAY must still fail evening residual") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, true, false, false, true),
                "CanSpawnNowPure ATEVENING + EVENING must still pass evening residual") &&
         ok;

    return ok;
}
