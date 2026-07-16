#include "test_map_networking_login_packet_2681.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingLoginPacket2681SelfTests() -> bool
{
    constexpr auto cases = std::array{ std::pair{ uint16{ 0x00A }, true }, std::pair{ uint16{ 0 }, false }, std::pair{ uint16{ 0x009 }, false }, std::pair{ uint16{ 0x00B }, false }, std::pair{ uint16{ 0x1FF }, false } };
    for (const auto& [packetID, expected] : cases)
    {
        if (mapnetworkinghelpers::IsUnencryptedLoginPacketID(packetID) != expected)
        {
            std::cerr << "map networking login packet 2681 self-test failed\n";
            return false;
        }
    }
    return true;
}
