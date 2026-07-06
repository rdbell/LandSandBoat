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

#include "test_s2c_res_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x0f9_res.h"

namespace
{

constexpr auto resUniqueNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_RES::PacketData, UniqueNo);
constexpr auto resActIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_RES::PacketData, ActIndex);
constexpr auto resTypeOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_RES::PacketData, type);
constexpr auto resPacketDataSize       = sizeof(GP_SERV_COMMAND_RES::PacketData);
constexpr auto resPacketSize           = sizeof(GP_SERV_HEADER) + resPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c RES packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(CBasicPacket& packet, std::size_t offset, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet) + offset;
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c RES packet self-test failed: " << label << " got";
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
            std::cerr << "s2c RES packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void makeCharacter(CCharEntity& character, std::uint32_t id, std::uint16_t targid)
{
    character.id = id;
    character.targid = targid;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_RES::PacketData), 8, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(resPacketSize, 12, "packet size") && ok;
    ok      = expectEqualUInt(resUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(resActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(resTypeOffset, 10, "type offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_RES_TYPE), 2, "sizeof(GP_SERV_COMMAND_RES_TYPE)") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_RES), 0x0F9, "packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RES_TYPE::Homepoint), 0, "Homepoint enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RES_TYPE::Raise), 1, "Raise enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RES_TYPE::Tractor), 2, "Tractor enum") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto character = CCharEntity{};
    makeCharacter(character, 0x11223344, 0x5566);
    auto packet = GP_SERV_COMMAND_RES(&character, GP_SERV_COMMAND_RES_TYPE::Tractor);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 12>{
        0xF9, 0x06, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x66, 0x55, 0x02, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0F9, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "size") && ok;
    ok      = expectBytes(packet, 0, expected, "encoded packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "tail") && ok;
    return ok;
}

} // namespace

auto runS2CResPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
