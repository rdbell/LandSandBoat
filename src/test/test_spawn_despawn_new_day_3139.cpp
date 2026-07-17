#include "test_spawn_despawn_new_day_3139.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn ShouldDespawnOnNewDay 3139 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ATNIGHT NEWDAY despawn gate for dual-wire cross-check (slice 3139):
//   (spawnType & SPAWNTYPE_ATNIGHT) != 0
//   // SPAWNTYPE_ATNIGHT = 0x01
auto inlineShouldDespawnOnNewDay(const uint8 spawnType) -> bool
{
    return (spawnType & 0x01) != 0;
}

} // namespace

// Pure dual-wire expansion for spawnhelpers::ShouldDespawnOnNewDay
// (SPAWNTYPE_ATNIGHT flag on NEWDAY; slice 3139).
auto runSpawnDespawnNewDay3139SelfTests() -> bool
{
    using spawnhelpers::HasSpawnTypeFlag;
    using spawnhelpers::ShouldDespawnOnDawn;
    using spawnhelpers::ShouldDespawnOnNewDay;
    using spawnhelpers::SpawnTypeAtEvening;
    using spawnhelpers::SpawnTypeAtNight;
    using spawnhelpers::SpawnTypeFog;
    using spawnhelpers::SpawnTypeWeather;

    bool ok = true;

    // Bit pin (formula depends on this exact value).
    ok = expect(SpawnTypeAtNight == static_cast<uint8>(0x01), "SpawnTypeAtNight pin 0x01") && ok;
    ok = expect(SpawnTypeAtEvening == static_cast<uint8>(0x02), "SpawnTypeAtEvening pin 0x02") && ok;

    const struct
    {
        uint8       spawnType;
        bool        want;
        const char* label;
    } cases[] = {
        // ATNIGHT flag on → despawn
        { SpawnTypeAtNight, true, "ATNIGHT alone despawns" },
        { 0x01, true, "0x01 ATNIGHT alone despawns" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeAtEvening), true, "ATNIGHT|ATEVENING despawns" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeFog), true, "ATNIGHT|FOG despawns" },
        { static_cast<uint8>(SpawnTypeAtNight | SpawnTypeWeather), true, "ATNIGHT|WEATHER despawns" },
        { 0xFF, true, "all flags despawns" },
        { 0x03, true, "0x03 ATNIGHT|ATEVENING despawns" },

        // ATNIGHT flag off → keep
        { 0x00, false, "normal type keeps" },
        { SpawnTypeAtEvening, false, "ATEVENING only keeps (newday gate)" },
        { SpawnTypeFog, false, "FOG only keeps (newday gate)" },
        { SpawnTypeWeather, false, "WEATHER only keeps (newday gate)" },
        { 0x02, false, "0x02 ATEVENING alone keeps" },
        { 0xFE, false, "0xFE all except ATNIGHT keeps" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDespawnOnNewDay(c.spawnType);
        const bool inlineF = inlineShouldDespawnOnNewDay(c.spawnType);
        const bool wantPin = HasSpawnTypeFlag(c.spawnType, SpawnTypeAtNight);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDespawnOnNewDay dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDespawnOnNewDay == pin formula") && ok;
    }

    // Pin composition: ATNIGHT flag is required and sufficient.
    ok = expect(!ShouldDespawnOnNewDay(0), "no ATNIGHT flag must not despawn") && ok;
    ok = expect(ShouldDespawnOnNewDay(SpawnTypeAtNight), "ATNIGHT flag must despawn") && ok;
    ok = expect(!ShouldDespawnOnNewDay(SpawnTypeAtEvening), "ATEVENING alone must not despawn on NEWDAY") && ok;

    // Dense compose: spawnType bit space free==inline==pin.
    for (int spawnType = 0; spawnType <= 0xFF; ++spawnType)
    {
        const auto st   = static_cast<uint8>(spawnType);
        const bool got  = ShouldDespawnOnNewDay(st);
        const bool want = HasSpawnTypeFlag(st, SpawnTypeAtNight);
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDespawnOnNewDay(st), "compose free == inline") && ok;
    }

    // Residual 1362 pins still hold under dual-wire.
    ok = expect(ShouldDespawnOnNewDay(SpawnTypeAtNight), "residual 1362 ATNIGHT despawns") && ok;
    ok = expect(!ShouldDespawnOnNewDay(SpawnTypeAtEvening), "residual 1362 ATEVENING keeps") && ok;

    // Sibling residual ShouldDespawnOnDawn still independent of NEWDAY dual-wire.
    ok = expect(ShouldDespawnOnDawn(SpawnTypeAtEvening), "ShouldDespawnOnDawn residual: ATEVENING despawns") && ok;
    ok = expect(!ShouldDespawnOnDawn(SpawnTypeAtNight), "ShouldDespawnOnDawn residual: ATNIGHT alone keeps") && ok;
    // Cross polarity: NEWDAY fires on ATNIGHT only; DAWN fires on ATEVENING only.
    ok = expect(!ShouldDespawnOnNewDay(SpawnTypeAtEvening), "NEWDAY dual-wire must not fire on ATEVENING alone") && ok;
    ok = expect(!ShouldDespawnOnDawn(SpawnTypeAtNight), "DAWN residual must not fire on ATNIGHT alone") && ok;

    return ok;
}
