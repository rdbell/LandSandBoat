#pragma once

#include "common/cbasetypes.h"

#include <string>

// Pure MapSessionContainer create/destroy/lookup-session gates.
// SQL, logging, ownership maps, and scheduler side effects remain host-owned.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2783: create policy (ShouldCreateSession, ShouldCreatePendingSession)
//   - 2925: ShouldCreateSession (queryOK && hasAccountsSessionRow)
//   - 2936: ShouldCreatePendingSession (queryOK identity; no rowsCount gate)
//
// Production host: MapSessionContainer::createSession injects queryOK /
// hasAccountsSessionRow after the accounts_sessions SELECT by client_addr.
// Go dual-wire: mapsession.ShouldCreateSession
// (internal/mapsession/create_session.go).
//
// Production host: MapSessionContainer::createPendingSession injects queryOK
// after the accounts_sessions SELECT by charid (static_cast<bool>(rset) only).
// Go dual-wire: mapsession.ShouldCreatePendingSession
// (internal/mapsession/pending_session.go).

namespace mapsessionhelpers
{

// ShouldCreateSession mirrors createSession after the accounts_sessions SELECT
// by client_addr. A failed query or empty result rejects creation (invalid
// login / SQL error). hasAccountsSessionRow is only meaningful when queryOK.
//
// Formula (slice 2925 dual-wire):
//   queryOK && hasAccountsSessionRow
//
// true  → host may allocate MapSession and proceed to index replace
// false → host returns nullptr (SQL error log or invalid-login debug)
inline auto ShouldCreateSession(const bool queryOK, const bool hasAccountsSessionRow) -> bool
{
    return queryOK && hasAccountsSessionRow;
}

// ShouldCreatePendingSession mirrors createPendingSession after the
// accounts_sessions SELECT by charid. Only a failed query rejects creation;
// an empty result set still creates a pending session (IPC may notify before
// the row is fully visible to a follow-up path, and LSB does not check
// rowsCount here).
//
// Formula (slice 2936 dual-wire):
//   queryOK
//
// true  → host may allocate MapSession (charID set) and proceed to index replace
// false → host returns nullptr (SQL error log only; no invalid-login empty path)
//
// Host inject (createPendingSession):
//   if (!ShouldCreatePendingSession(static_cast<bool>(rset))) return nullptr;
inline auto ShouldCreatePendingSession(const bool queryOK) -> bool
{
    return queryOK;
}

// ShouldReplaceExistingSession mirrors the same-key index replace step in
// createSession (by IPP) and createPendingSession (by charId) after the new
// MapSession is built. previousPresent is whether getSessionByIPP /
// getPendingSessionByCharId returned a non-null owner. Host owns
// removeSession / removePendingSession when this returns true; the ownership
// unique_ptr map overwrite remains host-owned either way.
inline auto ShouldReplaceExistingSession(const bool previousPresent) -> bool
{
    return previousPresent;
}

// ShouldDestroyPendingByPointer mirrors destroyPendingSession(MapSession*)
// after the null check and pending lookup by session->charID. found is whether
// a pending owner exists for that charID; pointerMatches is whether that owner
// is the same pointer. Host erase/delete only proceeds when both hold, so a
// stale or foreign pointer cannot drop a replacement pending session.
inline auto ShouldDestroyPendingByPointer(const bool found, const bool pointerMatches) -> bool
{
    return found && pointerMatches;
}

// ShouldDestroyPendingByCharID mirrors destroyPendingSession(uint32) after the
// pending lookup by charId. Host erase/delete only proceeds when an owner was
// found; missing keys are a no-op.
inline auto ShouldDestroyPendingByCharID(const bool found) -> bool
{
    return found;
}

// ShouldRejectNullCharLookup mirrors getSessionByChar's PChar == nullptr gate.
// Host returns nullptr before scanning confirmed sessions when this is true.
inline auto ShouldRejectNullCharLookup(const bool charNull) -> bool
{
    return charNull;
}

// SessionMatchesCharID mirrors one getSessionByChar loop comparison: the
// session must own a character and that character's id must equal the target.
// Host still owns map traversal and first-match return order.
inline auto SessionMatchesCharID(const bool sessionHasChar, const uint32 sessionCharID, const uint32 targetCharID) -> bool
{
    return sessionHasChar && sessionCharID == targetCharID;
}

// SessionMatchesCharName mirrors one getSessionByCharName loop comparison:
// session->PChar && session->PChar->name == name. Name equality is exact and
// case-sensitive. Host still owns map traversal and first-match return order.
inline auto SessionMatchesCharName(const bool sessionHasChar, const std::string& sessionName, const std::string& targetName) -> bool
{
    return sessionHasChar && sessionName == targetName;
}

// --- Slice 2799: cleanupSessions link-dead mark / recover pure plans ---

// ShouldMarkLinkDead mirrors the mark-link-dead gate inside the >5s inactive
// branch: PChar != nullptr && !PChar->isLinkDead. Caller has already ensured
// now > last_update + 5s. alreadyLinkDead is only meaningful when hasChar.
inline auto ShouldMarkLinkDead(const bool hasChar, const bool alreadyLinkDead) -> bool
{
    return hasChar && !alreadyLinkDead;
}

// ShouldRecoverLinkDead mirrors the recover-link-dead gate on the else-if of
// the >5s branch: PChar != nullptr && PChar->isLinkDead. Caller has already
// ensured the session is not past the five-second inactive gate.
inline auto ShouldRecoverLinkDead(const bool hasChar, const bool isLinkDead) -> bool
{
    return hasChar && isLinkDead;
}

// LinkDeadTransitionAction is one deferred effect of a link-dead mark or
// recover transition, in the order the host must apply them.
enum class LinkDeadTransitionAction : uint8
{
    SetDisconnectingFlag = 0,   // UPDATE char_flags SET disconnecting = 1
    ClearDisconnectingFlag,     // UPDATE char_flags SET disconnecting = 0
    SetLinkDead,                // PChar->isLinkDead = true
    ClearLinkDead,              // PChar->isLinkDead = false
    SetUpdateHPMask,            // PChar->updatemask |= UPDATE_HP
    SpawnPCsIfNormal,           // status == NORMAL → zone->SpawnPCs
    SaveCharStats,              // charutils::SaveCharStats (recover only)
};

// LinkDeadTransitionPlan is an ordered list of deferred mark/recover effects.
// Host owns SQL, character field writes, zone SpawnPCs, and SaveCharStats.
struct LinkDeadTransitionPlan
{
    static constexpr uint8 MaxActions = 5;

    LinkDeadTransitionAction actions[MaxActions]{};
    uint8                    count{ 0 };
};

// PlanLinkDeadMark returns cleanupSessions' mark-link-dead effects in exact
// production order. statusIsNormal gates the optional SpawnPCs step.
// 1) SetDisconnectingFlag  2) SetLinkDead  3) SetUpdateHPMask
// 4) SpawnPCsIfNormal (when statusIsNormal)
inline auto PlanLinkDeadMark(const bool statusIsNormal) -> LinkDeadTransitionPlan
{
    LinkDeadTransitionPlan plan{};
    plan.actions[plan.count++] = LinkDeadTransitionAction::SetDisconnectingFlag;
    plan.actions[plan.count++] = LinkDeadTransitionAction::SetLinkDead;
    plan.actions[plan.count++] = LinkDeadTransitionAction::SetUpdateHPMask;
    if (statusIsNormal)
    {
        plan.actions[plan.count++] = LinkDeadTransitionAction::SpawnPCsIfNormal;
    }
    return plan;
}

// PlanLinkDeadRecover returns cleanupSessions' recover-link-dead effects in
// exact production order. statusIsNormal gates the optional SpawnPCs step.
// 1) ClearDisconnectingFlag  2) ClearLinkDead  3) SetUpdateHPMask
// 4) SpawnPCsIfNormal (when statusIsNormal)  5) SaveCharStats
inline auto PlanLinkDeadRecover(const bool statusIsNormal) -> LinkDeadTransitionPlan
{
    LinkDeadTransitionPlan plan{};
    plan.actions[plan.count++] = LinkDeadTransitionAction::ClearDisconnectingFlag;
    plan.actions[plan.count++] = LinkDeadTransitionAction::ClearLinkDead;
    plan.actions[plan.count++] = LinkDeadTransitionAction::SetUpdateHPMask;
    if (statusIsNormal)
    {
        plan.actions[plan.count++] = LinkDeadTransitionAction::SpawnPCsIfNormal;
    }
    plan.actions[plan.count++] = LinkDeadTransitionAction::SaveCharStats;
    return plan;
}

// PendingTimeoutCleanupAction is one deferred effect of the pending-session
// timeout erase path at the end of cleanupSessions.
enum class PendingTimeoutCleanupAction : uint8
{
    DeleteDatabaseSession = 0, // DELETE FROM accounts_sessions
    RemovePendingIndex,        // index_.removePendingSession
    ErasePending,              // erase from pending_sessions_
};

// PendingTimeoutCleanupPlan is always the three erase-path effects in order.
struct PendingTimeoutCleanupPlan
{
    static constexpr uint8 MaxActions = 3;

    PendingTimeoutCleanupAction actions[MaxActions]{
        PendingTimeoutCleanupAction::DeleteDatabaseSession,
        PendingTimeoutCleanupAction::RemovePendingIndex,
        PendingTimeoutCleanupAction::ErasePending,
    };
    uint8 count{ 3 };
};

// PlanPendingTimeoutCleanup returns the fixed pending timeout erase plan.
// Host owns SQL, index remove, and map erase (erase_if return true).
inline auto PlanPendingTimeoutCleanup() -> PendingTimeoutCleanupPlan
{
    return PendingTimeoutCleanupPlan{};
}

// --- Slice 2804: cleanupSessions confirmed-session timeout pure plan ---

// TimeoutDecision describes the state read by cleanupSessions after a session
// has crossed both the five-second outer gate and configured timeout. Host
// owns the accounts_sessions server_addr/port lookup that sets otherMap.
// HasCharacter / OtherMap / HasMobPet are bool; ShuttingDown is the session's
// shuttingDown byte (save position only when 0 or 1).
struct TimeoutDecision
{
    bool  hasCharacter{ false };
    bool  otherMap{ false };
    bool  hasMobPet{ false };
    uint8 shuttingDown{ 0 };
};

// CleanupAction is one deferred timeout/destroy effect in exact LSB order.
// Values match OmegaXI mapsession.CleanupAction (including DecreaseZoneCounter
// for PlanDestroy parity; timeout planning does not emit it).
enum class CleanupAction : uint8
{
    SaveStatusEffects = 0, // StatusEffectContainer->SaveStatusEffects(true)
    DeleteDatabaseSession, // DELETE FROM accounts_sessions
    SaveCharacterPosition, // charutils::SaveCharPosition
    DespawnMobPet,         // petutils::DespawnPet (TYPE_MOB pet)
    SetCharacterShutdown,  // PChar->status = SHUTDOWN
    RemoveCharacterFromZone,
    ReleaseCharacter,      // map_session_data->PChar.reset()
    RemoveSessionIndex,    // index_.removeSession
    EraseSession,          // sessions_.erase(it++)
    DecreaseZoneCounter,   // destroySession only (not timeout path)
};

// DecisionPlan is an ordered list of deferred cleanup effects.
// Host owns SQL, character/pet/zone mutation, index remove, and map erase.
struct DecisionPlan
{
    static constexpr uint8 MaxActions = 9;

    CleanupAction actions[MaxActions]{};
    uint8         count{ 0 };
};

// PlanTimeoutCleanup returns cleanupSessions' exact deferred effect order for
// a confirmed session that has timed out (mirrors OmegaXI PlanTimeoutCleanup):
// - !hasCharacter && otherMap:  RemoveSessionIndex, EraseSession
// - !hasCharacter && !otherMap: DeleteDatabaseSession, RemoveSessionIndex, EraseSession
// - hasCharacter && !otherMap:  SaveStatusEffects, DeleteDatabaseSession,
//                               [SaveCharacterPosition if shuttingDown == 0 || 1]
// - hasCharacter && hasMobPet:  DespawnMobPet
// - hasCharacter always:        SetCharacterShutdown, RemoveCharacterFromZone,
//                               ReleaseCharacter, RemoveSessionIndex, EraseSession
inline auto PlanTimeoutCleanup(const TimeoutDecision input) -> DecisionPlan
{
    DecisionPlan plan{};

    if (!input.hasCharacter)
    {
        if (!input.otherMap)
        {
            plan.actions[plan.count++] = CleanupAction::DeleteDatabaseSession;
        }
        plan.actions[plan.count++] = CleanupAction::RemoveSessionIndex;
        plan.actions[plan.count++] = CleanupAction::EraseSession;
        return plan;
    }

    if (!input.otherMap)
    {
        plan.actions[plan.count++] = CleanupAction::SaveStatusEffects;
        plan.actions[plan.count++] = CleanupAction::DeleteDatabaseSession;
        if (input.shuttingDown == 0 || input.shuttingDown == 1)
        {
            plan.actions[plan.count++] = CleanupAction::SaveCharacterPosition;
        }
    }
    if (input.hasMobPet)
    {
        plan.actions[plan.count++] = CleanupAction::DespawnMobPet;
    }
    plan.actions[plan.count++] = CleanupAction::SetCharacterShutdown;
    plan.actions[plan.count++] = CleanupAction::RemoveCharacterFromZone;
    plan.actions[plan.count++] = CleanupAction::ReleaseCharacter;
    plan.actions[plan.count++] = CleanupAction::RemoveSessionIndex;
    plan.actions[plan.count++] = CleanupAction::EraseSession;
    return plan;
}

// DestroyDecision describes state used by destroySession after its identity-
// safe index removal has succeeded (mirrors OmegaXI DestroyDecision).
// HasCharacter / HasZone are bool; ShuttingDown is the session's shuttingDown
// byte (database row deleted only when exactly 1 — explicit logout).
struct DestroyDecision
{
    bool  hasCharacter{ false };
    bool  hasZone{ false };
    uint8 shuttingDown{ 0 };
};

// PlanDestroy returns destroySession's remaining effects in exact order after
// index_.removeSession has already succeeded (mirrors OmegaXI PlanDestroy).
// RemoveSessionIndex is intentionally not planned: production removes the
// index entry before planning so a stale/foreign pointer cannot erase a
// replacement session.
// Order:
// - shuttingDown == 1: DeleteDatabaseSession
// - hasCharacter && hasZone: DecreaseZoneCounter
// - hasCharacter: ReleaseCharacter
// - always: EraseSession
inline auto PlanDestroy(const DestroyDecision input) -> DecisionPlan
{
    DecisionPlan plan{};

    if (input.shuttingDown == 1)
    {
        plan.actions[plan.count++] = CleanupAction::DeleteDatabaseSession;
    }
    if (input.hasCharacter)
    {
        if (input.hasZone)
        {
            plan.actions[plan.count++] = CleanupAction::DecreaseZoneCounter;
        }
        plan.actions[plan.count++] = CleanupAction::ReleaseCharacter;
    }
    plan.actions[plan.count++] = CleanupAction::EraseSession;
    return plan;
}

} // namespace mapsessionhelpers
