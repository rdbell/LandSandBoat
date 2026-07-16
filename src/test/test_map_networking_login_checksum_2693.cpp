#include "test_map_networking_login_checksum_2693.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingLoginChecksum2693SelfTests() -> bool
{
    constexpr std::array<uint8, 5> packet{ 0xF0, 0x0D, 1, 2, 3 };
    const auto                      tailSum = uint8{ 1 + 2 + 3 };
    const bool ok = mapnetworkinghelpers::HasValidUnencryptedLoginPacketChecksum(packet, 2, tailSum) &&
                    !mapnetworkinghelpers::HasValidUnencryptedLoginPacketChecksum(packet, 2, uint8{ tailSum + 1 }) &&
                    mapnetworkinghelpers::HasValidUnencryptedLoginPacketChecksum(packet, packet.size(), 0) &&
                    !mapnetworkinghelpers::HasValidUnencryptedLoginPacketChecksum(packet, packet.size() + 1, 0) &&
                    mapnetworkinghelpers::HasValidUnencryptedLoginPacketChecksum(std::array<uint8, 2>{ 255, 2 }, 0, 1);
    if (!ok)
    {
        std::cerr << "map networking login checksum 2693 self-test failed\n";
    }
    return ok;
}
