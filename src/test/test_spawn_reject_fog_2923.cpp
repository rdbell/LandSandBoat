#include "test_spawn_reject_fog_2923.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldRejectFogSpawn 2923 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline FOG spawn-type weather gate for dual-wire cross-check (slice 2923):
//   (spawnType & SPAWNTYPE_FOG) != 0 && !isFog
//   // SPAWNTYPE_FOG = 0x08
auto inlineShouldRejectFogSpawn(const uint8 spawnType, const bool isFog) -> bool
{
    return (spawnType & 0x08) != 0 && !isFog;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldRejectFogSpawn
// (FOG spawnType flag && weather is not fog; slice 2923).
auto runSpawnRejectFog2923SelfTests() -> bool
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

    const struct
    {
        uint8       spawnType;
        bool        isFog;
        bool        want;
        const char* label;
    } cases[] = {
        // FOG flag set
        { SpawnTypeFog, false, true, "FOG flag not fog weather rejects" },
        { SpawnTypeFog, true, false, "FOG flag fog weather allows" },

        // no FOG flag
        { 0x00, false, false, "normal type not fog allows" },
        { 0x00, true, false, "normal type fog weather allows" },
        { SpawnTypeAtNight, false, false, "ATNIGHT only not fog allows" },
        { SpawnTypeAtEvening, false, false, "ATEVENING only not fog allows" },
        { SpawnTypeWeather, false, false, "WEATHER only not fog allows" },

        // combined flags with FOG bit
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), false, true, "FOG|ATNIGHT not fog rejects" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeAtNight), true, false, "FOG|ATNIGHT fog allows" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), false, true, "FOG|WEATHER not fog rejects" },
        { static_cast<uint8>(SpawnTypeFog | SpawnTypeWeather), true, false, "FOG|WEATHER fog allows" },
        { 0xFF, false, true, "all flags not fog rejects" },
        { 0xFF, true, false, "all flags fog allows" },

        // bit isolation: 0x08 vs neighbors 0x04 / 0x10
        { 0x04, false, false, "0x04 WEATHER alone allows" },
        { 0x08, false, true, "0x08 FOG alone rejects" },
        { 0x10, false, false, "0x10 MoonPhase alone allows" },
        { 0x0C, false, true, "0x0C WEATHER|FOG not fog rejects" },
        { 0x0C, true, false, "0x0C WEATHER|FOG fog allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectFogSpawn(c.spawnType, c.isFog);
        const bool inlineF = inlineShouldRejectFogSpawn(c.spawnType, c.isFog);
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeFog) && !c.isFog;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectFogSpawn dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectFogSpawn == pin formula") && ok;
    }

    // Pin composition: FOG flag is required; isFog alone never rejects.
    ok = expect(!ShouldRejectFogSpawn(0, true), "no FOG flag + isFog must not reject") && ok;
    ok = expect(ShouldRejectFogSpawn(SpawnTypeFog, false), "FOG flag + !isFog must reject") && ok;

    // Dense compose: spawnType bit space x isFog.
    for (int spawnType = 0; spawnType <= 0xFF; ++spawnType)
    {
        for (const bool isFog : { false, true })
        {
            const auto st   = static_cast<uint8>(spawnType);
            const bool got  = ShouldRejectFogSpawn(st, isFog);
            const bool want = HasSpawnTypeFlag(st, SpawnTypeFog) && !isFog;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectFogSpawn(st, isFog), "compose free == inline") && ok;
        }
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
