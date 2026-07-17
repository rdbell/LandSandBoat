#include "test_spawnslot_despawn_fog_3261.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawnslot ShouldDespawnFogMobOnWeather 3261 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline FOG spawn-type weather despawn gate for dedicated dual-wire cross-check
// (slice 3261):
//   (spawnType & SPAWNTYPE_FOG) != 0 && !isFog
//   // SPAWNTYPE_FOG = 0x08
auto inlineShouldDespawnFogMobOnWeather3261(const uint8 spawnType, const bool isFog) -> bool
{
    return (spawnType & 0x08) != 0 && !isFog;
}

// Compact dual-wire pin matching Go pinShouldDespawnFogMobOnWeather3261 / C++
// capacity positive form (formula unchanged from 1362 / residual 2923 shape;
// prior dedicated spawn-gate dual-wire ~3202):
//   HasSpawnTypeFlag(spawnType, SpawnTypeFog) && !isFog
auto pinShouldDespawnFogMobOnWeather3261(const uint8 spawnType, const bool isFog) -> bool
{
    return spawnhelpers::HasSpawnTypeFlag(spawnType, spawnhelpers::SpawnTypeFog) && !isFog;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldDespawnFogMobOnWeather
// (FOG spawnType flag && weather is not fog;
// OmegaXI internal/spawnslot; dedicated slice 3261; residual expand 2923 /
// pure 1362; prior dedicated spawn-gate dual-wire ~3202).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2923 / 1362 poles still hold
//   - poles: fog-required + not fog vs fog weather
//   - dense spawnType×isFog free == inline == pin
//   - host inject poles
auto runSpawnslotDespawnFog3261SelfTests() -> bool
{
    using spawnhelpers::HasSpawnTypeFlag;
    using spawnhelpers::ShouldDespawnFogMobOnWeather;
    using spawnhelpers::ShouldRejectFogSpawn;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;
    using spawnhelpers::SpawnTypeFog;
    using spawnhelpers::SpawnTypeWeather;

    bool ok = true;

    ok = expect(SpawnTypeFog == static_cast<uint8>(0x08), "SpawnTypeFog pin 0x08") && ok;

    // Residual 2923 / 1362 poles still hold under dedicated dual-wire.
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, false), "residual FOG + !isFog despawns") && ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(SpawnTypeFog, true), "residual FOG + isFog keeps") && ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(0, false), "residual no FOG + !isFog keeps") && ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(0, true), "residual no FOG + isFog keeps") && ok;

    // --- Core poles: free == inline == pin positive form ---
    // Fog-required + not fog vs fog weather.
    const struct
    {
        uint8       spawnType;
        bool        isFog;
        bool        want;
        const char* label;
    } cases[] = {
        // Fog-required classic despawn: FOG flag + not fog weather.
        { SpawnTypeFog, false, true, "fog-required + not fog despawns" },
        // Fog weather keeps FOG-type mob.
        { SpawnTypeFog, true, false, "fog-required + fog weather keeps" },

        // Flag required: isFog alone never despawns.
        { 0x00, false, false, "no FOG + not fog keeps" },
        { 0x00, true, false, "no FOG + fog weather keeps" },

        // Sibling spawn-type bits alone do not trip the fog despawn gate.
        { SpawnTypeAtNight, false, false, "ATNIGHT only not fog keeps" },
        { SpawnTypeAtEvening, false, false, "ATEVENING only not fog keeps" },
        { SpawnTypeWeather, false, false, "WEATHER only not fog keeps" },

        // Combined flags: FOG bit still gates.
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), false, true, "FOG|ATNIGHT not fog despawns" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), true, false, "FOG|ATNIGHT fog weather keeps" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), false, true, "FOG|WEATHER not fog despawns" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), true, false, "FOG|WEATHER fog weather keeps" },

        // Bit isolation: 0x08 vs neighbors 0x04 / 0x10.
        { 0x04, false, false, "0x04 WEATHER alone keeps" },
        { 0x08, false, true, "0x08 FOG alone despawns" },
        { 0x10, false, false, "0x10 MoonPhase alone keeps" },
        { 0x0C, false, true, "0x0C WEATHER|FOG not fog despawns" },
        { 0x0C, true, false, "0x0C WEATHER|FOG fog weather keeps" },
        { 0xFF, false, true, "all flags not fog despawns" },
        { 0xFF, true, false, "all flags fog weather keeps" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnFogMobOnWeather(c.spawnType, c.isFog);
        const bool inlineF = inlineShouldDespawnFogMobOnWeather3261(c.spawnType, c.isFog);
        const bool pin     = pinShouldDespawnFogMobOnWeather3261(c.spawnType, c.isFog);
        // Positive form pin composition (FOG flag && !isFog).
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeFog) && !c.isFog;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldDespawnFogMobOnWeather free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual fog-required poles.
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, false) ==
                    pinShouldDespawnFogMobOnWeather3261(SpawnTypeFog, false),
                "free == pin fog-required + not fog") &&
         ok;
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, true) ==
                    pinShouldDespawnFogMobOnWeather3261(SpawnTypeFog, true),
                "free == pin fog-required + fog weather") &&
         ok;
    ok = expect(ShouldDespawnFogMobOnWeather(0, false) == pinShouldDespawnFogMobOnWeather3261(0, false),
                "free == pin no FOG + not fog") &&
         ok;
    ok = expect(ShouldDespawnFogMobOnWeather(0, true) == pinShouldDespawnFogMobOnWeather3261(0, true),
                "free == pin no FOG + fog weather") &&
         ok;

    // Dense compose: spawnType bit space x isFog free == inline == pin.
    for (int spawnType = 0; spawnType <= 0xFF; ++spawnType)
    {
        for (const bool isFog : { false, true })
        {
            const auto st      = static_cast<uint8>(spawnType);
            const bool got     = ShouldDespawnFogMobOnWeather(st, isFog);
            const bool inlineF = inlineShouldDespawnFogMobOnWeather3261(st, isFog);
            const bool pin     = pinShouldDespawnFogMobOnWeather3261(st, isFog);
            const bool want    = HasSpawnTypeFlag(st, SpawnTypeFog) && !isFog;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production onWeatherChange path semantics ---
    // Host injects:
    //   isFog = weather == Weather::Fog
    // when true (FOG + !isFog) → despawn
    // when false               → keep
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, false) &&
                    pinShouldDespawnFogMobOnWeather3261(SpawnTypeFog, false),
                "host FOG + not fog → despawn path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(SpawnTypeFog, true), "host FOG + fog weather → keep") && ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(0, false), "host no FOG + not fog → keep") && ok;
    ok = expect(!ShouldDespawnFogMobOnWeather(0, true), "host no FOG + fog weather → keep") && ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        uint8       spawnType;
        bool        isFog;
        const char* label;
    } hostPoles[] = {
        { SpawnTypeFog, false, "FOG + not fog despawn" },
        { SpawnTypeFog, true, "FOG + fog weather keep" },
        { 0x00, false, "no FOG + not fog keep" },
        { 0x00, true, "no FOG + fog weather keep" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldDespawnFogMobOnWeather(p.spawnType, p.isFog);
        const bool inlineF = inlineShouldDespawnFogMobOnWeather3261(p.spawnType, p.isFog);
        const bool pin     = pinShouldDespawnFogMobOnWeather3261(p.spawnType, p.isFog);
        const bool want    = HasSpawnTypeFlag(p.spawnType, SpawnTypeFog) && !p.isFog;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Same-formula residual: spawn-gate free function still matches despawn shape.
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, false) == ShouldRejectFogSpawn(SpawnTypeFog, false),
                "despawn fog free == reject fog free FOG + !isFog") &&
         ok;
    ok = expect(ShouldDespawnFogMobOnWeather(SpawnTypeFog, true) == ShouldRejectFogSpawn(SpawnTypeFog, true),
                "despawn fog free == reject fog free FOG + isFog") &&
         ok;

    return ok;
}
