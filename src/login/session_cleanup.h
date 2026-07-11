#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure authenticated-session cleanup decisions for view_session::handle_error
// and data_session::handle_error, plus remaining VIEW create/name diagnostics.

namespace loginHelpers
{

// Which peer pointer the error path nulls before considering map erasure.
enum class session_error_peer : uint8_t
{
    VIEW, // view_session::handle_error clears session.view_session
    DATA, // data_session::handle_error clears session.data_session
};

// session_error_cleanup_plan is the pure outcome of handle_error after the host
// has located (or failed to locate) the authenticated session entry.
struct session_error_cleanup_plan
{
    bool clearPeer{};         // null the erroring view or data shared_ptr
    bool eraseSessionEntry{}; // erase map[ip][hash] when both peers are null after clear
    // eraseIPEntry is decided after erase using remaining IP map size (host inject).
};

// HasSessionHashForCleanup mirrors sessionHash != "" before map lookup.
inline auto HasSessionHashForCleanup(const std::string& sessionHash) -> bool
{
    return !sessionHash.empty();
}

// PlanSessionErrorCleanup mirrors the post-lookup body of view/data handle_error.
// entryFound is false when map.find(hash) fails.
// peerPresent is whether the non-erroring peer is non-null *before* clear
// (view path: has data_session; data path: has view_session).
// The erroring peer is always cleared when entryFound (even if already null).
inline auto PlanSessionErrorCleanup(
    const bool              entryFound,
    const session_error_peer /*erroringPeer*/,
    const bool              otherPeerPresent) -> session_error_cleanup_plan
{
    if (!entryFound)
    {
        return session_error_cleanup_plan{ .clearPeer = false, .eraseSessionEntry = false };
    }
    // After clearing the erroring peer, erase when the other peer is already null.
    return session_error_cleanup_plan{
        .clearPeer         = true,
        .eraseSessionEntry = !otherPeerPresent,
    };
}

// ShouldEraseIPAfterSessionErase mirrors outerIt->second.empty() after erase.
inline auto ShouldEraseIPAfterSessionErase(const bool ipMapEmptyAfterErase) -> bool
{
    return ipMapEmptyAfterErase;
}

// FormatCharacterCreatedInfo mirrors VIEW 0x21 success ShowInfo text.
inline auto FormatCharacterCreatedInfo(const std::string& charName, const uint32 accountID) -> std::string
{
    return fmt::format("char <{}> was successfully created on account {}", charName, accountID);
}

// FormatNewCharacterNameError mirrors VIEW 0x22 invalid-name ShowWarning text.
inline auto FormatNewCharacterNameError(const std::string& charName, const std::string& reason) -> std::string
{
    return fmt::format("new character name error <{}>: {}", charName, reason);
}

} // namespace loginHelpers
