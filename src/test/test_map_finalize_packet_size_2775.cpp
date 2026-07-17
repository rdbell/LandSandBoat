#include "test_map_finalize_packet_size_2775.h"

#include "common/cbasetypes.h"
#include "common/mmo.h"
#include "map/map_constants.h"
#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map finalize packet size 2775 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapFinalizePacketSize2775SelfTests() -> bool
{
    using mapnetworkinghelpers::FinalOutgoingPacketSize;
    using mapnetworkinghelpers::FormatScratchBufferOverflowCritical;
    using mapnetworkinghelpers::ShouldReportScratchBufferOverflow;

    bool ok = true;

    // ShouldReportScratchBufferOverflow: packetSize > maxBufferSize
    ok = expect(!ShouldReportScratchBufferOverflow(0, kMaxBufferSize), "0 does not overflow") && ok;
    ok = expect(!ShouldReportScratchBufferOverflow(kMaxBufferSize, kMaxBufferSize), "exact max does not overflow") && ok;
    ok = expect(ShouldReportScratchBufferOverflow(kMaxBufferSize + 1, kMaxBufferSize), "max+1 overflows") && ok;
    ok = expect(ShouldReportScratchBufferOverflow(100, 50), "100 > 50 overflows") && ok;
    ok = expect(!ShouldReportScratchBufferOverflow(50, 50), "50 == 50 does not overflow") && ok;
    ok = expect(!ShouldReportScratchBufferOverflow(49, 50), "49 < 50 does not overflow") && ok;

    // FinalOutgoingPacketSize: payload + header
    ok = expect(FinalOutgoingPacketSize(0, FFXI_HEADER_SIZE) == FFXI_HEADER_SIZE, "empty payload + header") && ok;
    ok = expect(FinalOutgoingPacketSize(16, FFXI_HEADER_SIZE) == 16 + FFXI_HEADER_SIZE, "MD5-only payload + header") && ok;
    ok = expect(FinalOutgoingPacketSize(100, FFXI_HEADER_SIZE) == 100 + FFXI_HEADER_SIZE, "100 + header") && ok;
    ok = expect(FinalOutgoingPacketSize(kMaxBufferSize, FFXI_HEADER_SIZE) == kMaxBufferSize + FFXI_HEADER_SIZE, "max payload + header") && ok;
    ok = expect(FinalOutgoingPacketSize(0, 0) == 0, "0 + 0") && ok;
    ok = expect(FinalOutgoingPacketSize(8, 28) == 36, "8 + 28") && ok;

    // FormatScratchBufferOverflowCritical mirrors ShowCritical text
    ok = expect(FormatScratchBufferOverflowCritical(2501) == "Network: PScratchBuffer is overflowed (2501)", "format 2501") && ok;
    ok = expect(FormatScratchBufferOverflowCritical(0) == "Network: PScratchBuffer is overflowed (0)", "format 0") && ok;
    ok = expect(FormatScratchBufferOverflowCritical(kMaxBufferSize + 1) == "Network: PScratchBuffer is overflowed (2501)", "format max+1") && ok;

    // Combined path used by finalizePacket after MD5 append:
    // report if over max, then *buffsize = payload + header (continues either way).
    {
        const std::size_t packetSize = kMaxBufferSize + 1;
        ok                           = expect(ShouldReportScratchBufferOverflow(packetSize, kMaxBufferSize), "finalize path reports overflow") && ok;
        ok                           = expect(FinalOutgoingPacketSize(packetSize, FFXI_HEADER_SIZE) == packetSize + FFXI_HEADER_SIZE, "finalize path buffsize on overflow") && ok;
    }
    {
        const std::size_t packetSize = 200;
        ok                           = expect(!ShouldReportScratchBufferOverflow(packetSize, kMaxBufferSize), "finalize path no overflow") && ok;
        ok                           = expect(FinalOutgoingPacketSize(packetSize, FFXI_HEADER_SIZE) == 200 + FFXI_HEADER_SIZE, "finalize path buffsize normal") && ok;
    }

    return ok;
}
