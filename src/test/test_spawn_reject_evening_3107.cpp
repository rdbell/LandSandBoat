#include "test_spawn_reject_evening_3107.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectAtEveningSpawn 3107 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ATEVENING spawn-type TOTD gate for dual-wire cross-check (slice 3107):
//   (spawnType & SPAWNTYPE_ATEVENING) != 0 && !(totd in evening window)
//   // SPAWNTYPE_ATEVENING = 0x02
//   // evening window: EVENING || NIGHT || MIDNIGHT || NEWDAY
//   // TOTD: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
// Positive form avoids De Morgan rewrite of !(A||B||C||D).
auto inlineShouldRejectAtEveningSpawn(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    const bool hasEveningFlag = (spawnType & 0x02) != 0;
    const bool inEveningWindow =
        totd == vanadiel_time::TOTD::EVENING || totd == vanadiel_time::TOTD::NIGHT ||
        totd == vanadiel_time::TOTD::MIDNIGHT || totd == vanadiel_time::TOTD::NEWDAY;
    return hasEveningFlag && !inEveningWindow;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectAtEveningSpawn
// (ATEVENING flag && not evening TOTD window; slice 3107).
auto runSpawnRejectEvening3107SelfTests() -> bool
{
    using spawnhelpers::CanSpawnNowPure;
    using spawnhelpers::HasSpawnTypeFlag;
    using spawnhelpers::IsEveningTotdWindow;
    using spawnhelpers::ShouldRejectAtEveningSpawn;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;
    using spawnhelpers::SpawnTypeFog;
    using spawnhelpers::SpawnTypeWeather;

    bool ok = true;

    // Enum / bit pins (formula depends on these exact values).
    ok = expect(SpawnTypeAtEvening == static_cast<uint8>(0x02), "SpawnTypeAtEvening pin 0x02") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NONE) == 0, "TOTD::NONE pin 0") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::MIDNIGHT) == 1, "TOTD::MIDNIGHT pin 1") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NEWDAY) == 2, "TOTD::NEWDAY pin 2") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DAWN) == 3, "TOTD::DAWN pin 3") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DAY) == 4, "TOTD::DAY pin 4") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::DUSK) == 5, "TOTD::DUSK pin 5") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::EVENING) == 6, "TOTD::EVENING pin 6") && ok;
    ok = expect(static_cast<uint8>(vanadiel_time::TOTD::NIGHT) == 7, "TOTD::NIGHT pin 7") && ok;
    ok = expect(IsEveningTotdWindow(vanadiel_time::TOTD::EVENING), "IsEveningTotdWindow EVENING") && ok;
    ok = expect(IsEveningTotdWindow(vanadiel_time::TOTD::NIGHT), "IsEveningTotdWindow NIGHT") && ok;
    ok = expect(IsEveningTotdWindow(vanadiel_time::TOTD::MIDNIGHT), "IsEveningTotdWindow MIDNIGHT") && ok;
    ok = expect(IsEveningTotdWindow(vanadiel_time::TOTD::NEWDAY), "IsEveningTotdWindow NEWDAY") && ok;
    ok = expect(!IsEveningTotdWindow(vanadiel_time::TOTD::DAY), "IsEveningTotdWindow not DAY") && ok;
    ok = expect(!IsEveningTotdWindow(vanadiel_time::TOTD::DAWN), "IsEveningTotdWindow not DAWN") && ok;
    ok = expect(!IsEveningTotdWindow(vanadiel_time::TOTD::DUSK), "IsEveningTotdWindow not DUSK") && ok;
    ok = expect(!IsEveningTotdWindow(vanadiel_time::TOTD::NONE), "IsEveningTotdWindow not NONE") && ok;

    const struct
    {
        uint8               spawnType;
        vanadiel_time::TOTD totd;
        bool                want;
        const char*         label;
    } cases[] = {
        // ATEVENING flag × evening window (allows)
        { SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, false, "ATEVENING + EVENING allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, false, "ATEVENING + NIGHT allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT, false, "ATEVENING + MIDNIGHT allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY, false, "ATEVENING + NEWDAY allows" },

        // ATEVENING flag × non-evening window (rejects)
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, "ATEVENING + DAY rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NONE, true, "ATEVENING + NONE rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAWN, true, "ATEVENING + DAWN rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DUSK, true, "ATEVENING + DUSK rejects" },

        // no ATEVENING flag — never rejects regardless of totd
        { 0x00, vanadiel_time::TOTD::DAY, false, "normal type DAY allows" },
        { 0x00, vanadiel_time::TOTD::EVENING, false, "normal type EVENING allows" },
        { 0x00, vanadiel_time::TOTD::NIGHT, false, "normal type NIGHT allows" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAY, false, "ATNIGHT only DAY allows (evening gate)" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::EVENING, false, "ATNIGHT only EVENING allows (evening gate)" },
        { SpawnTypeFog, vanadiel_time::TOTD::DAY, false, "FOG only DAY allows (evening gate)" },
        { SpawnTypeWeather, vanadiel_time::TOTD::DAY, false, "WEATHER only DAY allows (evening gate)" },

        // combined flags with ATEVENING bit
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|ATEVENING DAY rejects evening gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::EVENING, false,
          "ATNIGHT|ATEVENING EVENING allows evening gate" },
        { static_cast<uint8>(SpawnTypeAtEvening | SpawnTypeFog), vanadiel_time::TOTD::DAY, true,
          "ATEVENING|FOG DAY rejects evening gate" },
        { static_cast<uint8>(SpawnTypeAtEvening | SpawnTypeFog), vanadiel_time::TOTD::EVENING, false,
          "ATEVENING|FOG EVENING allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::DAY, true, "all flags DAY rejects evening gate" },
        { 0xFF, vanadiel_time::TOTD::EVENING, false, "all flags EVENING allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::NIGHT, false, "all flags NIGHT allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::MIDNIGHT, false, "all flags MIDNIGHT allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::NEWDAY, false, "all flags NEWDAY allows evening gate" },

        // bit isolation: 0x02 vs neighbors 0x01 / 0x00 / 0x04
        { 0x02, vanadiel_time::TOTD::DAY, true, "0x02 ATEVENING alone DAY rejects" },
        { 0x02, vanadiel_time::TOTD::EVENING, false, "0x02 ATEVENING alone EVENING allows" },
        { 0x01, vanadiel_time::TOTD::DAY, false, "0x01 ATNIGHT alone DAY allows evening gate" },
        { 0x03, vanadiel_time::TOTD::DAY, true, "0x03 ATNIGHT|ATEVENING DAY rejects evening gate" },
        { 0x03, vanadiel_time::TOTD::EVENING, false, "0x03 ATNIGHT|ATEVENING EVENING allows evening gate" },
        { 0x04, vanadiel_time::TOTD::DAY, false, "0x04 WEATHER alone DAY allows evening gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAtEveningSpawn(c.spawnType, c.totd);
        const bool inlineF = inlineShouldRejectAtEveningSpawn(c.spawnType, c.totd);
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(c.totd);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectAtEveningSpawn dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectAtEveningSpawn == pin formula") && ok;
    }

    // Pin composition: ATEVENING flag is required; evening window alone never rejects.
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::DAY), "no ATEVENING + DAY must not reject") && ok;
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::EVENING), "no ATEVENING + EVENING must not reject") &&
         ok;
    ok = expect(ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY), "ATEVENING + DAY must reject") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING),
                "ATEVENING + EVENING must not reject") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT),
                "ATEVENING + NIGHT must not reject") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT),
                "ATEVENING + MIDNIGHT must not reject") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY),
                "ATEVENING + NEWDAY must not reject") &&
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
            const bool got  = ShouldRejectAtEveningSpawn(st, totd);
            const bool want = HasSpawnTypeFlag(st, SpawnTypeAtEvening) && !IsEveningTotdWindow(totd);
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectAtEveningSpawn(st, totd), "compose free == inline") && ok;
        }
    }

    // CanSpawnNowPure integration: evening gate still composes via free function.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATEVENING + DAY must fail") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, true, false, false, true),
                "CanSpawnNowPure ATEVENING + EVENING must pass evening gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, true, false, false, true),
                "CanSpawnNowPure ATEVENING + NIGHT must pass evening gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT, true, false, false, true),
                "CanSpawnNowPure ATEVENING + MIDNIGHT must pass evening gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY, true, false, false, true),
                "CanSpawnNowPure ATEVENING + NEWDAY must pass evening gate") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, 0, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure normal + DAY must pass") &&
         ok;
    // Sibling night dual-wire still independent of evening dual-wire.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + DAY must still fail night residual") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeAtNight, vanadiel_time::TOTD::NIGHT, true, false, false, true),
                "CanSpawnNowPure ATNIGHT + NIGHT must still pass night residual") &&
         ok;

    return ok;
}
