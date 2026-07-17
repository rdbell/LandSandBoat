#pragma once

#include "common/cbasetypes.h"
#include "common/vana_time.h"
#include "entities/mob_entity.h"
#include "map_constants.h"

#include <cstdint>

// Pure SpawnHandler canSpawnNow / TOTD / weather despawn policy for native tests.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1362: canSpawnNow / TOTD / weather despawn / tick / register policy
//   - 2923: ShouldRejectFogSpawn (FOG spawnType flag && weather is not fog)
//
// Production host: SpawnHandler::canSpawnNow (spawn_handler.cpp) injects
// isFog = zone_->weather().current() == Weather::Fog before CanSpawnNowPure.
// Go dual-wire: spawnslot.ShouldRejectFogSpawn (internal/spawnslot/reject_fog.go).

namespace spawnhelpers
{

// SPAWNTYPE bit mirrors (from mob_entity.h).
constexpr uint8 SpawnTypeNormal    = SPAWNTYPE_NORMAL;
constexpr uint8 SpawnTypeAtNight   = SPAWNTYPE_ATNIGHT;
constexpr uint8 SpawnTypeAtEvening = SPAWNTYPE_ATEVENING;
constexpr uint8 SpawnTypeWeather   = SPAWNTYPE_WEATHER;
constexpr uint8 SpawnTypeFog       = SPAWNTYPE_FOG;

// ShouldRejectSpawnNullOrDisabled mirrors !PMob || !m_AllowRespawn.
inline auto ShouldRejectSpawnNullOrDisabled(const bool mobNull, const bool allowRespawn) -> bool
{
    return mobNull || !allowRespawn;
}

// HasSpawnTypeFlag mirrors spawnType & flag.
inline auto HasSpawnTypeFlag(const uint8 spawnType, const uint8 flag) -> bool
{
    return (spawnType & flag) != 0;
}

// IsNightTotdWindow mirrors NIGHT or MIDNIGHT (20:00-04:00 for ATNIGHT).
inline auto IsNightTotdWindow(const vanadiel_time::TOTD totd) -> bool
{
    return totd == vanadiel_time::TOTD::NIGHT || totd == vanadiel_time::TOTD::MIDNIGHT;
}

// IsEveningTotdWindow mirrors EVENING, NIGHT, MIDNIGHT, NEWDAY (18:00-06:00).
inline auto IsEveningTotdWindow(const vanadiel_time::TOTD totd) -> bool
{
    return totd == vanadiel_time::TOTD::EVENING ||
           totd == vanadiel_time::TOTD::NIGHT ||
           totd == vanadiel_time::TOTD::MIDNIGHT ||
           totd == vanadiel_time::TOTD::NEWDAY;
}

// ShouldRejectAtNightSpawn mirrors ATNIGHT flag && not night window.
inline auto ShouldRejectAtNightSpawn(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(totd);
}

// ShouldRejectAtEveningSpawn mirrors ATEVENING flag && not evening window.
inline auto ShouldRejectAtEveningSpawn(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(totd);
}

// ShouldRejectFogSpawn mirrors FOG flag && weather is not fog.
// isFog is host-evaluated weather == Weather::Fog.
//
// Formula (slice 2923 dual-wire):
//   HasSpawnTypeFlag(spawnType, SpawnTypeFog) && !isFog
//   // SpawnTypeFog = 0x08
//
// true  → reject spawn (FOG-type mob outside fog weather)
// false → fog gate passes (no FOG flag, or weather is fog)
//
// Dual-wire of Go spawnslot.ShouldRejectFogSpawn.
// Call site: CanSpawnNowPure (and SpawnHandler::canSpawnNow host inject).
inline auto ShouldRejectFogSpawn(const uint8 spawnType, const bool isFog) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeFog) && !isFog;
}

// ShouldCheckWeatherElementSpawn mirrors WEATHER flag && elemental without master.
inline auto ShouldCheckWeatherElementSpawn(
    const uint8 spawnType,
    const bool  isElemental,
    const bool  hasMaster) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeWeather) && isElemental && !hasMaster;
}

// ShouldRejectWeatherElementSpawn mirrors element != weather element.
inline auto ShouldRejectWeatherElementSpawn(const bool elementMatchesWeather) -> bool
{
    return !elementMatchesWeather;
}

// CanSpawnNowPure composes all canSpawnNow gates with host injects.
// elementMatchesWeather is only consulted when ShouldCheckWeatherElementSpawn is true;
// callers may pass true when not applicable.
inline auto CanSpawnNowPure(
    const bool                mobNull,
    const bool                allowRespawn,
    const uint8               spawnType,
    const vanadiel_time::TOTD totd,
    const bool                isFog,
    const bool                isElemental,
    const bool                hasMaster,
    const bool                elementMatchesWeather) -> bool
{
    if (ShouldRejectSpawnNullOrDisabled(mobNull, allowRespawn))
    {
        return false;
    }
    if (ShouldRejectAtNightSpawn(spawnType, totd))
    {
        return false;
    }
    if (ShouldRejectAtEveningSpawn(spawnType, totd))
    {
        return false;
    }
    if (ShouldRejectFogSpawn(spawnType, isFog))
    {
        return false;
    }
    if (ShouldCheckWeatherElementSpawn(spawnType, isElemental, hasMaster) &&
        ShouldRejectWeatherElementSpawn(elementMatchesWeather))
    {
        return false;
    }
    return true;
}

// --- TOTD despawn policy ---

// ShouldDespawnOnNewDay mirrors SPAWNTYPE_ATNIGHT on NEWDAY.
inline auto ShouldDespawnOnNewDay(const uint8 spawnType) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtNight);
}

// ShouldDespawnOnDawn mirrors SPAWNTYPE_ATEVENING on DAWN.
inline auto ShouldDespawnOnDawn(const uint8 spawnType) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtEvening);
}

// --- Weather despawn policy ---

// ShouldDespawnElementalOnWeather mirrors elemental, no master, WEATHER type, element mismatch.
inline auto ShouldDespawnElementalOnWeather(
    const bool  isElemental,
    const bool  hasMaster,
    const uint8 spawnType,
    const bool  elementMatchesWeather) -> bool
{
    return isElemental && !hasMaster && HasSpawnTypeFlag(spawnType, SpawnTypeWeather) && !elementMatchesWeather;
}

// ShouldDespawnFogMobOnWeather mirrors FOG type && weather != Fog.
inline auto ShouldDespawnFogMobOnWeather(const uint8 spawnType, const bool isFog) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeFog) && !isFog;
}

// --- Tick pure halves ---

// IsRespawnDueWithinWindow mirrors respawnAt <= now + spawnWindow.
inline auto IsRespawnDueWithinWindow(const bool respawnAtAfterThreshold) -> bool
{
    // pair.second > spawnThreshold → not due; invert for "due".
    return !respawnAtAfterThreshold;
}

// ShouldDropMissingMobRegistration mirrors !PMob after entity lookup.
inline auto ShouldDropMissingMobRegistration(const bool mobFound) -> bool
{
    return !mobFound;
}

// ShouldKeepPendingWhenCannotSpawn mirrors !canSpawnNow || lua cancel != 0.
// canSpawnNow and luaAllowsSpawn are host-evaluated.
inline auto ShouldKeepPendingWhenCannotSpawn(const bool canSpawnNow, const bool luaAllowsSpawn) -> bool
{
    return !canSpawnNow || !luaAllowsSpawn;
}

// EntityTargidFromId mirrors pair.first & 0x0FFF.
inline auto EntityTargidFromId(const uint32 entityID) -> uint16
{
    return static_cast<uint16>(entityID & 0x0FFF);
}

// ShouldRejectRegisterForRespawn mirrors !PMob || !AllowRespawn || in instance.
inline auto ShouldRejectRegisterForRespawn(const bool mobNull, const bool allowRespawn, const bool inInstance) -> bool
{
    return mobNull || !allowRespawn || inInstance;
}

// ShouldUseSpecificMobIdForSlot mirrors respawnTime.has_value() for slotted.
inline auto ShouldUseSpecificMobIdForSlot(const bool hasExplicitRespawnOverride) -> bool
{
    return hasExplicitRespawnOverride;
}

// ClampRemainingRespawn mirrors remaining > 0 ? remaining : 0.
inline auto ClampRemainingRespawnPositive(const bool remainingPositive) -> bool
{
    return remainingPositive;
}

// SpawnWindowSeconds is half of 30s interval = 15s.
constexpr int SpawnWindowSeconds = 15;

// SpawnHandlerIntervalSeconds is 30s.
constexpr int SpawnHandlerIntervalSeconds = 30;

} // namespace spawnhelpers
