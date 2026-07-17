#include "test_map_compress_packet_size_2785.h"

#include "common/zlib.h"
#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapCompressPacketSize2785SelfTests() -> bool
{
    using mapnetworkinghelpers::CompressedBitSizeTrailerBytes;
    using mapnetworkinghelpers::CompressedBitSizeTrailerOffset;
    using mapnetworkinghelpers::CompressedPayloadSize;

    if (CompressedBitSizeTrailerBytes != 4)
    {
        std::cerr << "map compress packet size 2785 self-test failed: CompressedBitSizeTrailerBytes\n";
        return false;
    }

    struct size_case_t
    {
        std::size_t bitSize;
        std::size_t expectedOffset;
        std::size_t expectedPayloadSize;
    };

    // Mirrors zlib_compressed_size golden cases plus the +4 trailer.
    constexpr auto cases = std::array{
        size_case_t{ 0, 0, 4 },
        size_case_t{ 1, 1, 5 },
        size_case_t{ 7, 1, 5 },
        size_case_t{ 8, 1, 5 },
        size_case_t{ 9, 2, 6 },
        size_case_t{ 15, 2, 6 },
        size_case_t{ 16, 2, 6 },
        size_case_t{ 17, 3, 7 },
        size_case_t{ 63, 8, 12 },
        size_case_t{ 64, 8, 12 },
        size_case_t{ 65, 9, 13 },
    };

    for (const auto& test : cases)
    {
        if (CompressedBitSizeTrailerOffset(test.bitSize) != test.expectedOffset)
        {
            std::cerr << "map compress packet size 2785 self-test failed: trailer offset for bitSize="
                      << test.bitSize << '\n';
            return false;
        }
        if (CompressedPayloadSize(test.bitSize) != test.expectedPayloadSize)
        {
            std::cerr << "map compress packet size 2785 self-test failed: payload size for bitSize="
                      << test.bitSize << '\n';
            return false;
        }
        // Offset is always zlib_compressed_size; payload is offset + trailer.
        if (CompressedBitSizeTrailerOffset(test.bitSize) != zlib_compressed_size(test.bitSize))
        {
            std::cerr << "map compress packet size 2785 self-test failed: offset reuse for bitSize="
                      << test.bitSize << '\n';
            return false;
        }
        if (CompressedPayloadSize(test.bitSize) !=
            CompressedBitSizeTrailerOffset(test.bitSize) + CompressedBitSizeTrailerBytes)
        {
            std::cerr << "map compress packet size 2785 self-test failed: payload = offset + 4 for bitSize="
                      << test.bitSize << '\n';
            return false;
        }
    }

    // compressPacket path after zlib_compress succeeds with bitSize `result`:
    //   packetSize = result
    //   write uint32 LE at CompressedBitSizeTrailerOffset(packetSize)
    //   packetSize = CompressedPayloadSize(packetSize)
    const std::size_t result    = 17; // partial final byte
    const auto        packetSize = CompressedPayloadSize(result);
    if (CompressedBitSizeTrailerOffset(result) != 3 || packetSize != 7)
    {
        std::cerr << "map compress packet size 2785 self-test failed: compressPacket path\n";
        return false;
    }

    return true;
}
