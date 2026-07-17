#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <string>

// Pure CBattlefield policy helpers for level cap, insert, cleanup, and tick.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1361: level cap, insert, cleanup, tick policy suite
//   - 2930: ShouldAcceptPCUnderCapacity (playerCount < maxParticipants)
//   - 2994: ShouldRejectNullInsert (entityNull identity)
//   - 3002: ShouldRejectAlreadyInBattlefield (hasBattlefield identity)
//   - 3014: ShouldRegisterPC (!enter && !alreadyRegistered)
//   - 3024: ShouldEnterPC (enter identity)
//   - 3059: ShouldApplyLevelCap (levelCap > 0)
//   - 3087: ShouldAddSjRestriction ((rules & ALLOW_SUBJOBS) == 0)
//
// Production host: CBattlefield::InsertEntity (battlefield.cpp) injects
// GetPlayerCount() / GetMaxParticipants() into ShouldAcceptPCUnderCapacity
// for the TYPE_PC capacity gate.
// Go dual-wire: battlefield.ShouldAcceptPCUnderCapacity
// (internal/battlefield/under_capacity.go).
//
// Production host: CBattlefield::InsertEntity injects entityNull =
// (PEntity == nullptr) before already-in-battlefield / capacity / type
// branches.
// Go dual-wire: battlefield.ShouldRejectNullInsert
// (internal/battlefield/reject_null_insert.go).
//
// Production host: CBattlefield::InsertEntity injects hasBattlefield =
// (PEntity->PBattlefield != nullptr) after null gate, before capacity / type
// branches.
// Go dual-wire: battlefield.ShouldRejectAlreadyInBattlefield
// (internal/battlefield/reject_already_in.go).
//
// Production host: CBattlefield::InsertEntity TYPE_PC under-capacity branch
// injects enter into ShouldEnterPC as the first branch under capacity.
// Go dual-wire: battlefield.ShouldEnterPC
// (internal/battlefield/enter_pc.go).
//
// Production host: CBattlefield::InsertEntity TYPE_PC under-capacity branch
// injects enter and alreadyRegistered = IsRegistered(PChar) into
// ShouldRegisterPC after ShouldEnterPC(enter) is declined (else-if).
// Go dual-wire: battlefield.ShouldRegisterPC
// (internal/battlefield/register_pc.go).
//
// Production host: CBattlefield::ApplyLevelRestrictions injects
// GetLevelCap() into ShouldApplyLevelCap; on true ResolveLevelCap +
// LevelRestriction path, else DelStatusEffect(LevelRestriction).
// Go dual-wire: battlefield.ShouldApplyLevelCap
// (internal/battlefield/apply_level_cap.go).
//
// Production host: CBattlefield::ApplyLevelRestrictions injects m_Rules
// into ShouldAddSjRestriction after the level-cap path (whether or not
// there is a lv cap); on true AddStatusEffect(SjRestriction, ...).
// Go dual-wire: battlefield.ShouldAddSjRestriction
// (internal/battlefield/add_sj_restriction.go).

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
//
// Formula (slice 2994 dual-wire):
//   entityNull
//
// true  → host logs FormatInsertEntityNullWarning and returns false
// false → proceed to already-in-battlefield / capacity / type branches
//
// Dual-wire of Go battlefield.ShouldRejectNullInsert.
// Call site: CBattlefield::InsertEntity before other insert gates.
//   if (ShouldRejectNullInsert(PEntity == nullptr)) {
//       ShowWarning("%s", FormatInsertEntityNullWarning());
//       return false;
//   }
inline auto ShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

// ShouldRejectAlreadyInBattlefield mirrors PEntity->PBattlefield != nullptr.
//
// Formula (slice 3002 dual-wire):
//   hasBattlefield
//
// true  → host returns false (entity already on a battlefield)
// false → proceed to capacity / type branches
//
// Dual-wire of Go battlefield.ShouldRejectAlreadyInBattlefield.
// Call site: CBattlefield::InsertEntity after null gate, before capacity.
//   if (ShouldRejectAlreadyInBattlefield(PEntity->PBattlefield != nullptr)) {
//       return false;
//   }
inline auto ShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

// ShouldAcceptPCUnderCapacity mirrors GetPlayerCount() < GetMaxParticipants().
//
// Formula (slice 2930 dual-wire):
//   playerCount < maxParticipants
//
// playerCount     — host-evaluated GetPlayerCount() (entered PCs)
// maxParticipants — host-evaluated GetMaxParticipants() / m_MaxParticipants
// true  → accept PC insert (room under cap; enter or register path may proceed)
// false → reject PC insert (at or above capacity)
//
// Dual-wire of Go battlefield.ShouldAcceptPCUnderCapacity.
// Call site: CBattlefield::InsertEntity TYPE_PC branch host inject.
// Strict less-than: equal counts reject (full). Empty field with max 0 rejects.
inline auto ShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

// ShouldRegisterPC mirrors !enter && !IsRegistered.
//
// Formula (slice 3014 dual-wire):
//   !enter && !alreadyRegistered
//
// enter             — host InsertEntity enter flag
// alreadyRegistered — host-evaluated IsRegistered(PChar)
// true  → emplace into m_RegisteredPlayers, return true
// false → do not register (enter path owns enter=true; skip if already reg)
//
// Dual-wire of Go battlefield.ShouldRegisterPC.
// Call site: CBattlefield::InsertEntity TYPE_PC under capacity, else-if after
// ShouldEnterPC(enter):
//   else if (ShouldRegisterPC(enter, IsRegistered(PChar))) {
//       m_RegisteredPlayers.emplace(PEntity->id);
//       return true;
//   }
inline auto ShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

// ShouldEnterPC mirrors enter path under capacity.
//
// Formula (slice 3024 dual-wire):
//   enter
//
// enter — host InsertEntity enter flag
// true  → ApplyLevelRestrictions, emplace EnteredPlayers, OnBattlefieldEnter,
//         optional timer packet / pet insert
// false → decline enter path (register path may own via ShouldRegisterPC)
//
// Dual-wire of Go battlefield.ShouldEnterPC.
// Call site: CBattlefield::InsertEntity TYPE_PC under capacity, first branch:
//   if (ShouldEnterPC(enter)) {
//       ApplyLevelRestrictions(PChar);
//       m_EnteredPlayers.emplace(PEntity->id);
//       // ... OnBattlefieldEnter, timer, pet insert ...
//   }
//   else if (ShouldRegisterPC(enter, IsRegistered(PChar))) {
//       // register path (3014)
//   }
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

// ShouldApplyLevelCap mirrors levelCap > 0.
//
// Formula (slice 3059 dual-wire):
//   levelCap > 0
//
// levelCap — host-evaluated GetLevelCap() / raw battlefield level cap
// true  → apply ResolveLevelCap + LevelRestriction status / dispel path
// false → clear LevelRestriction (uncapped battlefield)
//
// Dual-wire of Go battlefield.ShouldApplyLevelCap.
// Call site: CBattlefield::ApplyLevelRestrictions — host injects GetLevelCap():
//   if (ShouldApplyLevelCap(rawCap)) {
//       const uint8 cap = ResolveLevelCap(...);
//       // DelStatusEffectsByFlag(Dispelable), DelStatusEffectSilent(Reraise),
//       // AddStatusEffect(LevelRestriction, ..., cap, ...)
//   } else {
//       DelStatusEffect(LevelRestriction);
//   }
// Prior pure port: slice 1361 (battlefield policy suite). Residual pins remain
// in test_battlefield_policy_1361; dedicated dual-wire suite is
// test_battlefield_apply_level_cap_3059. Sibling residual ResolveLevelCap
// remains in this header (1361; not dual-wired in slice 3059).
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
//
// Formula (slice 3087 dual-wire):
//   (rules & RulesAllowSubjobs) == 0
//
// rules — host-evaluated battlefield m_Rules / BCRULES bitset
// true  → add SjRestriction status (subjobs disallowed)
// false → leave subjobs enabled (ALLOW_SUBJOBS set)
//
// Dual-wire of Go battlefield.ShouldAddSjRestriction.
// Call site: CBattlefield::ApplyLevelRestrictions — host injects m_Rules
// after the level-cap path (SJ restriction is independent of lv cap):
//   if (ShouldAddSjRestriction(m_Rules)) {
//       AddStatusEffect(SjRestriction, ...);
//   }
// Prior pure port: slice 1361 (battlefield policy suite). Residual pins remain
// in test_battlefield_policy_1361; dedicated dual-wire suite is
// test_battlefield_add_sj_3087. Sibling dual-wire ShouldApplyLevelCap remains
// in this header (3059; left alone in this slice).
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
