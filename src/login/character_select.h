#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fmt/format.h>
#include <string>

// Pure VIEW 0x07 character-selection ownership gates and data-session notify
// packet helpers extracted so native tests can pin policy without sockets/SQL.

namespace loginHelpers
{

// Packet field offsets for VIEW 0x07 "Notifying lobby server of current selections."
constexpr std::size_t CharacterSelectIDOffset   = 28;
constexpr std::size_t CharacterSelectNameOffset = 36;

// character_select_gate is the pure outcome after the charid+charname lookup.
enum class character_select_gate : uint8_t
{
    PROCEED,         // row found and accid matches session.accountID
    MISMATCHED_NAME, // query failed or no row for charid+charname
    WRONG_ACCOUNT,   // row found but accid != session.accountID
};

// ClassifyCharacterSelect mirrors the two sequential checks in VIEW 0x07.
// queryOk/rowFound describe the SELECT accid FROM chars WHERE charid AND charname result.
inline auto ClassifyCharacterSelect(
    const bool   queryOk,
    const bool   rowFound,
    const uint32 rowAccountID,
    const uint32 sessionAccountID) -> character_select_gate
{
    if (!queryOk || !rowFound)
    {
        return character_select_gate::MISMATCHED_NAME;
    }
    if (rowAccountID != sessionAccountID)
    {
        return character_select_gate::WRONG_ACCOUNT;
    }
    return character_select_gate::PROCEED;
}

// FormatCharacterSelectMismatchedName mirrors the ShowError text when the
// charid+charname pair is not found (or the query fails).
inline auto FormatCharacterSelectMismatchedName(const uint32 sessionAccountID) -> std::string
{
    return fmt::format("Account ID {} tried to select a character id with a mismatched character name.", sessionAccountID);
}

// FormatCharacterSelectWrongAccount mirrors the ShowError text when the
// character belongs to a different account.
inline auto FormatCharacterSelectWrongAccount(const uint32 sessionAccountID) -> std::string
{
    return fmt::format("Account ID {} tried to login as character not in their account.", sessionAccountID);
}

// DataSelectNotifyPacketSize is the fixed write length for the data_session
// nudge after a successful VIEW 0x07 selection.
constexpr std::size_t DataSelectNotifyPacketSize = 0x05;

// DataSelectNotifyCommand is buffer_[0] of that 5-byte notify.
constexpr uint8 DataSelectNotifyCommand = 0x02;

// GenerateDataSelectNotifyPacket fills a 5-byte buffer with zeros and sets
// byte 0 to DataSelectNotifyCommand. packet must point to at least
// DataSelectNotifyPacketSize bytes.
inline void GenerateDataSelectNotifyPacket(uint8* packet)
{
    std::memset(packet, 0, DataSelectNotifyPacketSize);
    packet[0] = DataSelectNotifyCommand;
}

// FormatMissingSessionHashWarning mirrors the VIEW entry warning when
// getHashFromPacket returns an empty session hash.
inline auto FormatMissingSessionHashWarning(const std::string& ipAddress) -> std::string
{
    return fmt::format("Session requested without valid sessionHash from {}", ipAddress);
}

} // namespace loginHelpers
