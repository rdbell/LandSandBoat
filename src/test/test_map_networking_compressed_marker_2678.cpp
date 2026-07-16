#include "test_map_networking_compressed_marker_2678.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingCompressedMarker2678SelfTests() -> bool
{
    constexpr auto cases = std::array{ std::pair{ uint8{ 1 }, true }, std::pair{ uint8{ 0 }, false }, std::pair{ uint8{ 2 }, false }, std::pair{ uint8{ 255 }, false } };
    for (const auto& [marker, expected] : cases)
    {
        if (mapnetworkinghelpers::HasCompressedPayloadMarker(marker) != expected)
        {
            std::cerr << "map networking compressed marker 2678 self-test failed\n";
            return false;
        }
    }
    return true;
}
