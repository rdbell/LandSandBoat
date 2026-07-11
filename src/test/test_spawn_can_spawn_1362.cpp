#include "test_spawn_can_spawn_1362.h"

#include "map/spawn_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn can spawn 1362 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "spawn can spawn 1362 self-test failed: " << label << " got "
                  << static_cast<long long>(actual) << " expected " << static_cast<long long>(expected) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runSpawnCanSpawn1362SelfTests() -> bool
{
    using TOTD = vanadiel_time::TOTD;
    bool ok    = true;

    // Constants
    ok = expectEq(spawnhelpers::SpawnTypeAtNight, static_cast<uint8>(0x01), "at night") && ok;
    ok = expectEq(spawnhelpers::SpawnTypeAtEvening, static_cast<uint8>(0x02), "at evening") && ok;
    ok = expectEq(spawnhelpers::SpawnTypeWeather, static_cast<uint8>(0x04), "weather") && ok;
    ok = expectEq(spawnhelpers::SpawnTypeFog, static_cast<uint8>(0x08), "fog") && ok;
    ok = expectEq(spawnhelpers::SpawnWindowSeconds, 15, "window") && ok;
    ok = expectEq(spawnhelpers::SpawnHandlerIntervalSeconds, 30, "interval") && ok;

    // Register guards
    ok = expect(spawnhelpers::ShouldRejectRegisterForRespawn(true, false, false), "reg null") && ok;
    ok = expect(spawnhelpers::ShouldRejectRegisterForRespawn(false, false, false), "reg no allow") && ok;
    ok = expect(spawnhelpers::ShouldRejectRegisterForRespawn(false, true, true), "reg instance") && ok;
    ok = expect(!spawnhelpers::ShouldRejectRegisterForRespawn(false, true, false), "reg ok") && ok;
    ok = expect(spawnhelpers::ShouldUseSpecificMobIdForSlot(true), "specific id") && ok;

    // TOTD windows
    ok = expect(spawnhelpers::IsNightTotdWindow(TOTD::NIGHT), "night") && ok;
    ok = expect(spawnhelpers::IsNightTotdWindow(TOTD::MIDNIGHT), "midnight") && ok;
    ok = expect(!spawnhelpers::IsNightTotdWindow(TOTD::DAY), "not night") && ok;
    ok = expect(spawnhelpers::IsEveningTotdWindow(TOTD::EVENING), "evening") && ok;
    ok = expect(spawnhelpers::IsEveningTotdWindow(TOTD::NEWDAY), "newday evening") && ok;
    ok = expect(!spawnhelpers::IsEveningTotdWindow(TOTD::DAY), "not evening") && ok;

    // Reject gates
    ok = expect(spawnhelpers::ShouldRejectSpawnNullOrDisabled(true, true), "null mob") && ok;
    ok = expect(spawnhelpers::ShouldRejectSpawnNullOrDisabled(false, false), "disabled") && ok;
    ok = expect(spawnhelpers::ShouldRejectAtNightSpawn(0x01, TOTD::DAY), "at night day") && ok;
    ok = expect(!spawnhelpers::ShouldRejectAtNightSpawn(0x01, TOTD::NIGHT), "at night ok") && ok;
    ok = expect(spawnhelpers::ShouldRejectAtEveningSpawn(0x02, TOTD::DAY), "at evening day") && ok;
    ok = expect(!spawnhelpers::ShouldRejectAtEveningSpawn(0x02, TOTD::EVENING), "at evening ok") && ok;
    ok = expect(spawnhelpers::ShouldRejectFogSpawn(0x08, false), "fog not fog") && ok;
    ok = expect(!spawnhelpers::ShouldRejectFogSpawn(0x08, true), "fog ok") && ok;
    ok = expect(spawnhelpers::ShouldCheckWeatherElementSpawn(0x04, true, false), "weather check") && ok;
    ok = expect(!spawnhelpers::ShouldCheckWeatherElementSpawn(0x04, true, true), "weather has master") && ok;
    ok = expect(spawnhelpers::ShouldRejectWeatherElementSpawn(false), "element mismatch") && ok;

    // Full canSpawnNow composition
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x00, TOTD::DAY, true, false, false, true), "normal ok") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(true, true, 0x00, TOTD::DAY, true, false, false, true), "null fail") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(false, false, 0x00, TOTD::DAY, true, false, false, true), "no allow") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(false, true, 0x01, TOTD::DAY, true, false, false, true), "night day fail") && ok;
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x01, TOTD::NIGHT, true, false, false, true), "night ok") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(false, true, 0x02, TOTD::DAY, true, false, false, true), "evening day fail") && ok;
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x02, TOTD::EVENING, true, false, false, true), "evening ok") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(false, true, 0x08, TOTD::DAY, false, false, false, true), "fog fail") && ok;
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x08, TOTD::DAY, true, false, false, true), "fog ok") && ok;
    ok = expect(!spawnhelpers::CanSpawnNowPure(false, true, 0x04, TOTD::DAY, true, true, false, false), "elem mismatch") && ok;
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x04, TOTD::DAY, true, true, false, true), "elem match") && ok;
    ok = expect(spawnhelpers::CanSpawnNowPure(false, true, 0x04, TOTD::DAY, true, true, true, false), "elem has master ignore") && ok;

    // Despawn policy
    ok = expect(spawnhelpers::ShouldDespawnOnNewDay(0x01), "despawn newday") && ok;
    ok = expect(!spawnhelpers::ShouldDespawnOnNewDay(0x02), "no despawn newday") && ok;
    ok = expect(spawnhelpers::ShouldDespawnOnDawn(0x02), "despawn dawn") && ok;
    ok = expect(spawnhelpers::ShouldDespawnElementalOnWeather(true, false, 0x04, false), "despawn elem") && ok;
    ok = expect(!spawnhelpers::ShouldDespawnElementalOnWeather(true, false, 0x04, true), "keep elem") && ok;
    ok = expect(spawnhelpers::ShouldDespawnFogMobOnWeather(0x08, false), "despawn fog") && ok;
    ok = expect(!spawnhelpers::ShouldDespawnFogMobOnWeather(0x08, true), "keep fog") && ok;

    // Tick helpers
    ok = expect(spawnhelpers::IsRespawnDueWithinWindow(false), "due") && ok;
    ok = expect(!spawnhelpers::IsRespawnDueWithinWindow(true), "not due") && ok;
    ok = expect(spawnhelpers::ShouldDropMissingMobRegistration(false), "drop missing") && ok;
    ok = expect(spawnhelpers::ShouldKeepPendingWhenCannotSpawn(false, true), "keep cannot") && ok;
    ok = expect(spawnhelpers::ShouldKeepPendingWhenCannotSpawn(true, false), "keep lua") && ok;
    ok = expect(!spawnhelpers::ShouldKeepPendingWhenCannotSpawn(true, true), "spawn ok") && ok;
    ok = expectEq(spawnhelpers::EntityTargidFromId(0x12345678), static_cast<uint16>(0x0678), "targid") && ok;

    return ok;
}
