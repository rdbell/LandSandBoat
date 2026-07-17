#include "test_spawnslot_reject_fog_spawn_3202.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawnslot ShouldRejectFogSpawn 3202 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline FOG spawn-type weather gate for dedicated dual-wire cross-check
// (slice 3202):
//   (spawnType & SPAWNTYPE_FOG) != 0 && !isFog
//   // SPAWNTYPE_FOG = 0x08
auto inlineShouldRejectFogSpawn3202(const uint8 spawnType, const bool isFog) -> bool
{
    return (spawnType & 0x08) != 0 && !isFog;
}

// Compact dual-wire pin matching Go pinShouldRejectFogSpawn3202 / C++ capacity
// positive form (formula unchanged from 1362 / 2923):
//   HasSpawnTypeFlag(spawnType, SpawnTypeFog) && !isFog
auto pinShouldRejectFogSpawn3202(const uint8 spawnType, const bool isFog) -> bool
{
    return spawnhelpers::HasSpawnTypeFlag(spawnType, spawnhelpers::SpawnTypeFog) && !isFog;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectFogSpawn
// (FOG spawnType flag && weather is not fog;
// OmegaXI internal/spawnslot; dedicated slice 3202; residual expand 2923 / pure 1362).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2923 pins still hold
//   - poles: fog-required + not fog vs fog weather
//   - dense spawnType×isFog free == inline == pin
//   - host inject poles
auto runSpawnslotRejectFogSpawn3202SelfTests() -> bool
{
    using spawnhelpers::CanSpawnNowPure;
    using spawnhelpers::HasSpawnTypeFlag;
    using spawnhelpers::ShouldRejectFogSpawn;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;
    using spawnhelpers::SpawnTypeFog;
    using spawnhelpers::SpawnTypeWeather;

    bool ok = true;

    ok = expect(SpawnTypeFog == static_cast<uint8>(0x08), "SpawnTypeFog pin 0x08") && ok;

    // Residual 2923 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRejectFogSpawn(SpawnTypeFog, false), "residual FOG + !isFog rejects") && ok;
    ok = expect(!ShouldRejectFogSpawn(SpawnTypeFog, true), "residual FOG + isFog allows") && ok;
    ok = expect(!ShouldRejectFogSpawn(0, false), "residual no FOG + !isFog allows") && ok;
    ok = expect(!ShouldRejectFogSpawn(0, true), "residual no FOG + isFog allows") && ok;

    // --- Core poles: free == inline == pin positive form ---
    // Fog-required + not fog vs fog weather.
    const struct
    {
        uint8       spawnType;
        bool        isFog;
        bool        want;
        const char* label;
    } cases[] = {
        // Fog-required classic reject: FOG flag + not fog weather.
        { SpawnTypeFog, false, true, "fog-required + not fog rejects" },
        // Fog weather allows FOG-type spawn.
        { SpawnTypeFog, true, false, "fog-required + fog weather allows" },

        // Flag required: isFog alone never rejects.
        { 0x00, false, false, "no FOG + not fog allows" },
        { 0x00, true, false, "no FOG + fog weather allows" },

        // Sibling spawn-type bits alone do not trip the fog gate.
        { SpawnTypeAtNight, false, false, "ATNIGHT only not fog allows" },
        { SpawnTypeAtEvening, false, false, "ATEVENING only not fog allows" },
        { SpawnTypeWeather, false, false, "WEATHER only not fog allows" },

        // Combined flags: FOG bit still gates.
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), false, true, "FOG|ATNIGHT not fog rejects" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), true, false, "FOG|ATNIGHT fog weather allows" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), false, true, "FOG|WEATHER not fog rejects" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), true, false, "FOG|WEATHER fog weather allows" },

        // Bit isolation: 0x08 vs neighbors 0x04 / 0x10.
        { 0x04, false, false, "0x04 WEATHER alone allows" },
        { 0x08, false, true, "0x08 FOG alone rejects" },
        { 0x10, false, false, "0x10 MoonPhase alone allows" },
        { 0x0C, false, true, "0x0C WEATHER|FOG not fog rejects" },
        { 0x0C, true, false, "0x0C WEATHER|FOG fog weather allows" },
        { 0xFF, false, true, "all flags not fog rejects" },
        { 0xFF, true, false, "all flags fog weather allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectFogSpawn(c.spawnType, c.isFog);
        const bool inlineF = inlineShouldRejectFogSpawn3202(c.spawnType, c.isFog);
        const bool pin     = pinShouldRejectFogSpawn3202(c.spawnType, c.isFog);
        // Positive form pin composition (FOG flag && !isFog).
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeFog) && !c.isFog;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRejectFogSpawn free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual fog-required poles.
    ok = expect(ShouldRejectFogSpawn(SpawnTypeFog, false) == pinShouldRejectFogSpawn3202(SpawnTypeFog, false),
                "free == pin fog-required + not fog") &&
         ok;
    ok = expect(ShouldRejectFogSpawn(SpawnTypeFog, true) == pinShouldRejectFogSpawn3202(SpawnTypeFog, true),
                "free == pin fog-required + fog weather") &&
         ok;
    ok = expect(ShouldRejectFogSpawn(0, false) == pinShouldRejectFogSpawn3202(0, false),
                "free == pin no FOG + not fog") &&
         ok;
    ok = expect(ShouldRejectFogSpawn(0, true) == pinShouldRejectFogSpawn3202(0, true),
                "free == pin no FOG + fog weather") &&
         ok;

    // Dense compose: spawnType bit space x isFog free == inline == pin.
    for (int spawnType = 0; spawnType <= 0xFF; ++spawnType)
    {
        for (const bool isFog : { false, true })
        {
            const auto st      = static_cast<uint8>(spawnType);
            const bool got     = ShouldRejectFogSpawn(st, isFog);
            const bool inlineF = inlineShouldRejectFogSpawn3202(st, isFog);
            const bool pin     = pinShouldRejectFogSpawn3202(st, isFog);
            const bool want    = HasSpawnTypeFlag(st, SpawnTypeFog) && !isFog;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production canSpawnNow path semantics ---
    // Host injects:
    //   isFog = zone_->weather().current() == Weather::Fog
    // when true (FOG + !isFog) → reject spawn
    // when false               → fog gate passes
    ok = expect(ShouldRejectFogSpawn(SpawnTypeFog, false) && pinShouldRejectFogSpawn3202(SpawnTypeFog, false),
                "host FOG + not fog → reject path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRejectFogSpawn(SpawnTypeFog, true), "host FOG + fog weather → allow") && ok;
    ok = expect(!ShouldRejectFogSpawn(0, false), "host no FOG + not fog → allow") && ok;
    ok = expect(!ShouldRejectFogSpawn(0, true), "host no FOG + fog weather → allow") && ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        uint8       spawnType;
        bool        isFog;
        const char* label;
    } hostPoles[] = {
        { SpawnTypeFog, false, "FOG + not fog reject" },
        { SpawnTypeFog, true, "FOG + fog weather allow" },
        { 0x00, false, "no FOG + not fog allow" },
        { 0x00, true, "no FOG + fog weather allow" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRejectFogSpawn(p.spawnType, p.isFog);
        const bool inlineF = inlineShouldRejectFogSpawn3202(p.spawnType, p.isFog);
        const bool pin     = pinShouldRejectFogSpawn3202(p.spawnType, p.isFog);
        const bool want    = HasSpawnTypeFlag(p.spawnType, SpawnTypeFog) && !p.isFog;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // CanSpawnNowPure integration: fog gate still composes via free function.
    ok = expect(!CanSpawnNowPure(false, true, SpawnTypeFog, vanadiel_time::TOTD::DAY, false, false, false, true),
                "CanSpawnNowPure FOG + !isFog must fail") &&
         ok;
    ok = expect(CanSpawnNowPure(false, true, SpawnTypeFog, vanadiel_time::TOTD::DAY, true, false, false, true),
                "CanSpawnNowPure FOG + isFog must pass fog gate") &&
         ok;

    return ok;
}
