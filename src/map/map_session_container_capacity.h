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
//   - 2787: destroy-pending pure gates (ShouldDestroyPendingByPointer residual)
//   - 3056: ShouldDestroyPendingByPointer (found && pointerMatches)
//   - 3066: ShouldDestroyPendingByCharID (found identity)
//   - 2790: lookup pure gates (ShouldRejectNullCharLookup, SessionMatches*)
//   - 2954: ShouldRejectNullCharLookup (charNull identity)
//   - 2799: link-dead mark/recover pure plans (ShouldMarkLinkDead residual)
//   - 2978: ShouldMarkLinkDead (hasChar && !alreadyLinkDead)
//   - 2985: ShouldRecoverLinkDead (hasChar && isLinkDead)
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
//
// Production host: MapSessionContainer::destroyPendingSession(MapSession*)
// injects found / pointerMatches after null check and pending lookup by
// session->charID before erase/delete. Go dual-wire:
// mapsession.ShouldDestroyPendingByPointer
// (internal/mapsession/destroy_pending_pointer.go). Prior pure port: 2787;
// sibling dual-wire: 3066 ShouldDestroyPendingByCharID.
//
// Production host: MapSessionContainer::destroyPendingSession(uint32)
// injects found after pending lookup by charId before erase/delete.
// Go dual-wire: mapsession.ShouldDestroyPendingByCharID
// (internal/mapsession/destroy_pending_charid.go). Prior pure port: 2787;
// sibling dual-wire: 3056 ShouldDestroyPendingByPointer.
//
// Production host: MapSessionContainer::getSessionByChar injects
// charNull = (PChar == nullptr) before scanning confirmed sessions.
// Go dual-wire: mapsession.ShouldRejectNullCharLookup
// (internal/mapsession/null_char.go).
//
// Production host: MapSessionContainer::cleanupSessions injects hasChar /
// alreadyLinkDead into ShouldMarkLinkDead inside the >5s inactive branch
// before the mark body. Go dual-wire: mapsession.ShouldMarkLinkDead
// (internal/mapsession/mark_link_dead.go). Prior pure port: slice 2799.
//
// Production host: MapSessionContainer::cleanupSessions injects hasChar /
// isLinkDead into ShouldRecoverLinkDead on the else-if of the >5s inactive
// branch before the recover body. Go dual-wire: mapsession.ShouldRecoverLinkDead
// (internal/mapsession/recover_link_dead.go). Prior pure port: slice 2799;
// sibling dual-wire: 2978 ShouldMarkLinkDead.

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
//
// Formula (slice 3086 dual-wire):
//   previousPresent
//
// Host-injected scalar (no session / index / ownership-map pointers):
//   previousPresent — same-key index lookup returned a non-null owner
//
// true  → host may removeSession / removePendingSession for that previous
// false → skip index remove (no same-key owner)
//
// Dual-wire of Go mapsession.ShouldReplaceExistingSession
// (internal/mapsession/replace_existing_session.go). Prior pure port: slice 2795.
// Sibling dual-wires 3056/3066 (destroy-pending gates) are left alone.
// Call sites: MapSessionContainer::createSession / createPendingSession
// (map_session_container.cpp) already inject previous != nullptr before remove.
inline auto ShouldReplaceExistingSession(const bool previousPresent) -> bool
{
    return previousPresent;
}

// ShouldDestroyPendingByPointer mirrors destroyPendingSession(MapSession*)
// after the null check and pending lookup by session->charID. found is whether
// a pending owner exists for that charID; pointerMatches is whether that owner
// is the same pointer. Host erase/delete only proceeds when both hold, so a
// stale or foreign pointer cannot drop a replacement pending session.
//
// Formula (slice 3056 dual-wire):
//   found && pointerMatches
//
// Host-injected scalars (no session / pending-map pointers):
//   found           — pending owner exists for session->charID
//   pointerMatches  — that owner is the same pointer as the argument
//
// true  → host may erase pending index entry and delete the MapSession
// false → skip erase/delete (missing key or stale/foreign pointer)
//
// Caller has already rejected a null session pointer (outside pure).
// Dual-wire of Go mapsession.ShouldDestroyPendingByPointer
// (internal/mapsession/destroy_pending_pointer.go). Prior pure port: slice 2787.
// Sibling dual-wire: ShouldDestroyPendingByCharID (found identity; slice 3066).
// Call site: MapSessionContainer::destroyPendingSession(MapSession*)
// (map_session_container.cpp) already injects the two bools before erase.
inline auto ShouldDestroyPendingByPointer(const bool found, const bool pointerMatches) -> bool
{
    return found && pointerMatches;
}

// ShouldDestroyPendingByCharID mirrors destroyPendingSession(uint32) after the
// pending lookup by charId. Host erase/delete only proceeds when an owner was
// found; missing keys are a no-op.
//
// Formula (slice 3066 dual-wire):
//   found
//
// Host-injected scalar (no session / pending-map pointers):
//   found — pending owner exists for charId
//
// true  → host may erase pending index entry and delete the MapSession
// false → skip erase/delete (missing key is a no-op)
//
// Dual-wire of Go mapsession.ShouldDestroyPendingByCharID
// (internal/mapsession/destroy_pending_charid.go). Prior pure port: slice 2787.
// Sibling dual-wire: ShouldDestroyPendingByPointer (found && pointerMatches;
// slice 3056; pointer overload needs identity).
// Call site: MapSessionContainer::destroyPendingSession(uint32)
// (map_session_container.cpp) already injects found before erase.
inline auto ShouldDestroyPendingByCharID(const bool found) -> bool
{
    return found;
}

// ShouldRejectNullCharLookup mirrors getSessionByChar's PChar == nullptr gate.
// Host returns nullptr before scanning confirmed sessions when this is true.
//
// Formula (slice 2954 dual-wire):
//   charNull
//
// true  → host returns nullptr before confirmed-map scan
// false → host proceeds to SessionMatchesCharID loop match
//
// Host inject (getSessionByChar):
//   if (ShouldRejectNullCharLookup(PChar == nullptr)) return nullptr;
//
// SessionMatchesCharID / SessionMatchesCharName remain 2790 residual siblings
// (scan-level match helpers; not dual-wired in slice 2954).
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
// --- Slice 2978: ShouldMarkLinkDead pure dual-wire expansion ---
// --- Slice 2985: ShouldRecoverLinkDead pure dual-wire expansion ---

// ShouldMarkLinkDead mirrors the mark-link-dead gate inside the >5s inactive
// branch of MapSessionContainer::cleanupSessions:
//   PChar != nullptr && !PChar->isLinkDead
//
// Formula (slice 2978 dual-wire):
//   hasChar && !alreadyLinkDead
//
// Host-injected scalars (no session / character pointers):
//   hasChar          — PChar != nullptr
//   alreadyLinkDead  — hasChar && PChar->isLinkDead (only meaningful when hasChar)
// true  → host may run PlanLinkDeadMark body (SQL disconnecting flag, set
//         isLinkDead, UPDATE_HP mask, optional SpawnPCs)
// false → skip mark body (no character or already link-dead)
//
// Caller has already ensured now > last_update + 5s (timeout gate outside pure).
// Dual-wire of Go mapsession.ShouldMarkLinkDead
// (internal/mapsession/mark_link_dead.go). Prior pure port: slice 2799.
// Sibling dual-wire: ShouldRecoverLinkDead (slice 2985; inverted polarity).
// Call site: MapSessionContainer::cleanupSessions (map_session_container.cpp).
inline auto ShouldMarkLinkDead(const bool hasChar, const bool alreadyLinkDead) -> bool
{
    return hasChar && !alreadyLinkDead;
}

// ShouldRecoverLinkDead mirrors the recover-link-dead gate on the else-if of
// the >5s branch of MapSessionContainer::cleanupSessions:
//   PChar != nullptr && PChar->isLinkDead
//
// Formula (slice 2985 dual-wire):
//   hasChar && isLinkDead
//
// Host-injected scalars (no session / character pointers):
//   hasChar    — PChar != nullptr
//   isLinkDead — hasChar && PChar->isLinkDead (only meaningful when hasChar)
// true  → host may run PlanLinkDeadRecover body (SQL clear disconnecting flag,
//         clear isLinkDead, UPDATE_HP mask, optional SpawnPCs, SaveCharStats)
// false → skip recover body (no character or not link-dead)
//
// Caller has already ensured the session is not past the five-second inactive
// gate (else of now > last_update + 5s; timeout gate outside pure).
// Dual-wire of Go mapsession.ShouldRecoverLinkDead
// (internal/mapsession/recover_link_dead.go). Prior pure port: slice 2799.
// Sibling dual-wire: ShouldMarkLinkDead (slice 2978; inverted polarity —
// mark uses !alreadyLinkDead; recover uses isLinkDead).
// Call site: MapSessionContainer::cleanupSessions (map_session_container.cpp)
// already injects the two bools before the recover body.
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
