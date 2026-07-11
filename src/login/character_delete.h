#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure VIEW 0x14 character-delete ownership gates and diagnostics extracted so
// native tests can pin policy without sockets/SQL.

namespace loginHelpers
{

// character_delete_ownership_gate is the pure outcome after looking up accid by
// charid for VIEW 0x14. Production leaves lookedUpAccountID at 0 when the query
// fails or returns no row, so a missing character is treated the same as a
// foreign account unless session.accountID is also 0.
enum class character_delete_ownership_gate : uint8_t
{
    PROCEED, // lookedUpAccountID == sessionAccountID
    DENIED,  // lookedUpAccountID != sessionAccountID (includes missing row → 0)
};

// ClassifyCharacterDeleteOwnership mirrors the single ownership check in
// VIEW 0x14 after SELECT accid FROM chars WHERE charid = ?.
inline auto ClassifyCharacterDeleteOwnership(
    const uint32 lookedUpAccountID,
    const uint32 sessionAccountID) -> character_delete_ownership_gate
{
    if (lookedUpAccountID != sessionAccountID)
    {
        return character_delete_ownership_gate::DENIED;
    }
    return character_delete_ownership_gate::PROCEED;
}

// LookedUpAccountIDFromDeleteQuery mirrors production: 0 when the query is
// missing/failed/empty, otherwise the row's accid. Pure helper so hosts can
// feed ClassifyCharacterDeleteOwnership without re-encoding the 0 default.
inline auto LookedUpAccountIDFromDeleteQuery(const bool queryOk, const bool rowFound, const uint32 rowAccountID) -> uint32
{
    if (queryOk && rowFound)
    {
        return rowAccountID;
    }
    return 0;
}

// FormatCharacterDeleteWrongAccount mirrors the ShowError text when ownership
// is denied (missing char or foreign account).
inline auto FormatCharacterDeleteWrongAccount(const uint32 sessionAccountID) -> std::string
{
    return fmt::format("Account ID {} tried to delete character not in their account.", sessionAccountID);
}

// FormatCharacterDeleteAttemptInfo mirrors the ShowInfo text logged after the
// 0x20 ack is sent and before the ownership query.
inline auto FormatCharacterDeleteAttemptInfo(const uint32 charID, const std::string& ipAddress) -> std::string
{
    return fmt::format("attempt to delete char:<{}> from ip:<{}>", charID, ipAddress);
}

} // namespace loginHelpers
