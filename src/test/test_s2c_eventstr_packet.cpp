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

#include "test_s2c_eventstr_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "entities/base_entity.h"
#include "entities/char_entity.h"
#include "event_info.h"
#include "map/packets/s2c/0x033_eventstr.h"

namespace
{

constexpr auto eventStrUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, UniqueNo);
constexpr auto eventStrActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, ActIndex);
constexpr auto eventStrEventNumOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, EventNum);
constexpr auto eventStrEventParaOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, EventPara);
constexpr auto eventStrModeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, Mode);
constexpr auto eventStrStringOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, String);
constexpr auto eventStrDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTSTR::PacketData, Data);
constexpr auto eventStrPacketSize      = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData);
constexpr auto eventStrStringCount     = sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::String) / sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::String[0]);
constexpr auto eventStrStringLen       = sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::String[0]);
constexpr auto eventStrDataCount       = sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::Data) / sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::Data[0]);
constexpr auto eventStrDataElementSize = sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData::Data[0]);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto makeChar(std::uint32_t id, std::uint16_t targid, std::uint16_t zone) -> CCharEntity
{
    auto character            = CCharEntity{};
    character.id              = id;
    character.targid          = targid;
    character.loc.destination = zone;
    return character;
}

auto makeEntity(std::uint32_t id, std::uint16_t targid) -> CBaseEntity
{
    auto entity  = CBaseEntity{};
    entity.id    = id;
    entity.targid = targid;
    return entity;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EVENTSTR packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EVENTSTR packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EVENTSTR packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto stringBytes(std::string_view text) -> std::array<uint8, eventStrStringLen>
{
    auto bytes = std::array<uint8, eventStrStringLen>{};
    std::memcpy(bytes.data(), text.data(), std::min<std::size_t>(text.size(), bytes.size() - 1));
    return bytes;
}

auto stringOffset(std::size_t index) -> std::size_t
{
    return eventStrStringOffset + index * eventStrStringLen;
}

auto dataOffset(std::size_t index) -> std::size_t
{
    return eventStrDataOffset + index * eventStrDataElementSize;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(eventStrStringCount, 4, "string count") && ok;
    ok      = expectEqualUInt(eventStrStringLen, 16, "string element size") && ok;
    ok      = expectEqualUInt(eventStrDataCount, 8, "data count") && ok;
    ok      = expectEqualUInt(eventStrDataElementSize, 4, "data element size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENTSTR::PacketData), 108, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(eventStrPacketSize, 112, "packet size") && ok;
    ok      = expectEqualUInt(eventStrUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(eventStrActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(eventStrEventNumOffset, 10, "EventNum offset") && ok;
    ok      = expectEqualUInt(eventStrEventParaOffset, 12, "EventPara offset") && ok;
    ok      = expectEqualUInt(eventStrModeOffset, 14, "Mode offset") && ok;
    ok      = expectEqualUInt(eventStrStringOffset, 16, "String offset") && ok;
    ok      = expectEqualUInt(eventStrDataOffset, 80, "Data offset") && ok;
    return ok;
}

auto testTargetEntityConstructorWithFlagsStringsAndData() -> bool
{
    auto      character = makeChar(0x11223344, 0x5566, 0x1234);
    auto      target    = makeEntity(0xAABBCCDD, 0xEEFF);
    EventInfo eventInfo{};
    eventInfo.targetEntity = &target;
    eventInfo.eventId      = 0x1234789A;
    eventInfo.eventFlags   = 0xBEEFCAFE;
    eventInfo.strings[0]   = "one";
    eventInfo.strings[2]   = std::string("ab\0cd", 5);
    eventInfo.strings[3]   = "abcdefghijklmnop";
    eventInfo.strings[4]   = "ignored";
    eventInfo.params[0]    = 0x01020304;
    eventInfo.params[3]    = 0xA0B0C0D0;
    eventInfo.params[7]    = 0x11223344;
    eventInfo.params[8]    = 0x55667788;

    auto packet = GP_SERV_COMMAND_EVENTSTR(&character, &eventInfo);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x033, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), eventStrPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x33, 0x38, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, eventStrUniqueNoOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "UniqueNo") && ok;
    ok      = expectBytes(packet, eventStrActIndexOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "ActIndex") && ok;
    ok      = expectBytes(packet, eventStrEventNumOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "EventNum") && ok;
    ok      = expectBytes(packet, eventStrEventParaOffset, std::array<uint8, 2>{ 0x9A, 0x78 }, "EventPara") && ok;
    ok      = expectBytes(packet, eventStrModeOffset, std::array<uint8, 2>{ 0xFE, 0xCA }, "Mode") && ok;
    ok      = expectBytes(packet, stringOffset(0), stringBytes("one"), "String[0]") && ok;
    ok      = expectBytes(packet, stringOffset(1), stringBytes(""), "String[1]") && ok;
    ok      = expectBytes(packet, stringOffset(2), stringBytes(std::string_view("ab\0cd", 5)), "String[2]") && ok;
    ok      = expectBytes(packet, stringOffset(3), stringBytes("abcdefghijklmno"), "String[3]") && ok;
    ok      = expectBytes(packet, dataOffset(0), std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "Data[0]") && ok;
    ok      = expectBytes(packet, dataOffset(1), std::array<uint8, 4>{ 0, 0, 0, 0 }, "Data[1]") && ok;
    ok      = expectBytes(packet, dataOffset(3), std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "Data[3]") && ok;
    ok      = expectBytes(packet, dataOffset(7), std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "Data[7]") && ok;
    ok      = expectEqualUInt(packetData(packet)[stringOffset(3) + 15], 0, "String[3][15]") && ok;
    ok      = expectZeroRange(packet, eventStrPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testPlayerFallbackDefaultsModeAndZeroPayload() -> bool
{
    auto      character = makeChar(0x01020304, 0x0506, 0x0708);
    EventInfo eventInfo{};
    eventInfo.eventId    = 0x12349001;
    eventInfo.params[2]  = 0x0A0B0C0D;
    eventInfo.strings[1] = "two";

    auto packet = GP_SERV_COMMAND_EVENTSTR(&character, &eventInfo);

    bool ok = true;
    ok      = expectBytes(packet, eventStrUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "fallback UniqueNo") && ok;
    ok      = expectBytes(packet, eventStrActIndexOffset, std::array<uint8, 2>{ 0x06, 0x05 }, "fallback ActIndex") && ok;
    ok      = expectBytes(packet, eventStrEventNumOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "fallback EventNum") && ok;
    ok      = expectBytes(packet, eventStrEventParaOffset, std::array<uint8, 2>{ 0x01, 0x90 }, "fallback EventPara") && ok;
    ok      = expectBytes(packet, eventStrModeOffset, std::array<uint8, 2>{ 0x08, 0x00 }, "fallback Mode") && ok;
    ok      = expectBytes(packet, stringOffset(0), stringBytes(""), "fallback String[0]") && ok;
    ok      = expectBytes(packet, stringOffset(1), stringBytes("two"), "fallback String[1]") && ok;
    ok      = expectBytes(packet, dataOffset(2), std::array<uint8, 4>{ 0x0D, 0x0C, 0x0B, 0x0A }, "fallback Data[2]") && ok;
    return ok;
}

} // namespace

auto runS2CEventStrPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testTargetEntityConstructorWithFlagsStringsAndData() && ok;
    ok      = testPlayerFallbackDefaultsModeAndZeroPayload() && ok;
    return ok;
}
