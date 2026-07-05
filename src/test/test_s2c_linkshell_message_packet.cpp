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

#include "test_s2c_linkshell_message_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0cc_linkshell_message.h"

namespace
{

using LinkshellMessagePacket = GP_SERV_COMMAND_LINKSHELL_MESSAGE;

constexpr auto linkshellMessageFlags0Offset       = sizeof(GP_SERV_HEADER);
constexpr auto linkshellMessageFlags1Offset       = linkshellMessageFlags0Offset + 1;
constexpr auto linkshellMessageSeqIDOffset        = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, seqId);
constexpr auto linkshellMessageTextOffset         = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, sMessage);
constexpr auto linkshellMessageTextSize           = sizeof(LinkshellMessagePacket::PacketData{}.sMessage);
constexpr auto linkshellMessageUpdateTimeOffset   = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, updateTime);
constexpr auto linkshellMessageModifierOffset     = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, modifier);
constexpr auto linkshellMessageModifierSize       = sizeof(LinkshellMessagePacket::PacketData{}.modifier);
constexpr auto linkshellMessageOpTypeOffset       = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, opType);
constexpr auto linkshellMessagePadding9EOffset    = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, padding9E);
constexpr auto linkshellMessageEncodedNameOffset  = sizeof(GP_SERV_HEADER) + offsetof(LinkshellMessagePacket::PacketData, encodedLsName);
constexpr auto linkshellMessageEncodedNameSize    = sizeof(LinkshellMessagePacket::PacketData{}.encodedLsName);
constexpr auto linkshellMessagePacketDataSize     = sizeof(LinkshellMessagePacket::PacketData);
constexpr auto linkshellMessagePacketSize         = sizeof(GP_SERV_HEADER) + linkshellMessagePacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c LINKSHELL_MESSAGE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c LINKSHELL_MESSAGE packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_LINKSHELL_MESSAGE), 0x0CC, "LINKSHELL_MESSAGE packet id") && ok;
    ok      = expectEqualUInt(linkshellMessageTextSize, 128, "sMessage size") && ok;
    ok      = expectEqualUInt(linkshellMessageModifierSize, 16, "modifier size") && ok;
    ok      = expectEqualUInt(linkshellMessageEncodedNameSize, 16, "encodedLsName size") && ok;
    ok      = expectEqualUInt(linkshellMessagePacketDataSize, 172, "sizeof(LINKSHELL_MESSAGE::PacketData)") && ok;
    ok      = expectEqualUInt(linkshellMessagePacketSize, 176, "LINKSHELL_MESSAGE packet size") && ok;
    ok      = expectEqualUInt(linkshellMessageFlags0Offset, 4, "flags byte 0 offset") && ok;
    ok      = expectEqualUInt(linkshellMessageFlags1Offset, 5, "flags byte 1 offset") && ok;
    ok      = expectEqualUInt(linkshellMessageSeqIDOffset, 6, "seqId offset") && ok;
    ok      = expectEqualUInt(linkshellMessageTextOffset, 8, "sMessage offset") && ok;
    ok      = expectEqualUInt(linkshellMessageUpdateTimeOffset, 136, "updateTime offset") && ok;
    ok      = expectEqualUInt(linkshellMessageModifierOffset, 140, "modifier offset") && ok;
    ok      = expectEqualUInt(linkshellMessageOpTypeOffset, 156, "opType offset") && ok;
    ok      = expectEqualUInt(linkshellMessagePadding9EOffset, 158, "padding9E offset") && ok;
    ok      = expectEqualUInt(linkshellMessageEncodedNameOffset, 160, "encodedLsName offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data              = LinkshellMessagePacket::PacketData{};
    data.stat              = 0x5;
    data.attr              = 0xA;
    data.readLevel         = 0x1;
    data.writeLevel        = 0x2;
    data.pubEditLevel      = 0x3;
    data.linkshell_index   = 0x1;
    data.seqId             = 0x1122;
    data.sMessage[0]       = 0x33;
    data.sMessage[127]     = 0x44;
    data.updateTime        = 0x55667788;
    data.modifier[0]       = 0x99;
    data.modifier[15]      = 0xAA;
    data.opType            = 0xBBCC;
    data.padding9E         = 0xDDEE;
    data.encodedLsName[0]  = 0x12;
    data.encodedLsName[15] = 0x34;

    auto expected = std::array<uint8, 172>{};
    expected[0]   = 0xA5;
    expected[1]   = 0x79;
    expected[2]   = 0x22;
    expected[3]   = 0x11;
    expected[4]   = 0x33;
    expected[131] = 0x44;
    expected[132] = 0x88;
    expected[133] = 0x77;
    expected[134] = 0x66;
    expected[135] = 0x55;
    expected[136] = 0x99;
    expected[151] = 0xAA;
    expected[152] = 0xCC;
    expected[153] = 0xBB;
    expected[154] = 0xEE;
    expected[155] = 0xDD;
    expected[156] = 0x12;
    expected[171] = 0x34;

    return expectStructBytes(data, expected, "LINKSHELL_MESSAGE PacketData bytes");
}

auto testConstructorBytes() -> bool
{
    auto ls1 = LinkshellMessagePacket("PosterName", "Linkshell message", "Linkshell", 0x11223344, LinkshellSlot::LS1);
    auto ls2 = LinkshellMessagePacket("PosterName", "Linkshell message", "Linkshell", 0x11223344, LinkshellSlot::LS2);

    const auto* ls1Data = packetData(ls1) + sizeof(GP_SERV_HEADER);
    const auto* ls2Data = packetData(ls2) + sizeof(GP_SERV_HEADER);

    bool ok = true;
    ok      = expectEqualUInt(ls1Data[0], 0x70, "constructor LS1 stat/attr byte") && ok;
    ok      = expectEqualUInt(ls1Data[1], 0x05, "constructor LS1 level/index byte") && ok;
    ok      = expectEqualUInt(ls2Data[1], 0x45, "constructor LS2 level/index byte") && ok;
    ok      = expectEqualUInt(ls1Data[4], static_cast<uint8>('L'), "constructor message first byte") && ok;
    ok      = expectEqualUInt(ls1Data[132], 0x44, "constructor updateTime byte 0") && ok;
    ok      = expectEqualUInt(ls1Data[135], 0x11, "constructor updateTime byte 3") && ok;
    ok      = expectEqualUInt(ls1Data[136], static_cast<uint8>('P'), "constructor modifier first byte") && ok;
    ok      = expectEqualUInt(ls1Data[152], 0x02, "constructor opType byte 0") && ok;
    ok      = expectEqualUInt(ls1Data[153], 0x00, "constructor opType byte 1") && ok;
    ok      = expectEqualUInt(ls1Data[156], static_cast<uint8>('L'), "constructor encodedLsName first byte") && ok;
    return ok;
}

} // namespace

auto runS2CLinkshellMessagePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testConstructorBytes() && ok;
    return ok;
}
