#pragma once

#include "common/cbasetypes.h"

#include <string>

// Pure MapSessionContainer create/destroy/lookup-session gates.
// SQL, logging, ownership maps, and scheduler side effects remain host-owned.

namespace mapsessionhelpers
{

// ShouldCreateSession mirrors createSession after the accounts_sessions SELECT
// by client_addr. A failed query or empty result rejects creation (invalid
// login / SQL error). hasAccountsSessionRow is only meaningful when queryOK.
inline auto ShouldCreateSession(const bool queryOK, const bool hasAccountsSessionRow) -> bool
{
    return queryOK && hasAccountsSessionRow;
}

// ShouldCreatePendingSession mirrors createPendingSession after the
// accounts_sessions SELECT by charid. Only a failed query rejects creation;
// an empty result set still creates a pending session (IPC may notify before
// the row is fully visible to a follow-up path, and LSB does not check
// rowsCount here).
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

} // namespace mapsessionhelpers
