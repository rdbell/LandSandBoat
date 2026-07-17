#include "test_spawn_reject_night_3092.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectAtNightSpawn 3092 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ATNIGHT spawn-type TOTD gate for dual-wire cross-check (slice 3092):
//   (spawnType & SPAWNTYPE_ATNIGHT) != 0 && !(totd == NIGHT || totd == MIDNIGHT)
//   // SPAWNTYPE_ATNIGHT = 0x01
//   // TOTD: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
auto inlineShouldRejectAtNightSpawn(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return (spawnType & 0x01) != 0 &&
           !(totd == vanadiel_time::TOTD::NIGHT || totd == vanadiel_time::TOTD::MIDNIGHT);
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectAtNightSpawn
// (ATNIGHT flag && not NIGHT/MIDNIGHT TOTD; slice 3092).
auto runSpawnRejectNight3092SelfTests() -> bool
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

    const struct
    {
        uint8                 spawnType;
        vanadiel_time::TOTD   totd;
        bool                  want;
        const char*           label;
    } cases[] = {
        // ATNIGHT flag × night window (allows)
        { SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, false, "ATNIGHT + NIGHT allows" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT, false, "ATNIGHT + MIDNIGHT allows" },

        // ATNIGHT flag × non-night window (rejects)
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAY, true, "ATNIGHT + DAY rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::NONE, true, "ATNIGHT + NONE rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::NEWDAY, true, "ATNIGHT + NEWDAY rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAWN, true, "ATNIGHT + DAWN rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::DUSK, true, "ATNIGHT + DUSK rejects" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::EVENING, true, "ATNIGHT + EVENING rejects" },

        // no ATNIGHT flag — never rejects regardless of totd
        { 0x00, vanadiel_time::TOTD::DAY, false, "normal type DAY allows" },
        { 0x00, vanadiel_time::TOTD::NIGHT, false, "normal type NIGHT allows" },
        { 0x00, vanadiel_time::TOTD::MIDNIGHT, false, "normal type MIDNIGHT allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, false, "ATEVENING only DAY allows (night gate)" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, false, "ATEVENING only NIGHT allows (night gate)" },
        { SpawnTypeFog, vanadiel_time::TOTD::DAY, false, "FOG only DAY allows (night gate)" },
        { SpawnTypeWeather, vanadiel_time::TOTD::DAY, false, "WEATHER only DAY allows (night gate)" },

        // combined flags with ATNIGHT bit
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|ATEVENING DAY rejects night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::NIGHT, false,
          "ATNIGHT|ATEVENING NIGHT allows night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeFog), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|FOG DAY rejects night gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeFog), vanadiel_time::TOTD::NIGHT, false,
          "ATNIGHT|FOG NIGHT allows night gate" },
        { 0xFF, vanadiel_time::TOTD::DAY, true, "all flags DAY rejects night gate" },
        { 0xFF, vanadiel_time::TOTD::NIGHT, false, "all flags NIGHT allows night gate" },
        { 0xFF, vanadiel_time::TOTD::MIDNIGHT, false, "all flags MIDNIGHT allows night gate" },

        // bit isolation: 0x01 vs neighbors 0x02 / 0x00
        { 0x01, vanadiel_time::TOTD::DAY, true, "0x01 ATNIGHT alone DAY rejects" },
        { 0x01, vanadiel_time::TOTD::NIGHT, false, "0x01 ATNIGHT alone NIGHT allows" },
        { 0x02, vanadiel_time::TOTD::DAY, false, "0x02 ATEVENING alone DAY allows night gate" },
        { 0x03, vanadiel_time::TOTD::DAY, true, "0x03 ATNIGHT|ATEVENING DAY rejects night gate" },
        { 0x03, vanadiel_time::TOTD::NIGHT, false, "0x03 ATNIGHT|ATEVENING NIGHT allows night gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAtNightSpawn(c.spawnType, c.totd);
        const bool inlineF = inlineShouldRejectAtNightSpawn(c.spawnType, c.totd);
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(c.totd);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectAtNightSpawn dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectAtNightSpawn == pin formula") && ok;
    }

    // Pin composition: ATNIGHT flag is required; night window alone never rejects.
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::DAY), "no ATNIGHT + DAY must not reject") && ok;
    ok = expect(!ShouldRejectAtNightSpawn(0, vanadiel_time::TOTD::NIGHT), "no ATNIGHT + NIGHT must not reject") && ok;
    ok = expect(ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::DAY), "ATNIGHT + DAY must reject") && ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT), "ATNIGHT + NIGHT must not reject") &&
         ok;
    ok = expect(!ShouldRejectAtNightSpawn(SpawnTypeAtNight, vanadiel_time::TOTD::MIDNIGHT),
                "ATNIGHT + MIDNIGHT must not reject") &&
         ok;

    // Dense compose: spawnType bit space × full TOTD domain.
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
            const auto st   = static_cast<uint8>(spawnType);
            const bool got  = ShouldRejectAtNightSpawn(st, totd);
            const bool want = HasSpawnTypeFlag(st, SpawnTypeAtNight) && !IsNightTotdWindow(totd);
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectAtNightSpawn(st, totd), "compose free == inline") && ok;
        }
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
    // Sibling evening residual still independent of night dual-wire.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATEVENING + DAY must still fail evening residual") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, true, false, false, true),
                "CanSpawnNowPure ATEVENING + EVENING must still pass evening residual") &&
         ok;

    return ok;
}
