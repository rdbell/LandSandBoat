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

#include "test_s2c_eventnum_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/base_entity.h"
#include "entities/char_entity.h"
#include "event_info.h"
#include "map/packets/s2c/0x034_eventnum.h"

namespace
{

constexpr auto eventNumUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, UniqueNo);
constexpr auto eventNumNumOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, num);
constexpr auto eventNumActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, ActIndex);
constexpr auto eventNumEventNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, EventNum);
constexpr auto eventNumEventParaOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, EventPara);
constexpr auto eventNumModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, Mode);
constexpr auto eventNumEventNum2Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, EventNum2);
constexpr auto eventNumEventPara2Offset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTNUM::PacketData, EventPara2);
constexpr auto eventNumPacketSize       = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EVENTNUM::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid, std::uint16_t zone)
{
    character.id              = id;
    character.targid          = targid;
    character.loc.destination = zone;
}

void makeEntity(CBaseEntity& entity, std::uint32_t id, std::uint16_t targid)
{
    entity.id    = id;
    entity.targid = targid;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EVENTNUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EVENTNUM packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EVENTNUM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENTNUM::PacketData::num), 32, "num size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENTNUM::PacketData), 48, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(eventNumPacketSize, 52, "packet size") && ok;
    ok      = expectEqualUInt(eventNumUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(eventNumNumOffset, 8, "num offset") && ok;
    ok      = expectEqualUInt(eventNumActIndexOffset, 40, "ActIndex offset") && ok;
    ok      = expectEqualUInt(eventNumEventNumOffset, 42, "EventNum offset") && ok;
    ok      = expectEqualUInt(eventNumEventParaOffset, 44, "EventPara offset") && ok;
    ok      = expectEqualUInt(eventNumModeOffset, 46, "Mode offset") && ok;
    ok      = expectEqualUInt(eventNumEventNum2Offset, 48, "EventNum2 offset") && ok;
    ok      = expectEqualUInt(eventNumEventPara2Offset, 50, "EventPara2 offset") && ok;
    return ok;
}

auto testTargetEntityConstructorWithFlagsAndTextTable() -> bool
{
    auto      character = CCharEntity{};
    auto      target    = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, 0x1234);
    makeEntity(target, 0xAABBCCDD, 0xEEFF);
    EventInfo eventInfo{};
    eventInfo.targetEntity = &target;
    eventInfo.eventId      = 0x1234789A;
    eventInfo.eventFlags   = 0xBEEFCAFE;
    eventInfo.textTable    = 0x4567;
    eventInfo.params[0]    = 0x01020304;
    eventInfo.params[3]    = 0xA0B0C0D0;
    eventInfo.params[7]    = 0x11223344;
    eventInfo.params[8]    = 0x55667788;

    auto packet = GP_SERV_COMMAND_EVENTNUM(&character, &eventInfo);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x034, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), eventNumPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x34, 0x1A, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, eventNumUniqueNoOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "UniqueNo") && ok;
    ok      = expectBytes(packet, eventNumNumOffset + 0*4, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "num[0]") && ok;
    ok      = expectBytes(packet, eventNumNumOffset + 1*4, std::array<uint8, 4>{ 0, 0, 0, 0 }, "num[1]") && ok;
    ok      = expectBytes(packet, eventNumNumOffset + 3*4, std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "num[3]") && ok;
    ok      = expectBytes(packet, eventNumNumOffset + 7*4, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "num[7]") && ok;
    ok      = expectBytes(packet, eventNumActIndexOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "ActIndex") && ok;
    ok      = expectBytes(packet, eventNumEventNumOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "EventNum") && ok;
    ok      = expectBytes(packet, eventNumEventParaOffset, std::array<uint8, 2>{ 0x9A, 0x78 }, "EventPara") && ok;
    ok      = expectBytes(packet, eventNumModeOffset, std::array<uint8, 2>{ 0xFE, 0xCA }, "Mode") && ok;
    ok      = expectBytes(packet, eventNumEventNum2Offset, std::array<uint8, 2>{ 0x67, 0x45 }, "EventNum2") && ok;
    ok      = expectBytes(packet, eventNumEventPara2Offset, std::array<uint8, 2>{ 0xEF, 0xBE }, "EventPara2") && ok;
    ok      = expectZeroRange(packet, eventNumPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testPlayerFallbackDefaultsModeAndZoneEventNum2() -> bool
{
    auto      character = CCharEntity{};
    makeChar(character, 0x01020304, 0x0506, 0x0708);
    EventInfo eventInfo{};
    eventInfo.eventId  = 0x12349001;
    eventInfo.params[2] = 0x0A0B0C0D;

    auto packet = GP_SERV_COMMAND_EVENTNUM(&character, &eventInfo);

    bool ok = true;
    ok      = expectBytes(packet, eventNumUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "fallback UniqueNo") && ok;
    ok      = expectBytes(packet, eventNumNumOffset + 2*4, std::array<uint8, 4>{ 0x0D, 0x0C, 0x0B, 0x0A }, "fallback num[2]") && ok;
    ok      = expectBytes(packet, eventNumActIndexOffset, std::array<uint8, 2>{ 0x06, 0x05 }, "fallback ActIndex") && ok;
    ok      = expectBytes(packet, eventNumEventNumOffset, std::array<uint8, 2>{ 0x08, 0x07 }, "fallback EventNum") && ok;
    ok      = expectBytes(packet, eventNumEventParaOffset, std::array<uint8, 2>{ 0x01, 0x90 }, "fallback EventPara") && ok;
    ok      = expectBytes(packet, eventNumModeOffset, std::array<uint8, 2>{ 0x08, 0x00 }, "fallback Mode") && ok;
    ok      = expectBytes(packet, eventNumEventNum2Offset, std::array<uint8, 2>{ 0x08, 0x07 }, "fallback EventNum2") && ok;
    ok      = expectBytes(packet, eventNumEventPara2Offset, std::array<uint8, 2>{ 0x00, 0x00 }, "fallback EventPara2") && ok;
    return ok;
}

} // namespace

auto runS2CEventNumPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testTargetEntityConstructorWithFlagsAndTextTable() && ok;
    ok      = testPlayerFallbackDefaultsModeAndZoneEventNum2() && ok;
    return ok;
}
