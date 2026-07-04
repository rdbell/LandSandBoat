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

#include "test_s2c_musicvolume_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x060_musicvolume.h"

namespace
{

constexpr auto musicVolumeTimeOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MUSICVOLUME::PacketData, time);
constexpr auto musicVolumeVolumeOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_MUSICVOLUME::PacketData, volume);
constexpr auto musicVolumePacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_MUSICVOLUME::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MUSICVOLUME packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 8>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c MUSICVOLUME packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MUSICVOLUME packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_MUSICVOLUME::PacketData), 4, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(musicVolumePacketDefaultSize, 8, "packet default size") && ok;
    ok      = expectEqualUInt(musicVolumeTimeOffset, 4, "time offset") && ok;
    ok      = expectEqualUInt(musicVolumeVolumeOffset, 6, "volume offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_MUSICVOLUME();
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 8>{
        0x60, 0x04, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x060, "MUSICVOLUME type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "MUSICVOLUME size") && ok;
    ok      = expectBytes(packet, expected, "encoded MUSICVOLUME prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "MUSICVOLUME tail") && ok;
    return ok;
}

} // namespace

auto runS2CMusicVolumePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
