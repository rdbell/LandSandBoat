#pragma once

#include <cstddef>
#include <span>
#include <string>

#include <fmt/format.h>

#include <common/mmo.h>
#include <common/zlib.h>

// Pure MapNetworking capacity helpers extracted so native tests can pin policy
// without sockets, Blowfish keys, or session tables.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2660: ShouldOpenSocket (!isTestServer) — residual pure port
//   - 2948: ShouldOpenSocket residual dual-wire suite
//   - 3169: ShouldOpenSocket prior dedicated dual-wire (socket_gate.go)
//   - 3232: ShouldOpenSocket dedicated dual-wire expand residual 2948
//   - 2711: ShouldMarkCurrentKeyDecryption (decryptCount == 0) — residual pure port
//   - 2995: ShouldMarkCurrentKeyDecryption (decryptCount == 0) dual-wire expansion
//   - 3336: ShouldMarkCurrentKeyDecryption dedicated dual-wire expand residual 2995
//
// Dual-wire index:
//   - 2660: ShouldOpenSocket residual pure port
//   - 2948: ShouldOpenSocket residual dual-wire suite
//   - 3169: ShouldOpenSocket prior dedicated dual-wire
//   - 3232: ShouldOpenSocket = !isTestServer
//   - 2711: ShouldMarkCurrentKeyDecryption residual pure port
//   - 2995: ShouldMarkCurrentKeyDecryption residual dual-wire suite
//   - 3336: ShouldMarkCurrentKeyDecryption = decryptCount == 0
//
// Production host: MapNetworking constructor (map_networking.cpp) injects
// config_.isTestServer into ShouldOpenSocket before MapSocket allocation.
// Go dual-wire: mapwire.ShouldOpenSocket (internal/mapwire/socket_gate.go).
// Residual dual-wire suite: 2948 (test_mapwire_open_socket_2948).
// Prior dedicated dual-wire suite: 3169 (test_mapwire_open_socket_3169).
// Dedicated dual-wire suite: 3232 (test_mapwire_open_socket_3232).
//
// Production host: MapNetworking::recv_parse (map_networking.cpp ~400) injects
// local decryptCount into ShouldMarkCurrentKeyDecryption before setting
// PSession->hasDecryptedPacket. Go dual-wire:
// mapwire.ShouldMarkCurrentKeyDecryption (internal/mapwire/current_key.go).
// Residual dual-wire suite: 2995 (test_mapwire_mark_current_key_2995).
// Dedicated dual-wire suite: 3336 (test_mapwire_mark_current_key_3336).

namespace mapnetworkinghelpers
{

// ---------------------------------------------------------------------------
// Slice 3232 — MapNetworking constructor socket open gate
// (dedicated expand residual 2948)
// ---------------------------------------------------------------------------

// ShouldOpenSocket mirrors MapNetworking construction: embedded test servers
// skip UDP socket creation.
//
// Formula (slice 3232 dedicated dual-wire; residual expand 2948 / pure 2660 —
// formula unchanged):
//   ShouldOpenSocket(isTestServer) = !isTestServer
//
// isTestServer — host-evaluated config_.isTestServer (MapConfig)
// false → open UDP MapSocket (production map process)
// true  → skip socket creation (embedded test map server)
//
// Dual-wire of Go mapwire.ShouldOpenSocket.
// Call site: MapNetworking constructor before MapSocket allocation.
// Prior pure port: slice 2660. Residual dual-wire suite: 2948 /
// test_mapwire_open_socket_2948. Prior dedicated dual-wire suite: 3169 /
// test_mapwire_open_socket_3169. Dedicated dual-wire suite is
// test_mapwire_open_socket_3232. Host still owns MapSocket construction,
// port selection, and failure exit. Formula is unchanged; this slice only
// expands dual-wire docs + index + dedicated suite.
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

// ShouldDispatchIncomingSmallPacket mirrors MapNetworking's in-order window:
// only sequences newer than the last processed client packet and no newer than
// the enclosing datagram sequence are dispatched. The native comparisons are
// intentionally ordinary unsigned comparisons, not wrap-aware arithmetic.
inline auto ShouldDispatchIncomingSmallPacket(const uint16 packetSequence, const uint16 lastClientPacketSequence, const uint16 datagramSequence) -> bool
{
    return packetSequence > lastClientPacketSequence && packetSequence <= datagramSequence;
}

enum class AcknowledgementPlan : uint8
{
    IncrementServerPacketID,
    IgnoreLoginMismatch,
    ReplayCachedPacket,
};

// PlanOutgoingPacketAcknowledgement mirrors the post-parse acknowledgement
// handling. Acknowledged packets advance the server sequence; a mismatched
// login packet is ignored to avoid caching zone login traffic; other
// mismatches resend the cached outgoing packet.
inline auto PlanOutgoingPacketAcknowledgement(const uint16 acknowledgedServerPacketID, const uint16 serverPacketID, const uint16 lastPacketType) -> AcknowledgementPlan
{
    if (acknowledgedServerPacketID == serverPacketID)
    {
        return AcknowledgementPlan::IncrementServerPacketID;
    }

    return lastPacketType == 0x00A ? AcknowledgementPlan::IgnoreLoginMismatch : AcknowledgementPlan::ReplayCachedPacket;
}

// NextPacketCountForCompressionRetry mirrors send_parse's retry backoff after
// each compression attempt. It removes one integer third of the current
// candidate count, so counts below three intentionally remain unchanged.
inline auto NextPacketCountForCompressionRetry(const std::size_t packetCount) -> std::size_t
{
    return packetCount - packetCount / 3;
}

// CanAppendPacketToCompression mirrors send_parse's packet aggregation gate.
// The byte limit is deliberately strict: a packet that exactly fills the
// buffer is deferred to a later send.
inline auto CanAppendPacketToCompression(const std::size_t bufferSize, const std::size_t packetSize, const std::size_t maximumBufferSize, const std::size_t appendedPackets, const std::size_t packetBudget) -> bool
{
    return bufferSize + packetSize < maximumBufferSize && appendedPackets < packetBudget;
}

enum class IncomingDecryptionPlan : uint8
{
    UsePrimary,
    UsePrevious,
    Reject,
};

// PlanIncomingDecryption mirrors recv_parse's key-rollover fallback. A
// successfully decrypted primary packet always wins. A failed primary packet
// may use the prior key only while zoning and only if that decryption succeeds.
inline auto PlanIncomingDecryption(const bool primaryDecrypted, const bool pendingZone, const bool previousDecrypted) -> IncomingDecryptionPlan
{
    if (primaryDecrypted)
    {
        return IncomingDecryptionPlan::UsePrimary;
    }
    if (pendingZone && previousDecrypted)
    {
        return IncomingDecryptionPlan::UsePrevious;
    }
    return IncomingDecryptionPlan::Reject;
}

// ---------------------------------------------------------------------------
// Slice 3336 — MapNetworking recv_parse current-key decryption mark
// (dedicated expand residual 2995)
// ---------------------------------------------------------------------------

// ShouldMarkCurrentKeyDecryption identifies packets successfully decrypted by
// the current key. recv_parse uses zero for that result and one for a
// previous-key zone-transition fallback.
//
// Formula (slice 3336 dedicated dual-wire; residual expand 2995 / pure 2711 —
// formula unchanged):
//   ShouldMarkCurrentKeyDecryption(decryptCount) = decryptCount == 0
//
// decryptCount — host-injected recv_parse key-attempt result:
//   0  → current-key success (mark hasDecryptedPacket)
//   1  → previous-key zone-transition fallback (do not mark current key)
//  -1  → reject / decrypt failure path (host returns early; still false)
//   2+ → out-of-band / future counts (not current-key success)
//
// Dual-wire of Go mapwire.ShouldMarkCurrentKeyDecryption.
// Call site: MapNetworking::recv_parse after PlanIncomingDecryption; host
// injects local decryptCount (0 primary, ++ on UsePrevious) before
// PSession->hasDecryptedPacket = true when the free function is true.
// Residual pure port: slice 2711. Residual dual-wire suite: 2995 /
// test_mapwire_mark_current_key_2995. Dedicated dual-wire suite is
// test_mapwire_mark_current_key_3336. Host still owns PlanIncomingDecryption,
// primary/previous decipher, buffer copy on UsePrevious, and writing
// hasDecryptedPacket. Formula is unchanged; this slice only expands dual-wire
// docs + index + dedicated suite.
inline auto ShouldMarkCurrentKeyDecryption(const int decryptCount) -> bool
{
    return decryptCount == 0;
}

// ShouldTapLastUpdate mirrors recv_parse's session-liveness gate. Packets
// received while zoning or waiting for zone entry must not keep an old
// character session alive.
inline auto ShouldTapLastUpdate(const bool pendingZone, const bool waitingForZone) -> bool
{
    return !pendingZone && !waitingForZone;
}

enum class IncomingPacketZonePlan : uint8
{
    Dispatch,
    SkipUnexpectedPendingZone,
    WarnAndSkipUnexpected,
};

// PlanIncomingPacketForZone mirrors parse's zone-presence gate. Packets other
// than LOGIN cannot be dispatched before a character has a zone; packets from
// a prior key while zoning are expected and therefore skipped without warning.
inline auto PlanIncomingPacketForZone(const bool characterHasZone, const bool isLoginPacket, const bool pendingZone) -> IncomingPacketZonePlan
{
    if (characterHasZone || isLoginPacket)
    {
        return IncomingPacketZonePlan::Dispatch;
    }
    return pendingZone ? IncomingPacketZonePlan::SkipUnexpectedPendingZone : IncomingPacketZonePlan::WarnAndSkipUnexpected;
}

enum class CompressionFailurePlan : uint8
{
    DropOldestPacketAndRetry,
    ClearOutputAndFail,
};

// PlanCompressionFailure mirrors send_parse's recovery after compression
// reports failure. A queued packet is discarded one at a time to make
// progress; an empty queue clears the outgoing buffer and terminates.
inline auto PlanCompressionFailure(const bool hasQueuedPackets) -> CompressionFailurePlan
{
    return hasQueuedPackets ? CompressionFailurePlan::DropOldestPacketAndRetry : CompressionFailurePlan::ClearOutputAndFail;
}

// CompressPacketResultPlan is the early gate inside compressPacket after
// zlib_compress returns. Reject maps to nullopt; Proceed continues to the
// compressed-size trailer write.
enum class CompressPacketResultPlan : uint8
{
    Reject,
    Proceed,
};

// PlanCompressPacket mirrors compressPacket's zlib failure gate.
// zlibFailed is true when zlib_compress returned -1.
inline auto PlanCompressPacket(const bool zlibFailed) -> CompressPacketResultPlan
{
    return zlibFailed ? CompressPacketResultPlan::Reject : CompressPacketResultPlan::Proceed;
}

// ShouldRejectCompressFailure is the bool form of PlanCompressPacket(true).
// True when zlib_compress returned -1 (result == -1).
inline auto ShouldRejectCompressFailure(const bool zlibResultIsNegativeOne) -> bool
{
    return zlibResultIsNegativeOne;
}

// CompressSucceeded is the inverse gate: true when zlib_compress did not
// return the -1 failure sentinel.
inline auto CompressSucceeded(const int32 zlibResult) -> bool
{
    return zlibResult != -1;
}

// ShouldUsePreviousKeyForOutgoingPacket mirrors finalizePacket's key choice.
// A previous key is valid only for an explicitly requested send while the
// session is pending its zone transition.
inline auto ShouldUsePreviousKeyForOutgoingPacket(const bool pendingZone, const bool usePreviousKey) -> bool
{
    return pendingZone && usePreviousKey;
}

// ShouldIncrementKeyAfterEncrypt mirrors send_parse's zone-out trigger. The
// key must advance only after the 0x00B packet has been encrypted and sent.
inline auto ShouldIncrementKeyAfterEncrypt(const uint16 packetType) -> bool
{
    return packetType == 0x00B;
}

// ShouldSendCharZoneAfterKeyIncrement mirrors send_parse's post-zone-out IPC
// gate. A real logout rotates its key but does not announce a destination.
inline auto ShouldSendCharZoneAfterKeyIncrement(const bool isLogout) -> bool
{
    return !isLogout;
}

// ZoneOutSessionUpdatePlan describes which accounts_sessions columns send_parse
// should rewrite when an outgoing 0x00B zone-out packet is encrypted.
// Session fields zone_ipp and zone_type are always stored by the host when
// ShouldIncrementKeyAfterEncrypt has already gated entry into this path.
struct ZoneOutSessionUpdatePlan
{
    bool updateServerEndpoint{};  // true for zone transition (not full logout)
    bool clearClientPort{};       // always true
    bool stampLastZoneoutTime{};  // always true
};

// PlanZoneOutSessionUpdate mirrors send_parse's accounts_sessions update choice
// after a LOGOUT-type packet is detected. isLogout mirrors
// zone_type == GP_GAME_LOGOUT_STATE::LOGOUT. SQL remains host-owned.
inline auto PlanZoneOutSessionUpdate(const bool isLogout) -> ZoneOutSessionUpdatePlan
{
    return {
        .updateServerEndpoint = !isLogout,
        .clearClientPort      = true,
        .stampLastZoneoutTime = true,
    };
}

// ShouldResetCharacterForUnencryptedLogin mirrors recv_parse's 0x00A cleanup
// of the old character during a pending zone transition.
inline auto ShouldResetCharacterForUnencryptedLogin(const bool pendingZone) -> bool
{
    return pendingZone;
}

// CompressedBitSizeTrailerBytes is the little-endian bit-count trailer length
// written after zlib_compress in compressPacket.
constexpr std::size_t CompressedBitSizeTrailerBytes = sizeof(uint32);

// CompressedBitSizeTrailerOffset is the byte offset of the LE bit-size trailer
// after a successful zlib_compress: zlib_compressed_size(bitSize).
inline auto CompressedBitSizeTrailerOffset(const std::size_t bitSize) -> std::size_t
{
    return zlib_compressed_size(bitSize);
}

// CompressedPayloadSize is the total compressed payload length after the
// bit-size trailer is written: zlib_compressed_size(bitSize) + 4.
inline auto CompressedPayloadSize(const std::size_t bitSize) -> std::size_t
{
    return zlib_compressed_size(bitSize) + CompressedBitSizeTrailerBytes;
}

// MD5DigestSize is the digest length appended after the compressed payload in
// finalizePacket. The host writes the digest at PacketSize, then advances
// PacketSize by this constant before the overflow check and Blowfish step.
constexpr std::size_t MD5DigestSize = 16;

// PacketSizeAfterMD5 returns the payload length after the MD5 trailer is
// accounted for: compressedPayloadSize + MD5DigestSize. MD5 computation and
// byte copy remain host-owned; this is only the size bump.
inline auto PacketSizeAfterMD5(const std::size_t compressedPayloadSize) -> std::size_t
{
    return compressedPayloadSize + MD5DigestSize;
}

// OutgoingCypherWordCount mirrors finalizePacket's even word count for Blowfish
// encryption of the payload (post-MD5 size, excluding the FFXI header). The
// floor to an even number of 4-byte words drops any trailing partial 8-byte block.
inline auto OutgoingCypherWordCount(const uint32 packetSize) -> uint32
{
    return (packetSize / 4) & ~1u;
}

// OutgoingCypherBlockCount converts an even word count into the number of
// 64-bit Blowfish blocks passed to blowfish_encipher_blocks.
inline auto OutgoingCypherBlockCount(const uint32 wordCount) -> uint32
{
    return wordCount / 2;
}

// ShouldReportScratchBufferOverflow mirrors finalizePacket's post-MD5
// overflow guard. True when the payload (including the 16-byte MD5 trailer)
// exceeds the fixed scratch buffer; finalizePacket still continues either way.
inline auto ShouldReportScratchBufferOverflow(const std::size_t packetSize, const std::size_t maxBufferSize) -> bool
{
    return packetSize > maxBufferSize;
}

// FormatScratchBufferOverflowCritical mirrors ShowCritical's overflow text
// after MD5 is appended: "Network: PScratchBuffer is overflowed (%u)".
inline auto FormatScratchBufferOverflowCritical(const std::size_t packetSize) -> std::string
{
    return fmt::format("Network: PScratchBuffer is overflowed ({})", packetSize);
}

// FinalOutgoingPacketSize returns the full wire length assigned to *buffsize
// after encryption: post-MD5 payload size plus the FFXI header.
inline auto FinalOutgoingPacketSize(const std::size_t payloadSize, const std::size_t headerSize) -> std::size_t
{
    return payloadSize + headerSize;
}

// DynamicTargIdCapacityPerZone is the dynamic targid pool size counted per
// active zone in MapNetworking::flushStatistics. Matches
// CZoneEntities' [0x700, 0x900) range (0x1FF = 511 entities).
constexpr std::size_t DynamicTargIdCapacityPerZone = 511;

// AccumulateDynamicTargIdCapacity returns the process-wide capacity used for
// the Dynamic TargID Usage (%) statistic: activeZoneCount * 511.
inline auto AccumulateDynamicTargIdCapacity(const std::size_t activeZoneCount) -> std::size_t
{
    return activeZoneCount * DynamicTargIdCapacityPerZone;
}

// DynamicTargIdUsagePercent mirrors flushStatistics' utilization formula:
//   percent = capacity > 0 ? (double)count / (double)capacity * 100.0 : 0.0
//   return static_cast<int64>(percent)  // truncates toward zero
inline auto DynamicTargIdUsagePercent(const std::size_t count, const std::size_t capacity) -> int64
{
    if (capacity == 0)
    {
        return 0;
    }

    const auto percent = static_cast<double>(count) / static_cast<double>(capacity) * 100.0;
    return static_cast<int64>(percent);
}

} // namespace mapnetworkinghelpers
