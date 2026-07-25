#include "test_map_networking_small_packet_6918.h"

#include "map/map_networking_small_packet.h"

#include <array>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking small packet 6918 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingSmallPacket6918SelfTests() -> bool
{
    using mapnetworkingsmallpackethelpers::Next;

    bool ok = true;

    // The first size unit is intentionally odd; parse masks it to 0x02 and
    // advances four bytes to the next frame.
    const std::array<uint8, 12> payload = {
        0x34, 0x03, 0x10, 0x00,
        0xCD, 0x02, 0x11, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    const auto first = Next(payload, 0);
    ok               = expect(first.has_value(), "first frame exists") && ok;
    ok               = expect(first && first->type == 0x134 && first->sequence == 0x0010 && first->sizeUnits == 2 && first->byteSize == 4,
                        "first frame fields") &&
         ok;

    const auto second = Next(payload, first ? first->byteSize : 0);
    ok                = expect(second.has_value(), "second frame exists") && ok;
    ok                = expect(second && second->type == 0x0CD && second->sequence == 0x0011 && second->byteSize == 4,
                         "second frame fields") &&
         ok;
    ok = expect(!Next(payload, 8).has_value(), "zero size terminates") && ok;

    const std::array<uint8, 5> truncated = { 0x01, 0x04, 0x02, 0x00, 0xFF };
    ok                                    = expect(!Next(truncated, 0).has_value(), "truncated frame terminates") && ok;
    ok                                    = expect(!Next(payload, payload.size()).has_value(), "end terminates") && ok;

    return ok;
}
