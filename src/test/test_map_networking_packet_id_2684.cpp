#include "test_map_networking_packet_id_2684.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingPacketID2684SelfTests() -> bool
{
    constexpr auto cases = std::array{ std::pair{ uint16{ 0 }, uint16{ 0 } }, std::pair{ uint16{ 0x00A }, uint16{ 0x00A } }, std::pair{ uint16{ 0xFE0A }, uint16{ 0x00A } }, std::pair{ uint16{ 0xFFFF }, uint16{ 0x1FF } } };
    for (const auto& [word, expected] : cases)
    {
        if (mapnetworkinghelpers::MapPacketID(word) != expected)
        {
            std::cerr << "map networking packet ID 2684 self-test failed\n";
            return false;
        }
    }
    return true;
}
