#include "test_map_networking_compressed_bits_2669.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>
#include <limits>

auto runMapNetworkingCompressedBits2669SelfTests() -> bool
{
    struct case_t
    {
        uint32      bitSize;
        std::size_t compressedSize;
        bool        expected;
    };

    constexpr auto minimumCompressedSize = std::size_t{ 1 } + sizeof(uint32);
    constexpr auto cases = std::array{
        case_t{ 8, minimumCompressedSize - 1, false },
        case_t{ 0, minimumCompressedSize, false },
        case_t{ 8, minimumCompressedSize, true },
        case_t{ 9, minimumCompressedSize + 1, true },
        case_t{ 9, minimumCompressedSize, false },
        case_t{ std::numeric_limits<uint32>::max(), 536870916, true },
    };

    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::HasValidCompressedBitCount(test.bitSize, test.compressedSize) != test.expected)
        {
            std::cerr << "map networking compressed bits 2669 self-test failed\n";
            return false;
        }
    }
    return true;
}
