#pragma once

// Pure MapSessionContainer create/destroy-session gates.
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

} // namespace mapsessionhelpers
