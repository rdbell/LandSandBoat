#include "test_spawn_reject_evening_3341.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectAtEveningSpawn 3341 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ATEVENING spawn-type TOTD gate for dedicated dual-wire cross-check
// (slice 3341):
//   (spawnType & SPAWNTYPE_ATEVENING) != 0 && !(totd in evening window)
//   // SPAWNTYPE_ATEVENING = 0x02
//   // evening window: EVENING || NIGHT || MIDNIGHT || NEWDAY
//   // TOTD: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
// Positive form avoids De Morgan rewrite of !(A||B||C||D).
auto inlineShouldRejectAtEveningSpawn3341(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    const bool hasEveningFlag = (spawnType & 0x02) != 0;
    const bool inEveningWindow =
        totd == vanadiel_time::TOTD::EVENING || totd == vanadiel_time::TOTD::NIGHT ||
        totd == vanadiel_time::TOTD::MIDNIGHT || totd == vanadiel_time::TOTD::NEWDAY;
    return hasEveningFlag && !inEveningWindow;
}

// Compact dual-wire pin matching Go pinShouldRejectAtEveningSpawn3341 / C++
// capacity positive form (formula unchanged from 1362 / 3107):
//   HasSpawnTypeFlag(spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(totd)
auto pinShouldRejectAtEveningSpawn3341(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return spawnhelpers::HasSpawnTypeFlag(spawnType, spawnhelpers::SpawnTypeAtEvening) &&
           !spawnhelpers::IsEveningTotdWindow(totd);
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectAtEveningSpawn
// (ATEVENING flag && not evening TOTD window;
// OmegaXI internal/spawnslot; dedicated slice 3341; residual expand 3107 / pure 1362).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 3107 / 1362 pins still hold
//   - poles: ATEVENING + DAY rejects vs evening window allows
//   - dense spawnType×TOTD free == inline == pin
//   - host inject poles
auto runSpawnRejectEvening3341SelfTests() -> bool
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

    // Residual 3107 / 1362 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY),
                "residual ATEVENING + DAY rejects") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING),
                "residual ATEVENING + EVENING allows") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT),
                "residual ATEVENING + NIGHT allows") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT),
                "residual ATEVENING + MIDNIGHT allows") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY),
                "residual ATEVENING + NEWDAY allows") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::DAY), "residual no ATEVENING + DAY allows") && ok;
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::EVENING),
                "residual no ATEVENING + EVENING allows") &&
         ok;

    // --- Core poles: free == inline == pin positive form ---
    // ATEVENING-required + non-evening window vs evening window.
    const struct
    {
        uint8               spawnType;
        vanadiel_time::TOTD totd;
        bool                want;
        const char*         label;
    } cases[] = {
        // ATEVENING-required classic reject: ATEVENING flag + DAY.
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, true, "ATEVENING-required + DAY rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NONE, true, "ATEVENING-required + NONE rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAWN, true, "ATEVENING-required + DAWN rejects" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DUSK, true, "ATEVENING-required + DUSK rejects" },
        // Evening window allows ATEVENING-type spawn.
        { SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, false, "ATEVENING-required + EVENING allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, false, "ATEVENING-required + NIGHT allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT, false, "ATEVENING-required + MIDNIGHT allows" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY, false, "ATEVENING-required + NEWDAY allows" },

        // Flag required: evening window alone never rejects.
        { 0x00, vanadiel_time::TOTD::DAY, false, "no ATEVENING + DAY allows" },
        { 0x00, vanadiel_time::TOTD::EVENING, false, "no ATEVENING + EVENING allows" },
        { 0x00, vanadiel_time::TOTD::NIGHT, false, "no ATEVENING + NIGHT allows" },

        // Sibling spawn-type bits alone do not trip the evening gate.
        { SpawnTypeAtNight, vanadiel_time::TOTD::DAY, false, "ATNIGHT only DAY allows (evening gate)" },
        { SpawnTypeAtNight, vanadiel_time::TOTD::EVENING, false, "ATNIGHT only EVENING allows (evening gate)" },
        { SpawnTypeFog, vanadiel_time::TOTD::DAY, false, "FOG only DAY allows (evening gate)" },
        { SpawnTypeWeather, vanadiel_time::TOTD::DAY, false, "WEATHER only DAY allows (evening gate)" },

        // Combined flags: ATEVENING bit still gates.
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::DAY, true,
          "ATNIGHT|ATEVENING DAY rejects evening gate" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), vanadiel_time::TOTD::EVENING, false,
          "ATNIGHT|ATEVENING EVENING allows evening gate" },
        { static_cast<uint8>(SpawnTypeAtEvening | SpawnTypeFog), vanadiel_time::TOTD::DAY, true,
          "ATEVENING|FOG DAY rejects evening gate" },
        { static_cast<uint8>(SpawnTypeAtEvening | SpawnTypeFog), vanadiel_time::TOTD::EVENING, false,
          "ATEVENING|FOG EVENING allows evening gate" },

        // Bit isolation: 0x02 vs neighbors 0x01 / 0x00 / 0x04.
        { 0x01, vanadiel_time::TOTD::DAY, false, "0x01 ATNIGHT alone DAY allows evening gate" },
        { 0x02, vanadiel_time::TOTD::DAY, true, "0x02 ATEVENING alone DAY rejects" },
        { 0x02, vanadiel_time::TOTD::EVENING, false, "0x02 ATEVENING alone EVENING allows" },
        { 0x04, vanadiel_time::TOTD::DAY, false, "0x04 WEATHER alone DAY allows evening gate" },
        { 0x03, vanadiel_time::TOTD::DAY, true, "0x03 ATNIGHT|ATEVENING DAY rejects evening gate" },
        { 0x03, vanadiel_time::TOTD::EVENING, false, "0x03 ATNIGHT|ATEVENING EVENING allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::DAY, true, "all flags DAY rejects evening gate" },
        { 0xFF, vanadiel_time::TOTD::EVENING, false, "all flags EVENING allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::NIGHT, false, "all flags NIGHT allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::MIDNIGHT, false, "all flags MIDNIGHT allows evening gate" },
        { 0xFF, vanadiel_time::TOTD::NEWDAY, false, "all flags NEWDAY allows evening gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAtEveningSpawn(c.spawnType, c.totd);
        const bool inlineF = inlineShouldRejectAtEveningSpawn3341(c.spawnType, c.totd);
        const bool pin     = pinShouldRejectAtEveningSpawn3341(c.spawnType, c.totd);
        // Positive form pin composition (ATEVENING flag && !evening window).
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(c.totd);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRejectAtEveningSpawn free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual ATEVENING-required poles.
    ok = expect(ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY) ==
                    pinShouldRejectAtEveningSpawn3341(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY),
                "free == pin ATEVENING-required + DAY") &&
         ok;
    ok = expect(ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING) ==
                    pinShouldRejectAtEveningSpawn3341(SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING),
                "free == pin ATEVENING-required + EVENING") &&
         ok;
    ok = expect(ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::DAY) ==
                    pinShouldRejectAtEveningSpawn3341(0, vanadiel_time::TOTD::DAY),
                "free == pin no ATEVENING + DAY") &&
         ok;
    ok = expect(ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::EVENING) ==
                    pinShouldRejectAtEveningSpawn3341(0, vanadiel_time::TOTD::EVENING),
                "free == pin no ATEVENING + EVENING") &&
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
            const bool got     = ShouldRejectAtEveningSpawn(st, totd);
            const bool inlineF = inlineShouldRejectAtEveningSpawn3341(st, totd);
            const bool pin     = pinShouldRejectAtEveningSpawn3341(st, totd);
            const bool want    = HasSpawnTypeFlag(st, SpawnTypeAtEvening) && !IsEveningTotdWindow(totd);
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production canSpawnNow path semantics ---
    // Host injects:
    //   totd = current vanadiel_time::TOTD
    // when true (ATEVENING + !evening window) → reject spawn
    // when false                             → evening gate passes
    ok = expect(ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY) &&
                    pinShouldRejectAtEveningSpawn3341(SpawnTypeAtEvening, vanadiel_time::TOTD::DAY),
                "host ATEVENING + DAY → reject path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING),
                "host ATEVENING + EVENING → allow") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT),
                "host ATEVENING + NIGHT → allow") &&
         ok;
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::DAY), "host no ATEVENING + DAY → allow") && ok;
    ok = expect(!ShouldRejectAtEveningSpawn(0, vanadiel_time::TOTD::EVENING),
                "host no ATEVENING + EVENING → allow") &&
         ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        uint8               spawnType;
        vanadiel_time::TOTD totd;
        const char*         label;
    } hostPoles[] = {
        { SpawnTypeAtEvening, vanadiel_time::TOTD::DAY, "ATEVENING + DAY reject" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::EVENING, "ATEVENING + EVENING allow" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NIGHT, "ATEVENING + NIGHT allow" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::MIDNIGHT, "ATEVENING + MIDNIGHT allow" },
        { SpawnTypeAtEvening, vanadiel_time::TOTD::NEWDAY, "ATEVENING + NEWDAY allow" },
        { 0x00, vanadiel_time::TOTD::DAY, "no ATEVENING + DAY allow" },
        { 0x00, vanadiel_time::TOTD::EVENING, "no ATEVENING + EVENING allow" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectAtEveningSpawn(p.spawnType, p.totd);
        const bool inlineF = inlineShouldRejectAtEveningSpawn3341(p.spawnType, p.totd);
        const bool pin     = pinShouldRejectAtEveningSpawn3341(p.spawnType, p.totd);
        const bool want    = HasSpawnTypeFlag(p.spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(p.totd);
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
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
