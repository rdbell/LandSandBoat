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
//   - 3092: ShouldRejectAtNightSpawn (ATNIGHT flag && not NIGHT/MIDNIGHT TOTD)
//   - 3107: ShouldRejectAtEveningSpawn (ATEVENING flag && not evening TOTD window)
//   - 3124: ShouldRejectSpawnNullOrDisabled (mobNull || !allowRespawn)
//   - 3139: ShouldDespawnOnNewDay (SPAWNTYPE_ATNIGHT flag on NEWDAY TOTD)
//
// Production host: SpawnHandler::canSpawnNow (spawn_handler.cpp) injects
// isFog = zone_->weather().current() == Weather::Fog and current TOTD before
// CanSpawnNowPure. SpawnHandler::onTOTDChange NEWDAY injects m_SpawnType into
// ShouldDespawnOnNewDay.
// Go dual-wire: spawnslot.ShouldRejectFogSpawn (internal/spawnslot/reject_fog.go),
// spawnslot.ShouldRejectAtNightSpawn (internal/spawnslot/reject_night_spawn.go),
// spawnslot.ShouldRejectAtEveningSpawn (internal/spawnslot/reject_evening_spawn.go),
// spawnslot.ShouldRejectSpawnNullOrDisabled
// (internal/spawnslot/reject_spawn_null_disabled.go),
// spawnslot.ShouldDespawnOnNewDay (internal/spawnslot/despawn_on_new_day.go).

namespace spawnhelpers
{

// SPAWNTYPE bit mirrors (from mob_entity.h).
constexpr uint8 SpawnTypeNormal    = SPAWNTYPE_NORMAL;
constexpr uint8 SpawnTypeAtNight   = SPAWNTYPE_ATNIGHT;
constexpr uint8 SpawnTypeAtEvening = SPAWNTYPE_ATEVENING;
constexpr uint8 SpawnTypeWeather   = SPAWNTYPE_WEATHER;
constexpr uint8 SpawnTypeFog       = SPAWNTYPE_FOG;

// ShouldRejectSpawnNullOrDisabled mirrors !PMob || !m_AllowRespawn.
// mobNull / allowRespawn are host-evaluated PMob nullity and m_AllowRespawn.
//
// Formula (slice 3124 dual-wire):
//   mobNull || !allowRespawn
//   // Positive form avoids QF1001 De Morgan rewrite of !(!mobNull && allowRespawn)
//
// true  → reject spawn (missing mob, or respawn disabled)
// false → null/disabled gate passes (mob present and allowRespawn)
//
// Dual-wire of Go spawnslot.ShouldRejectSpawnNullOrDisabled.
// Call site: CanSpawnNowPure (and SpawnHandler::canSpawnNow host inject).
// Sibling dual-wires (leave alone): ShouldRejectAtNightSpawn (3092),
// ShouldRejectAtEveningSpawn (3107). Related residual
// ShouldRejectRegisterForRespawn adds an inInstance clause (not this gate).
inline auto ShouldRejectSpawnNullOrDisabled(const bool mobNull, const bool allowRespawn) -> bool
{
    // Positive form: mobNull || !allowRespawn (avoid QF1001 De Morgan).
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
// totd is host-evaluated vanadiel_time::TOTD.
//
// Formula (slice 3092 dual-wire):
//   HasSpawnTypeFlag(spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(totd)
//   // SpawnTypeAtNight = 0x01 (SPAWNTYPE_ATNIGHT)
//   // IsNightTotdWindow: totd == NIGHT || totd == MIDNIGHT
//   // TOTD pins: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
//
// true  → reject spawn (ATNIGHT-type mob outside NIGHT/MIDNIGHT window)
// false → night gate passes (no ATNIGHT flag, or totd is night window)
//
// Dual-wire of Go spawnslot.ShouldRejectAtNightSpawn.
// Call site: CanSpawnNowPure (and SpawnHandler::canSpawnNow host inject).
// Sibling dual-wire: ShouldRejectAtEveningSpawn (ATEVENING + evening window; 3107).
inline auto ShouldRejectAtNightSpawn(const uint8 spawnType, const vanadiel_time::TOTD totd) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtNight) && !IsNightTotdWindow(totd);
}

// ShouldRejectAtEveningSpawn mirrors ATEVENING flag && not evening window.
// totd is host-evaluated vanadiel_time::TOTD.
//
// Formula (slice 3107 dual-wire):
//   HasSpawnTypeFlag(spawnType, SpawnTypeAtEvening) && !IsEveningTotdWindow(totd)
//   // SpawnTypeAtEvening = 0x02 (SPAWNTYPE_ATEVENING)
//   // IsEveningTotdWindow: totd == EVENING || NIGHT || MIDNIGHT || NEWDAY
//   // TOTD pins: NONE=0 MIDNIGHT=1 NEWDAY=2 DAWN=3 DAY=4 DUSK=5 EVENING=6 NIGHT=7
//
// true  → reject spawn (ATEVENING-type mob outside evening window)
// false → evening gate passes (no ATEVENING flag, or totd is evening window)
//
// Dual-wire of Go spawnslot.ShouldRejectAtEveningSpawn.
// Call site: CanSpawnNowPure (and SpawnHandler::canSpawnNow host inject).
// Sibling dual-wire: ShouldRejectAtNightSpawn (ATNIGHT + night window; 3092).
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
// spawnType is host-injected m_SpawnType bitfield.
//
// Formula (slice 3139 dual-wire):
//   HasSpawnTypeFlag(spawnType, SpawnTypeAtNight)
//   // SpawnTypeAtNight = 0x01 (SPAWNTYPE_ATNIGHT)
//
// true  → despawn ATNIGHT-type mob at NEWDAY TOTD
// false → keep (no ATNIGHT flag)
//
// Dual-wire of Go spawnslot.ShouldDespawnOnNewDay.
// Call site: SpawnHandler::onTOTDChange NEWDAY branch host inject.
// Sibling residual (leave alone): ShouldDespawnOnDawn (ATEVENING on DAWN).
// Sibling dual-wires (leave alone): ShouldRejectAtNightSpawn (3092),
// ShouldRejectAtEveningSpawn (3107), ShouldRejectSpawnNullOrDisabled (3124).
inline auto ShouldDespawnOnNewDay(const uint8 spawnType) -> bool
{
    return HasSpawnTypeFlag(spawnType, SpawnTypeAtNight);
}

// ShouldDespawnOnDawn mirrors SPAWNTYPE_ATEVENING on DAWN.
// Residual only (not dual-wired by slice 3139).
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
