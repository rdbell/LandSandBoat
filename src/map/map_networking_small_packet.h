#pragma once

#include <cstddef>
#include <optional>
#include <span>

#include <common/cbasetypes.h>

#include "map_networking_capacity.h"

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

enum class Disposition : uint8
{
    SkipSequence,
    Dispatch,
    SkipPendingZone,
    WarnAndSkip,
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

// PlanDisposition mirrors the packet-specific part of MapNetworking::parse:
// reject an out-of-window sequence before applying the unzoned-character
// login and pending-zone policy.
inline auto PlanDisposition(
    const uint16 packetSequence,
    const uint16 lastClientPacketSequence,
    const uint16 datagramSequence,
    const bool   characterHasZone,
    const bool   isLoginPacket,
    const bool   pendingZone) -> Disposition
{
    if (!mapnetworkinghelpers::ShouldDispatchIncomingSmallPacket(packetSequence, lastClientPacketSequence, datagramSequence))
    {
        return Disposition::SkipSequence;
    }

    switch (mapnetworkinghelpers::PlanIncomingPacketForZone(characterHasZone, isLoginPacket, pendingZone))
    {
        case mapnetworkinghelpers::IncomingPacketZonePlan::Dispatch:
            return Disposition::Dispatch;
        case mapnetworkinghelpers::IncomingPacketZonePlan::SkipUnexpectedPendingZone:
            return Disposition::SkipPendingZone;
        case mapnetworkinghelpers::IncomingPacketZonePlan::WarnAndSkipUnexpected:
            return Disposition::WarnAndSkip;
    }

    return Disposition::WarnAndSkip;
}

} // namespace mapnetworkingsmallpackethelpers
