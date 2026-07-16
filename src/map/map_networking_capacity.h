#pragma once

#include <cstddef>
#include <span>

#include <common/mmo.h>

namespace mapnetworkinghelpers
{

// ShouldOpenSocket mirrors MapNetworking construction: embedded test servers
// skip UDP socket creation.
inline auto ShouldOpenSocket(const bool isTestServer) -> bool
{
    return !isTestServer;
}

// IncomingEncryptedBlockCount returns the number of complete eight-byte
// Blowfish blocks after the fixed FFXI packet header. A partial trailing block
// is deliberately left untouched by MapNetworking::decodeIncomingPacket.
inline auto IncomingEncryptedBlockCount(const std::size_t packetSize) -> std::size_t
{
    if (packetSize <= FFXI_HEADER_SIZE)
    {
        return 0;
    }

    return (packetSize - FFXI_HEADER_SIZE) / 8;
}

// HasValidIncomingPacketSize mirrors the envelope bounds guard before inbound
// packet decryption. A frame needs the header, compression marker, bit-count,
// and MD5 trailer, and must fit the caller's fixed output buffer.
inline auto HasValidIncomingPacketSize(const std::size_t packetSize, const std::size_t outputCapacity) -> bool
{
    constexpr auto minimumPacketSize = FFXI_HEADER_SIZE + 1 + sizeof(uint32) + 16;
    return packetSize >= minimumPacketSize && packetSize <= outputCapacity;
}

// HasValidCompressedBitCount checks the bit-count trailer against the decoded
// compressed payload length. The marker byte is intentionally checked by the
// caller because it is a distinct protocol rule.
inline auto HasValidCompressedBitCount(const uint32 bitSize, const std::size_t compressedSize) -> bool
{
    constexpr auto minimumCompressedSize = std::size_t{ 1 } + sizeof(uint32);
    const auto     byteSize              = (static_cast<uint64>(bitSize) + 7) / 8;
    return compressedSize >= minimumCompressedSize && bitSize >= 8 && byteSize == compressedSize - sizeof(uint32);
}

// HasCompressedPayloadMarker mirrors the leading marker check on a decrypted
// map payload. Callers must ensure the payload has at least one byte.
inline auto HasCompressedPayloadMarker(const uint8 marker) -> bool
{
    return marker == 1;
}

// IsUnencryptedLoginPacketID identifies the map protocol's unencrypted login
// packet accepted before a session has a Blowfish key.
inline auto IsUnencryptedLoginPacketID(const uint16 packetID) -> bool
{
    return packetID == 0x00A;
}

// MapPacketID extracts the low nine-bit map packet identifier from its wire
// header word.
inline auto MapPacketID(const uint16 headerWord) -> uint16
{
    return headerWord & 0x1FF;
}

// HasUnencryptedLoginPacketSize verifies that a frame can contain the FFXI
// header plus the complete unencrypted login packet.
inline auto HasUnencryptedLoginPacketSize(const std::size_t packetSize, const std::size_t loginPacketSize) -> bool
{
    return packetSize >= FFXI_HEADER_SIZE + loginPacketSize;
}

// IsChecksumValid mirrors the common checksum convention used by map packet
// parsing: zero denotes a valid digest.
inline auto IsChecksumValid(const int32 checksumResult) -> bool
{
    return checksumResult == 0;
}

// HasValidUnencryptedLoginPacketChecksum mirrors the byte-sum validation for
// the unencrypted 0x00A login packet. The sum starts at the packet's MyIP
// field and wraps naturally at eight bits.
inline auto HasValidUnencryptedLoginPacketChecksum(const std::span<const uint8> packet, const std::size_t checksumOffset, const uint8 expectedChecksum) -> bool
{
    if (checksumOffset > packet.size())
    {
        return false;
    }

    uint8 checksum = 0;
    for (const auto byte : packet.subspan(checksumOffset))
    {
        checksum += byte;
    }
    return checksum == expectedChecksum;
}

} // namespace mapnetworkinghelpers
