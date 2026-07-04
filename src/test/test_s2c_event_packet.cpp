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

#include "test_s2c_event_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "entities/char_entity.h"
#include "event_info.h"
#include "map/packets/s2c/0x032_event.h"

namespace
{

constexpr auto eventUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, UniqueNo);
constexpr auto eventActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, ActIndex);
constexpr auto eventEventNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, EventNum);
constexpr auto eventEventParaOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, EventPara);
constexpr auto eventModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, Mode);
constexpr auto eventEventNum2Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, EventNum2);
constexpr auto eventEventPara2Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENT::PacketData, EventPara2);
constexpr auto eventPacketSize       = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EVENT::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto makeChar(std::uint32_t id, std::uint16_t targid, std::uint16_t zone) -> CCharEntity
{
    auto character             = CCharEntity{};
    character.id               = id;
    character.targid           = targid;
    character.loc.destination  = zone;
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
        std::cerr << "s2c EVENT packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EVENT packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EVENT packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENT::PacketData), 16, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(eventPacketSize, 20, "packet size") && ok;
    ok      = expectEqualUInt(eventUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(eventActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(eventEventNumOffset, 10, "EventNum offset") && ok;
    ok      = expectEqualUInt(eventEventParaOffset, 12, "EventPara offset") && ok;
    ok      = expectEqualUInt(eventModeOffset, 14, "Mode offset") && ok;
    ok      = expectEqualUInt(eventEventNum2Offset, 16, "EventNum2 offset") && ok;
    ok      = expectEqualUInt(eventEventPara2Offset, 18, "EventPara2 offset") && ok;
    return ok;
}

auto testTargetEntityConstructor() -> bool
{
    auto      character = makeChar(0x11223344, 0x5566, 0x1234);
    auto      target    = makeEntity(0xAABBCCDD, 0xEEFF);
    EventInfo eventInfo{};
    eventInfo.targetEntity = &target;
    eventInfo.eventId      = 0x789A;
    eventInfo.eventFlags   = 0xBEEFCAFE;

    auto packet = GP_SERV_COMMAND_EVENT(&character, &eventInfo);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x032, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), eventPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x32, 0x0A, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, eventUniqueNoOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "UniqueNo") && ok;
    ok      = expectBytes(packet, eventActIndexOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "ActIndex") && ok;
    ok      = expectBytes(packet, eventEventNumOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "EventNum") && ok;
    ok      = expectBytes(packet, eventEventParaOffset, std::array<uint8, 2>{ 0x9A, 0x78 }, "EventPara") && ok;
    ok      = expectBytes(packet, eventModeOffset, std::array<uint8, 2>{ 0xFE, 0xCA }, "Mode") && ok;
    ok      = expectBytes(packet, eventEventNum2Offset, std::array<uint8, 2>{ 0x34, 0x12 }, "EventNum2") && ok;
    ok      = expectBytes(packet, eventEventPara2Offset, std::array<uint8, 2>{ 0xEF, 0xBE }, "EventPara2") && ok;
    ok      = expectZeroRange(packet, eventPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testPlayerFallbackConstructor() -> bool
{
    auto      character = makeChar(0x01020304, 0x0506, 0x0708);
    EventInfo eventInfo{};
    eventInfo.eventId    = 0x12349001;
    eventInfo.eventFlags = 0;

    auto packet = GP_SERV_COMMAND_EVENT(&character, &eventInfo);

    bool ok = true;
    ok      = expectBytes(packet, eventUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "fallback UniqueNo") && ok;
    ok      = expectBytes(packet, eventActIndexOffset, std::array<uint8, 2>{ 0x06, 0x05 }, "fallback ActIndex") && ok;
    ok      = expectBytes(packet, eventEventNumOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "fallback EventNum") && ok;
    ok      = expectBytes(packet, eventEventParaOffset, std::array<uint8, 2>{ 0x01, 0x90 }, "fallback EventPara") && ok;
    ok      = expectBytes(packet, eventModeOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "fallback Mode") && ok;
    ok      = expectBytes(packet, eventEventNum2Offset, std::array<uint8, 2>{ 0x08, 0x07 }, "fallback EventNum2") && ok;
    ok      = expectBytes(packet, eventEventPara2Offset, std::array<uint8, 2>{ 0x00, 0x00 }, "fallback EventPara2") && ok;
    return ok;
}

} // namespace

auto runS2CEventPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testTargetEntityConstructor() && ok;
    ok      = testPlayerFallbackConstructor() && ok;
    return ok;
}
