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

#include "test_s2c_grap_list_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "map/entities/char_entity.h"
#include "map/monstrosity.h"
#include "map/packets/s2c/0x051_grap_list.h"

namespace
{

constexpr auto grapListGrapIDTblOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GRAP_LIST::PacketData, GrapIDTbl);
constexpr auto grapListPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GRAP_LIST::PacketData, padding00);
constexpr auto grapListPacketDataSize  = sizeof(GP_SERV_COMMAND_GRAP_LIST::PacketData);
constexpr auto grapListFullPacketSize  = sizeof(GP_SERV_HEADER) + grapListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GRAP_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GRAP_LIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c GRAP_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto sampleLook() -> look_t
{
    auto look  = look_t{};
    look.face  = 0x12;
    look.race  = 0x03;
    look.head  = 0x0021;
    look.body  = 0x0032;
    look.hands = 0x0043;
    look.legs  = 0x0054;
    look.feet  = 0x0065;
    look.main  = 0x0076;
    look.sub   = 0x0087;
    look.ranged = 0x0098;
    return look;
}

auto sampleStyleLockedLook() -> look_t
{
    auto look  = look_t{};
    look.face  = 0x34;
    look.race  = 0x05;
    look.head  = 0x00A1;
    look.body  = 0x00B2;
    look.hands = 0x00C3;
    look.legs  = 0x00D4;
    look.feet  = 0x00E5;
    look.main  = 0x00F6;
    look.sub   = 0x0107;
    look.ranged = 0x0118;
    return look;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(grapListPacketDataSize, 20, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(grapListFullPacketSize, 24, "full packet size") && ok;
    ok      = expectEqualUInt(grapListGrapIDTblOffset, 4, "GrapIDTbl offset") && ok;
    ok      = expectEqualUInt(grapListPadding00Offset, 22, "padding00 offset") && ok;
    return ok;
}

auto testNormalLookConstructor() -> bool
{
    auto character = CCharEntity{};
    character.look = sampleLook();

    auto packet = GP_SERV_COMMAND_GRAP_LIST(&character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x051, "GRAP_LIST type") && ok;
    ok      = expectEqualUInt(packet.getSize(), grapListFullPacketSize, "GRAP_LIST size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 24>{
                              0x51, 0x0C, 0xEF, 0xBE,
                              0x12, 0x03, 0x21, 0x10, 0x32, 0x20, 0x43, 0x30,
                              0x54, 0x40, 0x65, 0x50, 0x76, 0x60, 0x87, 0x70,
                              0x98, 0x80, 0x00, 0x00 },
                          "normal constructor bytes") &&
              ok;
    ok = expectZeroTail(packet, grapListFullPacketSize, "normal constructor tail") && ok;
    return ok;
}

auto testStyleLockedAndHiddenHeadConstructor() -> bool
{
    auto character                     = CCharEntity{};
    character.look                     = sampleLook();
    character.mainlook                 = sampleStyleLockedLook();
    character.playerConfig.DisplayHeadOffFlg = 1;
    character.setStyleLocked(true);

    auto packet = GP_SERV_COMMAND_GRAP_LIST(&character);

    return expectBytes(packet, grapListGrapIDTblOffset, std::array<uint8, 20>{
                           0x34, 0x05, 0x00, 0x00, 0xB2, 0x20, 0xC3, 0x30,
                           0xD4, 0x40, 0xE5, 0x50, 0xF6, 0x60, 0x07, 0x71,
                           0x18, 0x81, 0x00, 0x00 },
                       "style-locked hidden-head payload");
}

auto testCostumeOverrideConstructor() -> bool
{
    auto character      = CCharEntity{};
    character.look      = sampleLook();
    character.m_Costume2 = 0xCAFE;

    auto packet = GP_SERV_COMMAND_GRAP_LIST(&character);

    return expectBytes(packet, grapListGrapIDTblOffset, std::array<uint8, 20>{
                           0xFE, 0xCA, 0x21, 0x10, 0x32, 0x20, 0x43, 0x30,
                           0x54, 0x40, 0x65, 0x50, 0x76, 0x60, 0x87, 0x70,
                           0xFF, 0xFF, 0x00, 0x00 },
                       "costume payload");
}

auto testMonstrosityOverrideConstructor() -> bool
{
    auto character                    = CCharEntity{};
    character.look                    = sampleLook();
    character.m_Costume2              = 0xCAFE;
    character.m_PMonstrosity          = std::make_unique<monstrosity::MonstrosityData_t>();
    character.m_PMonstrosity->Look    = 0x2468;

    auto packet = GP_SERV_COMMAND_GRAP_LIST(&character);

    return expectBytes(packet, grapListGrapIDTblOffset, std::array<uint8, 20>{
                           0x68, 0x24, 0x21, 0x10, 0x32, 0x20, 0x43, 0x30,
                           0x54, 0x40, 0x65, 0x50, 0x76, 0x60, 0x87, 0x70,
                           0xFF, 0xFF, 0x00, 0x00 },
                       "monstrosity payload");
}

} // namespace

auto runS2CGrapListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNormalLookConstructor() && ok;
    ok      = testStyleLockedAndHiddenHeadConstructor() && ok;
    ok      = testCostumeOverrideConstructor() && ok;
    ok      = testMonstrosityOverrideConstructor() && ok;
    return ok;
}
