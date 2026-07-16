#include "test_map_networking_compression_retry_2702.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingCompressionRetry2702SelfTests() -> bool
{
    constexpr std::array cases{
        std::pair{ std::size_t{ 0 }, std::size_t{ 0 } },
        std::pair{ std::size_t{ 1 }, std::size_t{ 1 } },
        std::pair{ std::size_t{ 2 }, std::size_t{ 2 } },
        std::pair{ std::size_t{ 3 }, std::size_t{ 2 } },
        std::pair{ std::size_t{ 32 }, std::size_t{ 22 } },
    };
    for (const auto& [packetCount, expected] : cases)
    {
        if (mapnetworkinghelpers::NextPacketCountForCompressionRetry(packetCount) != expected)
        {
            std::cerr << "map networking compression retry 2702 self-test failed\n";
            return false;
        }
    }
    return true;
}
