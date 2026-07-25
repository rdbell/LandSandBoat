#pragma once

#include <common/cbasetypes.h>

// Pure pending-zone resend policy for MapNetworking::handle_incoming_packet.
namespace mapnetworkingzonepacketrebuildhelpers
{

struct PacketPlan
{
    uint16 packetSequence;
    bool   usePreviousKey;
};

struct CompletionPlan
{
    bool   clearOutput;
    bool   incrementPacketsSent;
    uint16 nextServerPacketID;
};

// MakePacketPlan mirrors rebuilding the missed 0x00B packet: it uses the
// current server sequence in the small packet and encrypts with the previous
// key because this path is only reached while the session is pending zone.
inline auto MakePacketPlan(const uint16 serverPacketID) -> PacketPlan
{
    return PacketPlan{
        .packetSequence = serverPacketID,
        .usePreviousKey = true,
    };
}

// MakeCompletionPlan mirrors the end of the rebuild path. LSB sends no payload
// and does not increment TotalPacketsSent when compression fails, but it still
// advances the server packet ID on every rebuild attempt.
inline auto MakeCompletionPlan(const uint16 serverPacketID, const bool compressionSucceeded) -> CompletionPlan
{
    return CompletionPlan{
        .clearOutput           = !compressionSucceeded,
        .incrementPacketsSent  = compressionSucceeded,
        .nextServerPacketID    = static_cast<uint16>(serverPacketID + 1),
    };
}

} // namespace mapnetworkingzonepacketrebuildhelpers
