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

#include "test_s2c_gm_support_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0b6_set_gmmsg.h"
#include "map/packets/s2c/0x0b7_gmscitem.h"

namespace
{

constexpr auto setGMMsgMsgIDOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SET_GMMSG::PacketData, msgId);
constexpr auto setGMMsgSeqIDOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SET_GMMSG::PacketData, seqId);
constexpr auto setGMMsgPktNumOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SET_GMMSG::PacketData, pktNum);
constexpr auto setGMMsgMsgOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SET_GMMSG::PacketData, Msg);
constexpr auto setGMMsgPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SET_GMMSG::PacketData);
constexpr auto gmSCItemItemFlagOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMSCITEM::PacketData, ItemFlag);
constexpr auto gmSCItemUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMSCITEM::PacketData, UniqueNo);
constexpr auto gmSCItemSNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMSCITEM::PacketData, sName);
constexpr auto gmSCItemTableIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_GMSCITEM::PacketData, TableIndex);
constexpr auto gmSCItemPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_GMSCITEM::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GM support packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GM support packet self-test failed: " << label << " got";
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

auto expectZeroRange(CBasicPacket& packet, std::size_t offset, std::size_t end, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < end; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c GM support packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    return expectZeroRange(packet, offset, PACKET_SIZE, label);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_SET_GMMSG::PacketData), 252, "SET_GMMSG sizeof(PacketData)") && ok;
    ok = expectEqualUInt(setGMMsgPacketSize, 256, "SET_GMMSG packet size") && ok;
    ok = expectEqualUInt(setGMMsgMsgIDOffset, 4, "SET_GMMSG msgId offset") && ok;
    ok = expectEqualUInt(setGMMsgSeqIDOffset, 8, "SET_GMMSG seqId offset") && ok;
    ok = expectEqualUInt(setGMMsgPktNumOffset, 10, "SET_GMMSG pktNum offset") && ok;
    ok = expectEqualUInt(setGMMsgMsgOffset, 12, "SET_GMMSG Msg offset") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_SET_GMMSG::PacketData::Msg), 244, "SET_GMMSG Msg size") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_COMMAND_GMSCITEM::PacketData), 88, "GMSCITEM sizeof(PacketData)") && ok;
    ok = expectEqualUInt(gmSCItemPacketSize, 92, "GMSCITEM packet size") && ok;
    ok = expectEqualUInt(gmSCItemItemFlagOffset, 4, "GMSCITEM ItemFlag offset") && ok;
    ok = expectEqualUInt(gmSCItemUniqueNoOffset, 68, "GMSCITEM UniqueNo offset") && ok;
    ok = expectEqualUInt(gmSCItemSNameOffset, 72, "GMSCITEM sName offset") && ok;
    ok = expectEqualUInt(gmSCItemTableIndexOffset, 88, "GMSCITEM TableIndex offset") && ok;
    return ok;
}

auto testSetGMMsgConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_SET_GMMSG(0x11223344, 0x5566, 0x7788, std::string("GM reply"));
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 20>{
        0xB6, 0x80, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x66, 0x55, 0x88, 0x77,
        'G', 'M', ' ', 'r', 'e', 'p', 'l', 'y',
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0B6, "SET_GMMSG type") && ok;
    ok      = expectEqualUInt(packet.getSize(), setGMMsgPacketSize, "SET_GMMSG size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded SET_GMMSG prefix") && ok;
    ok      = expectZeroRange(packet, setGMMsgMsgOffset + 8, setGMMsgPacketSize, "SET_GMMSG short message padding") && ok;
    ok      = expectZeroTail(packet, setGMMsgPacketSize, "SET_GMMSG tail") && ok;
    return ok;
}

auto testSetGMMsgRawCopy() -> bool
{
    auto nulPacket = GP_SERV_COMMAND_SET_GMMSG(1, 2, 3, std::string("ab\0cd", 5));

    auto longMessage = std::string(300, 'x');
    longMessage[243] = 'z';
    longMessage[244] = 'q';
    auto truncPacket = GP_SERV_COMMAND_SET_GMMSG(4, 5, 6, longMessage);

    bool ok = true;
    ok      = expectBytes(nulPacket, setGMMsgMsgOffset, std::array<uint8, 5>{ 'a', 'b', 0x00, 'c', 'd' }, "SET_GMMSG embedded NUL copy") && ok;
    ok      = expectEqualUInt(packetData(truncPacket)[setGMMsgMsgOffset + 242], static_cast<uint8>('x'), "SET_GMMSG trunc byte 242") && ok;
    ok      = expectEqualUInt(packetData(truncPacket)[setGMMsgMsgOffset + 243], static_cast<uint8>('z'), "SET_GMMSG trunc byte 243") && ok;
    ok      = expectZeroTail(truncPacket, setGMMsgPacketSize, "SET_GMMSG trunc tail") && ok;
    return ok;
}

auto testGMSCItemConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_GMSCITEM();
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 12>{
        0xB7, 0x2E, 0xEF, 0xBE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0B7, "GMSCITEM type") && ok;
    ok      = expectEqualUInt(packet.getSize(), gmSCItemPacketSize, "GMSCITEM size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "encoded GMSCITEM prefix") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), gmSCItemPacketSize, "GMSCITEM payload") && ok;
    ok      = expectZeroTail(packet, gmSCItemPacketSize, "GMSCITEM tail") && ok;
    return ok;
}

} // namespace

auto runS2CGMSupportPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testSetGMMsgConstructor() && ok;
    ok      = testSetGMMsgRawCopy() && ok;
    ok      = testGMSCItemConstructor() && ok;
    return ok;
}
