#include "test_map_finalize_md5_size_2778.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapFinalizeMD5Size2778SelfTests() -> bool
{
    if (mapnetworkinghelpers::MD5DigestSize != 16)
    {
        std::cerr << "map finalize md5 size 2778 self-test failed: MD5DigestSize\n";
        return false;
    }

    struct size_case_t
    {
        std::size_t compressedPayloadSize;
        std::size_t expectedPacketSize;
    };

    constexpr auto cases = std::array{
        size_case_t{ 0, 16 },
        size_case_t{ 1, 17 },
        size_case_t{ 15, 31 },
        size_case_t{ 16, 32 },
        size_case_t{ 100, 116 },
        size_case_t{ 1284, 1300 }, // client accept edge used near send_parse
        size_case_t{ 1300 - 16, 1300 },
    };

    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::PacketSizeAfterMD5(test.compressedPayloadSize) != test.expectedPacketSize)
        {
            std::cerr << "map finalize md5 size 2778 self-test failed: PacketSizeAfterMD5("
                      << test.compressedPayloadSize << ")\n";
            return false;
        }
    }

    // finalizePacket path: size after MD5 is compressed + digest; cypher sizing
    // consumes that post-MD5 length (slice 2771) independently.
    const auto postMD5 = mapnetworkinghelpers::PacketSizeAfterMD5(20);
    if (postMD5 != 36)
    {
        std::cerr << "map finalize md5 size 2778 self-test failed: finalize path size\n";
        return false;
    }
    if (mapnetworkinghelpers::OutgoingCypherWordCount(static_cast<uint32>(postMD5)) != 8)
    {
        std::cerr << "map finalize md5 size 2778 self-test failed: post-md5 cypher words\n";
        return false;
    }

    return true;
}
