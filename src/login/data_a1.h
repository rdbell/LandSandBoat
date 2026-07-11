#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure DATA 0xA1 character-list admission gates and diagnostics extracted so
// native tests can pin policy without sockets/SQL.

namespace loginHelpers
{

// Packet field offsets for DATA command 0xA1.
constexpr std::size_t DataA1AccountIDOffset = 1;
constexpr std::size_t DataA1ServerIPOffset  = 5;

// data_a1_account_match_gate is the pure outcome of comparing the session
// account ID to the account ID carried in the 0xA1 payload.
enum class data_a1_account_match_gate : uint8_t
{
    MATCH,    // enter character-list path
    MISMATCH, // production silently skips the entire 0xA1 body
};

// ClassifyDataA1AccountMatch mirrors session.accountID == receivedAccountID.
inline auto ClassifyDataA1AccountMatch(const uint32 sessionAccountID, const uint32 receivedAccountID) -> data_a1_account_match_gate
{
    return sessionAccountID == receivedAccountID ? data_a1_account_match_gate::MATCH : data_a1_account_match_gate::MISMATCH;
}

// data_a1_account_row_gate is the pure outcome of SELECT content_ids FROM accounts.
enum class data_a1_account_row_gate : uint8_t
{
    FOUND,   // continue with content_ids limit
    MISSING, // warn and close socket
};

// ClassifyDataA1AccountRow mirrors rset0 success with at least one row.
inline auto ClassifyDataA1AccountRow(const bool queryOk, const bool rowFound) -> data_a1_account_row_gate
{
    if (queryOk && rowFound)
    {
        return data_a1_account_row_gate::FOUND;
    }
    return data_a1_account_row_gate::MISSING;
}

// FormatClaimedAccountMissingWarning mirrors the ShowWarning when the claimed
// account ID has no accounts row.
inline auto FormatClaimedAccountMissingWarning(const uint32 accountID) -> std::string
{
    return fmt::format("Claimed accountID {} somehow doesn't have an account and should not have gotten this far.", accountID);
}

// data_a1_maint_empty_gate rejects logins that would present zero characters
// during maintenance (GM-filtered list empty).
enum class data_a1_maint_empty_gate : uint8_t
{
    ALLOW,
    REJECT, // maintMode > 0 && visibleCharacterCount == 0
};

// ClassifyDataA1MaintEmptyList mirrors maintMode > 0 && i == 0 after loading.
// visibleCharacterCount is the number of characters that passed the GM filter.
inline auto ClassifyDataA1MaintEmptyList(const uint8 maintMode, const uint32 visibleCharacterCount) -> data_a1_maint_empty_gate
{
    if (maintMode > 0 && visibleCharacterCount == 0)
    {
        return data_a1_maint_empty_gate::REJECT;
    }
    return data_a1_maint_empty_gate::ALLOW;
}

// FormatMaintModeLoginAttemptWarning mirrors the ShowWarning text. LSB embeds
// the misspelling "attmpted" and labels the path "(0xA2)" even though this runs
// on the 0xA1 character-list path — preserved for parity.
inline auto FormatMaintModeLoginAttemptWarning(const uint32 accountID) -> std::string
{
    return fmt::format("char:({}) attmpted login during maintenance mode (0xA2). Sending error to client.", accountID);
}

// MainJobLevelColumnIndex is the 0-based result-set column index used by
// create/list queries that place job level columns after a fixed 14-column
// prefix (indices 0..13), with WAR at 13+1 when mainJob is 1-based.
// LSB: rset1->get<uint8>(13 + MainJob).
inline auto MainJobLevelColumnIndex(const uint8 mainJob) -> int
{
    return 13 + static_cast<int>(mainJob);
}

// ShouldIncludeCharacterInMaintList mirrors maintMode == 0 || gmlevel > 0.
inline auto ShouldIncludeCharacterInMaintList(const uint8 maintMode, const int32 gmlevel) -> bool
{
    return maintMode == 0 || gmlevel > 0;
}

} // namespace loginHelpers
