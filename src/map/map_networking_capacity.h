#pragma once

#include <cstddef>

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

} // namespace mapnetworkinghelpers
