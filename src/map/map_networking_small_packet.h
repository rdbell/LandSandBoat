#pragma once

#include <cstddef>
#include <optional>
#include <span>

#include <common/cbasetypes.h>

// Pure framing seam for MapNetworking::parse. It decodes one complete C2S
// small-packet header from the decompressed payload without owning session or
// packet-system policy.
namespace mapnetworkingsmallpackethelpers
{

struct SmallPacket
{
    uint16      type;
    uint16      sequence;
    uint8       sizeUnits;
    std::size_t byteSize;
};

// Next returns the frame at offset. A zero size unit or an incomplete frame
// terminates scanning, matching MapNetworking::parse's loop condition.
inline auto Next(const std::span<const uint8> payload, const std::size_t offset) -> std::optional<SmallPacket>
{
    if (offset >= payload.size() || payload.size() - offset < 2)
    {
        return std::nullopt;
    }

    const auto sizeUnits = static_cast<uint8>(payload[offset + 1] & 0xFE);
    if (sizeUnits == 0)
    {
        return std::nullopt;
    }

    const auto byteSize = static_cast<std::size_t>(sizeUnits) * 2;
    if (byteSize > payload.size() - offset || byteSize < 4)
    {
        return std::nullopt;
    }

    const auto type = static_cast<uint16>(static_cast<uint16>(payload[offset]) |
                                          static_cast<uint16>(static_cast<uint16>(payload[offset + 1]) << 8));
    const auto sequence = static_cast<uint16>(static_cast<uint16>(payload[offset + 2]) |
                                              static_cast<uint16>(static_cast<uint16>(payload[offset + 3]) << 8));
    return SmallPacket{
        .type      = static_cast<uint16>(type & 0x1FF),
        .sequence  = sequence,
        .sizeUnits = sizeUnits,
        .byteSize  = byteSize,
    };
}

} // namespace mapnetworkingsmallpackethelpers
