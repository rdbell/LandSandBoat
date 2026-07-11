#pragma once

#include "common/md52.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

// Pure VIEW lobby acknowledgement packet (0x20) and character-deletion enable
// gate helpers used by view_session commands 0x14 / 0x21 / 0x22.

namespace loginHelpers
{

// ViewLobbyAckPacketSize is the fixed write length for the lobby IXFF ack.
constexpr std::size_t ViewLobbyAckPacketSize = 0x20;

// ViewLobbyAckResult is buffer_[8] for successful name/delete/register acks.
constexpr uint8 ViewLobbyAckResult = 0x03;

// character_deletion_gate is the pure outcome of login.CHARACTER_DELETION.
enum class character_deletion_gate : uint8_t
{
    ALLOW,
    DENIED,
};

// ClassifyCharacterDeletionGate mirrors the 0x14 settings check.
inline auto ClassifyCharacterDeletionGate(const bool deletionEnabled) -> character_deletion_gate
{
    return deletionEnabled ? character_deletion_gate::ALLOW : character_deletion_gate::DENIED;
}

// GenerateViewLobbyAckPacket fills a 0x20-byte buffer with LSB's shared
// lobby success ack: size, IXFF terminator bytes, result 0x03, and MD5
// identifier over the full 0x20-byte zeroed frame (hash written at offset 12).
// packet must point to at least ViewLobbyAckPacketSize bytes.
inline void GenerateViewLobbyAckPacket(uint8* packet)
{
    std::memset(packet, 0, ViewLobbyAckPacketSize);
    packet[0] = static_cast<uint8>(ViewLobbyAckPacketSize);
    packet[4] = 0x49; // I
    packet[5] = 0x58; // X
    packet[6] = 0x46; // F
    packet[7] = 0x46; // F
    packet[8] = ViewLobbyAckResult;

    uint8 hash[16];
    md5(packet, hash, static_cast<int32>(ViewLobbyAckPacketSize));
    std::memcpy(packet + 12, hash, 16);
}

// DeleteKeyIncrement is the session.incrementKeyValue bump after a successful
// character soft-delete on VIEW 0x14.
constexpr uint8 DeleteKeyIncrement = 4;

} // namespace loginHelpers
