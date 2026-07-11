#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <string>

// Pure CBattlefield policy helpers for level cap, insert, cleanup, and tick.

namespace battlefieldhelpers
{

// RULES_ALLOW_SUBJOBS bit (0x01).
constexpr uint16 RulesAllowSubjobs = 0x01;

// CONDITION_DISAPPEAR_AT_START bit (0x04).
constexpr uint8 ConditionDisappearAtStart = 0x04;

// BATTLEFIELD_STATUS_WON / LOST numeric values from battlefield.h.
constexpr uint8 StatusOpen   = 0;
constexpr uint8 StatusLocked = 1;
constexpr uint8 StatusWon    = 2;
constexpr uint8 StatusLost   = 3;

// Leave codes from battlefield.h.
constexpr uint8 LeaveCodeExit   = 1;
constexpr uint8 LeaveCodeWin    = 2;
constexpr uint8 LeaveCodeWarpDC = 3;
constexpr uint8 LeaveCodeLose   = 4;

// FormatInsertEntityNullWarning mirrors InsertEntity null gate warning.
inline auto FormatInsertEntityNullWarning() -> std::string
{
    return "CBattlefield::InsertEntity() - PEntity is null.";
}

// ShouldRejectNullInsert mirrors PEntity == nullptr.
inline auto ShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

// ShouldRejectAlreadyInBattlefield mirrors PEntity->PBattlefield != nullptr.
inline auto ShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

// ShouldAcceptPCUnderCapacity mirrors GetPlayerCount() < GetMaxParticipants().
inline auto ShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

// ShouldRegisterPC mirrors !enter && !IsRegistered.
inline auto ShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

// ShouldEnterPC mirrors enter path under capacity.
inline auto ShouldEnterPC(const bool enter) -> bool
{
    return enter;
}

// NPC status: DISAPPEAR when CONDITION_DISAPPEAR_AT_START else NORMAL.
// Returns true for disappear.
inline auto ShouldNPCDisappearAtStart(const uint8 conditions) -> bool
{
    return (conditions & ConditionDisappearAtStart) == ConditionDisappearAtStart;
}

// IsMobOrPetEntity for enemy insert branch.
inline auto IsMobOrPetEntity(const bool isMob, const bool isPet) -> bool
{
    return isMob || isPet;
}

// --- ApplyLevelRestrictions pure cap resolution ---

// ShouldApplyLevelCap mirrors cap > 0.
inline auto ShouldApplyLevelCap(const uint8 levelCap) -> bool
{
    return levelCap > 0;
}

// ResolveLevelCap after BATTLE_CAP_TWEAK and mission uncapped override.
// battleCapTweak is map.BATTLE_CAP_TWEAK; lvCapMissionBCNM is map.LV_CAP_MISSION_BCNM;
// serverMaxLevel is main.MAX_LEVEL; isMission is m_isMission == 1.
inline auto ResolveLevelCap(
    const uint8 levelCap,
    const int8  battleCapTweak,
    const bool  lvCapMissionBCNM,
    const bool  isMission,
    const uint8 serverMaxLevel) -> uint8
{
    if (levelCap == 0)
    {
        return 0;
    }
    int16 adjusted = static_cast<int16>(levelCap) + static_cast<int16>(battleCapTweak);
    if (adjusted < 0)
    {
        adjusted = 0;
    }
    if (adjusted > 255)
    {
        adjusted = 255;
    }
    uint8 cap = static_cast<uint8>(adjusted);
    // Mission BCNM uncapped when LV_CAP_MISSION_BCNM is false.
    if (!lvCapMissionBCNM && isMission)
    {
        cap = serverMaxLevel;
    }
    return cap;
}

// ShouldAddSjRestriction mirrors !(m_Rules & RULES_ALLOW_SUBJOBS).
inline auto ShouldAddSjRestriction(const uint16 rules) -> bool
{
    return (rules & RulesAllowSubjobs) == 0;
}

// ShouldClearLevelRestriction mirrors cap == 0 path (delete LevelRestriction).
inline auto ShouldClearLevelRestriction(const uint8 levelCap) -> bool
{
    return levelCap == 0;
}

// --- CanCleanup / Cleanup ---

// CanCleanupResult mirrors m_Cleanup || entered empty (after sticky set).
inline auto CanCleanupResult(const bool cleanupSticky, const bool enteredEmpty) -> bool
{
    return cleanupSticky || enteredEmpty;
}

// ShouldSetCleanupSticky mirrors if (cleanup) m_Cleanup = cleanup.
inline auto ShouldSetCleanupSticky(const bool cleanupRequest) -> bool
{
    return cleanupRequest;
}

// ShouldDeferCleanup mirrors !force && !empty && cleanupTime > time.
inline auto ShouldDeferCleanup(const bool force, const bool enteredEmpty, const bool cleanupTimeAfterNow) -> bool
{
    return !force && !enteredEmpty && cleanupTimeAfterNow;
}

// LeaveCodeFromStatus mirrors WON → WIN else LOSE.
inline auto LeaveCodeFromStatus(const uint8 status) -> uint8
{
    return status == StatusWon ? LeaveCodeWin : LeaveCodeLose;
}

// ShouldSchedulePlayerCleanupDelay mirrors !force after first player cleanup pass.
inline auto ShouldSchedulePlayerCleanupDelay(const bool force) -> bool
{
    return !force;
}

// ShouldUpdateRecordOnWin mirrors status WON && record.time > finishTime.
inline auto ShouldUpdateRecordOnWin(const uint8 status, const bool recordTimeWorseThanFinish) -> bool
{
    return status == StatusWon && recordTimeWorseThanFinish;
}

// --- onTick pure halves ---

// ShouldCheckInProgress mirrors !m_Attacked at tick start.
inline auto ShouldCheckInProgress(const bool attacked) -> bool
{
    return !attacked;
}

// ShouldAdvanceBattlefieldTick mirrors time > m_Tick + 1s.
inline auto ShouldAdvanceBattlefieldTick(const bool pastTickPlusOneSecond) -> bool
{
    return pastTickPlusOneSecond;
}

// ResolveFightTick mirrors locked status keeps fight tick else uses now.
inline auto ShouldHoldFightTick(const uint8 status) -> bool
{
    return status == StatusLocked;
}

// ShouldCaptureFinishTime mirrors status >= WON.
inline auto ShouldCaptureFinishTime(const uint8 status) -> bool
{
    return status >= StatusWon;
}

// IsOccupied mirrors !entered.empty().
inline auto IsOccupied(const bool enteredEmpty) -> bool
{
    return !enteredEmpty;
}

// IsEntered mirrors set contains id.
inline auto IsEntered(const bool foundInEntered) -> bool
{
    return foundInEntered;
}

// IsRegistered mirrors set contains id.
inline auto IsRegistered(const bool foundInRegistered) -> bool
{
    return foundInRegistered;
}

// ShouldInsertPetWithPC mirrors enter && PPet != nullptr.
inline auto ShouldInsertPetWithPC(const bool enter, const bool hasPet) -> bool
{
    return enter && hasPet;
}

// ShouldSendTimerPacket mirrors m_showTimer on enter.
inline auto ShouldSendTimerPacket(const bool showTimer) -> bool
{
    return showTimer;
}

// ShouldClearNoDespawnBehavior mirrors BEHAVIOR_NO_DESPAWN set on cleanup.
inline auto ShouldClearNoDespawnBehavior(const bool hasNoDespawnBit) -> bool
{
    return hasNoDespawnBit;
}

// ShouldDespawnMobOnCleanup mirrors isAlive && IsSpawned.
inline auto ShouldDespawnMobOnCleanup(const bool isAlive, const bool isSpawned) -> bool
{
    return isAlive && isSpawned;
}

} // namespace battlefieldhelpers
