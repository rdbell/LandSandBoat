/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_s2c_gm_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x012_gm.h"
#include "map/packets/s2c/0x013_gmcommand.h"

namespace
{

constexpr auto gmMesOffset              = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GM::PacketData, Mes);
constexpr auto gmPacketDefaultSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GM::PacketData);
constexpr auto gmCommandUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMCOMMAND::PacketData, GMUniqueNo);
constexpr auto gmCommandMesOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMCOMMAND::PacketData, Mes);
constexpr auto gmCommandPacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GMCOMMAND::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c GM packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto value : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c GM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_GM::PacketData), 248, "GM sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(gmPacketDefaultSize, 252, "GM packet default size") && ok;
    ok      = expectEqualUInt(gmMesOffset, 4, "GM Mes offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_GMCOMMAND::PacketData), 248, "GMCOMMAND sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(gmCommandPacketDefaultSize, 252, "GMCOMMAND packet default size") && ok;
    ok      = expectEqualUInt(gmCommandUniqueNoOffset, 4, "GMCOMMAND GMUniqueNo offset") && ok;
    ok      = expectEqualUInt(gmCommandMesOffset, 8, "GMCOMMAND Mes offset") && ok;
    return ok;
}

auto testGMConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_GM();
    packet.setSequence(0xBEEF);

    auto expected = std::array<uint8, gmPacketDefaultSize>{};
    expected[0]   = 0x12;
    expected[1]   = 0x7E;
    expected[2]   = 0xEF;
    expected[3]   = 0xBE;

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x012, "GM type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "GM size") && ok;
    ok      = expectBytes(packet, expected, "encoded GM packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "GM tail") && ok;
    return ok;
}

auto testGMCommandConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_GMCOMMAND();
    packet.setSequence(0xBEEF);

    auto expected = std::array<uint8, gmCommandPacketDefaultSize>{};
    expected[0]   = 0x13;
    expected[1]   = 0x7E;
    expected[2]   = 0xEF;
    expected[3]   = 0xBE;

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x013, "GMCOMMAND type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "GMCOMMAND size") && ok;
    ok      = expectBytes(packet, expected, "encoded GMCOMMAND packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "GMCOMMAND tail") && ok;
    return ok;
}

} // namespace

auto runS2CGMPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testGMConstructor() && ok;
    ok      = testGMCommandConstructor() && ok;
    return ok;
}
