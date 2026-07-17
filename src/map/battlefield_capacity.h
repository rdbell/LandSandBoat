#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <string>

// Pure CBattlefield policy helpers for level cap, insert, cleanup, and tick.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1361: level cap, insert, cleanup, tick policy suite
//   - 2930: ShouldAcceptPCUnderCapacity (playerCount < maxParticipants)
//   - 2994: ShouldRejectNullInsert residual dual-wire expand
//   - 3002: ShouldRejectAlreadyInBattlefield residual dual-wire expand
//   - 3014: ShouldRegisterPC prior dual-wire
//           (!enter && !alreadyRegistered; pure 1361)
//   - 3024: ShouldEnterPC prior dual-wire
//           (enter identity; pure 1361)
//   - 3059: ShouldApplyLevelCap (levelCap > 0)
//   - 3087: ShouldAddSjRestriction ((rules & ALLOW_SUBJOBS) == 0)
//   - 3102: ShouldClearLevelRestriction (levelCap == 0)
//   - 3123: ShouldCheckInProgress (!attacked)
//   - 3140: ShouldAdvanceBattlefieldTick (pastTickPlusOneSecond identity)
//   - 3198: ShouldRejectNullInsert dedicated dual-wire
//           (entityNull identity; residual expand 2994 / pure 1361)
//   - 3216: ShouldRejectAlreadyInBattlefield dedicated dual-wire
//           (hasBattlefield identity; residual expand 3002 / pure 1361)
//   - 3271: ShouldAcceptPCUnderCapacity prior dedicated dual-wire
//           (playerCount < maxParticipants; residual expand 2930 / pure 1361)
//   - 3302: ShouldAcceptPCUnderCapacity dedicated dual-wire
//           (playerCount < maxParticipants; residual expand 2930 /
//            prior dedicated 3271 / pure 1361)
//   - 3365: ShouldRegisterPC dedicated dual-wire
//           (!enter && !alreadyRegistered; residual expand 3014 / pure 1361)
//   - 3381: ShouldEnterPC prior dedicated dual-wire
//           (enter identity; residual expand 3024 / pure 1361)
//   - 3431: ShouldEnterPC prior dedicated dual-wire
//           (enter identity; residual expand 3024 /
//            prior dedicated 3381 / pure 1361)
//   - 3497: ShouldEnterPC dedicated dual-wire
//           (enter identity; residual expand 3024 /
//            prior dedicated 3431 / 3381 / pure 1361)
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
//
// Production host: CBattlefield::ApplyLevelRestrictions else branch when
// !ShouldApplyLevelCap(GetLevelCap()) clears LevelRestriction — that clear
// path is dual-wired as ShouldClearLevelRestriction(levelCap == 0).
// Go dual-wire: battlefield.ShouldClearLevelRestriction
// (internal/battlefield/clear_level_restriction.go).
//
// Production host: CBattlefield::onTick injects m_Attacked into
// ShouldCheckInProgress; on true calls CheckInProgress().
// Go dual-wire: battlefield.ShouldCheckInProgress
// (internal/battlefield/check_in_progress.go).
//
// Production host: CBattlefield::onTick injects time > m_Tick + 1s into
// ShouldAdvanceBattlefieldTick; on true advances m_Tick / fight tick path.
// Go dual-wire: battlefield.ShouldAdvanceBattlefieldTick
// (internal/battlefield/advance_battlefield_tick.go).

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
// Formula (slice 3198 dedicated dual-wire; residual expand 2994 / pure 1361 —
// formula unchanged):
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
// Prior pure port: slice 1361. Residual dual-wire suite: 2994 /
// test_battlefield_reject_null_insert_2994. Dedicated dual-wire suite is
// test_battlefield_reject_null_insert_3198. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wires left alone: 3002 already-in, 2930 under-capacity,
// 3014 register, 3024 enter, 3140 advance-tick, etc.
inline auto ShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

// ShouldRejectAlreadyInBattlefield mirrors PEntity->PBattlefield != nullptr.
//
// Formula (slice 3216 dedicated dual-wire; residual expand 3002 / pure 1361 —
// formula unchanged):
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
// Prior pure port: slice 1361. Residual dual-wire suite: 3002 /
// test_battlefield_reject_already_in_3002. Dedicated dual-wire suite is
// test_battlefield_reject_already_in_3216. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wires left alone: 3198 null-insert, 2930 under-capacity,
// 3014 register, 3024 enter, 3140 advance-tick, etc.
inline auto ShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

// ShouldAcceptPCUnderCapacity mirrors GetPlayerCount() < GetMaxParticipants().
//
// Formula (slice 3302 dedicated dual-wire; residual expand 2930 /
// prior dedicated 3271 / pure 1361 — formula unchanged):
//   playerCount < maxParticipants
//
// playerCount     — host-evaluated GetPlayerCount() (entered PCs)
// maxParticipants — host-evaluated GetMaxParticipants() / m_MaxParticipants
// true  → accept PC insert (room under cap; enter or register path may proceed)
// false → reject PC insert (at or above capacity)
//
// Dual-wire of Go battlefield.ShouldAcceptPCUnderCapacity.
// Call site: CBattlefield::InsertEntity TYPE_PC branch host inject.
//   if (ShouldAcceptPCUnderCapacity(GetPlayerCount(), GetMaxParticipants())) {
//       // enter or register path
//   } else {
//       return false; // battlefield full
//   }
// Prior pure port: slice 1361. Residual dual-wire suite: 2930 /
// test_battlefield_under_capacity_2930. Prior dedicated dual-wire suite: 3271 /
// test_battlefield_under_capacity_3271. Dedicated dual-wire suite is
// test_battlefield_under_capacity_3302. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite.
// Sibling dual-wires left alone: 3198 null-insert, 3216 already-in,
// 3014 register, 3024 enter, 3140 advance-tick, etc.
// Strict less-than: equal counts reject (full). Empty field with max 0 rejects.
inline auto ShouldAcceptPCUnderCapacity(const uint8 playerCount, const uint8 maxParticipants) -> bool
{
    return playerCount < maxParticipants;
}

// ShouldRegisterPC mirrors !enter && !IsRegistered.
//
// Formula (slice 3365 dedicated dual-wire; residual expand 3014 / pure 1361 —
// formula unchanged):
//   !enter && !alreadyRegistered
//
// Positive form only — do not De Morgan rewrite to !(enter || alreadyRegistered)
// (QF1001).
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
// Prior pure port: slice 1361. Prior dual-wire suite: 3014 /
// test_battlefield_register_pc_3014. Dedicated dual-wire suite is
// test_battlefield_register_pc_3365. Formula is unchanged; this slice
// only expands dual-wire docs + index + dedicated suite
// (free == inline == pin residual pins).
// Sibling dual-wires left alone: 3198 null-insert, 3216 already-in,
// 3302 under-capacity, 3024 enter, 3140 advance-tick, etc.
inline auto ShouldRegisterPC(const bool enter, const bool alreadyRegistered) -> bool
{
    return !enter && !alreadyRegistered;
}

// ShouldEnterPC mirrors enter path under capacity.
//
// Formula (slice 3497 dedicated dual-wire; residual expand 3024 /
// prior dedicated 3431 / 3381 / pure 1361 — formula unchanged):
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
//       // register path (3014 / 3365)
//   }
// Prior pure port: slice 1361. Prior dual-wire suite: 3024 /
// test_battlefield_enter_pc_3024. Prior dedicated dual-wire suites: 3381 /
// test_battlefield_enter_pc_3381, 3431 / test_battlefield_enter_pc_3431.
// Dedicated dual-wire suite is test_battlefield_enter_pc_3497. Formula is
// unchanged; this slice only expands dual-wire docs + index + dedicated suite
// (free == inline == pin residual pins).
// Sibling dual-wires left alone: 3198 null-insert, 3216 already-in,
// 3302 under-capacity, 3365 register, 3140 advance-tick, etc.
// Do not thrash register_pc.
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
//
// Formula (slice 3102 dual-wire):
//   levelCap == 0
//
// levelCap — host-evaluated GetLevelCap() / raw battlefield level cap
// true  → clear / DelStatusEffect(LevelRestriction) (uncapped battlefield)
// false → do not take the clear-only path (nonzero cap applies restriction)
//
// Dual-wire of Go battlefield.ShouldClearLevelRestriction.
// Call site semantics: CBattlefield::ApplyLevelRestrictions else branch
// when !ShouldApplyLevelCap(rawCap) — DelStatusEffect(LevelRestriction).
// Logically inverse of ShouldApplyLevelCap (3059; left alone): clear iff
// !apply for all uint8. Sibling dual-wire ShouldAddSjRestriction remains
// in this header (3087; left alone). Sibling residual ResolveLevelCap
// remains in this header (1361).
// Prior pure port: slice 1361 (battlefield policy suite). Residual pins
// remain in test_battlefield_policy_1361; dedicated dual-wire suite is
// test_battlefield_clear_level_3102.
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
//
// Formula (slice 3123 dual-wire):
//   !attacked
//
// attacked — host-injected m_Attacked (battlefield has been engaged)
// true  → host calls CheckInProgress() (not yet attacked)
// false → skip CheckInProgress (already attacked / progress checked)
//
// Dual-wire of Go battlefield.ShouldCheckInProgress.
// Call site: CBattlefield::onTick — host injects m_Attacked:
//   if (ShouldCheckInProgress(m_Attacked)) {
//       CheckInProgress();
//   }
// Prior pure port: slice 1361 (battlefield policy suite). Residual pins remain
// in test_battlefield_policy_1361; dedicated dual-wire suite is
// test_battlefield_check_in_progress_3123. Sibling dual-wire
// ShouldAdvanceBattlefieldTick remains in this header (3140). Sibling residual
// tick halves (ShouldHoldFightTick / ShouldCaptureFinishTime) remain in this
// header (1361). Sibling dual-wires 3059 / 3087 / 3102 (ApplyLevelRestrictions)
// left alone.
inline auto ShouldCheckInProgress(const bool attacked) -> bool
{
    return !attacked;
}

// ShouldAdvanceBattlefieldTick mirrors time > m_Tick + 1s.
//
// Formula (slice 3140 dual-wire):
//   pastTickPlusOneSecond
//
// pastTickPlusOneSecond — host-injected time > m_Tick + 1s
// true  → host advances m_Tick / fight tick / finish capture / OnBattlefieldTick
// false → skip 1s tick advance this onTick
//
// Dual-wire of Go battlefield.ShouldAdvanceBattlefieldTick.
// Call site: CBattlefield::onTick — host injects time > m_Tick + 1s:
//   if (ShouldAdvanceBattlefieldTick(time > m_Tick + 1s)) {
//       m_Tick = time;
//       // fight tick / finish / OnBattlefieldTick ...
//   }
// Prior pure port: slice 1361 (battlefield policy suite). Residual pins remain
// in test_battlefield_policy_1361; dedicated dual-wire suite is
// test_battlefield_advance_tick_3140. Sibling residual tick halves
// (ShouldHoldFightTick / ShouldCaptureFinishTime) remain in this header
// (1361; ShouldHoldFightTick not dual-wired in this slice). Sibling dual-wire
// ShouldCheckInProgress remains in this header (3123; left alone). Sibling
// dual-wires 3059 / 3087 / 3102 (ApplyLevelRestrictions) left alone.
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
