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

#include "test_s2c_eventucoff_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "event_info.h"
#include "entities/char_entity.h"
#include "map/packets/s2c/0x052_eventucoff.h"

namespace
{

constexpr auto eventUCOffModeOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_EVENTUCOFF::PacketData, Mode);
constexpr auto eventUCOffPacketSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_EVENTUCOFF::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto readU32(CBasicPacket& packet, std::size_t offset) -> std::uint32_t
{
    std::uint32_t value = 0;
    std::memcpy(&value, packetData(packet) + offset, sizeof(value));
    return value;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EVENTUCOFF packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EVENTUCOFF packet self-test failed: " << label << " got";
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
            std::cerr << "s2c EVENTUCOFF packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_EVENTUCOFF::PacketData), 4, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(eventUCOffPacketSize, 8, "packet size") && ok;
    ok      = expectEqualUInt(eventUCOffModeOffset, 4, "Mode offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_EVENTUCOFF_MODE::Standard), 0, "Standard mode") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_EVENTUCOFF_MODE::EventRecvPending), 1, "EventRecvPending mode") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent), 2, "CancelEvent mode") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelInput), 3, "CancelInput mode") && ok;
    ok      = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_EVENTUCOFF_MODE::Fishing), 4, "Fishing mode") && ok;
    return ok;
}

auto testStandardConstructor() -> bool
{
    auto character      = CCharEntity{};
    character.m_Substate = CHAR_SUBSTATE::SUBSTATE_IN_CS;
    auto packet         = GP_SERV_COMMAND_EVENTUCOFF(&character, GP_SERV_COMMAND_EVENTUCOFF_MODE::Standard);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x052, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), eventUCOffPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x52, 0x04, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, eventUCOffModeOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "standard Mode") && ok;
    ok      = expectZeroRange(packet, eventUCOffPacketSize, PACKET_SIZE, "tail") && ok;
    ok      = expectEqualUInt(character.m_Substate, CHAR_SUBSTATE::SUBSTATE_NONE, "standard substate") && ok;
    return ok;
}

auto testCancelEventPacksEventId() -> bool
{
    auto character                 = CCharEntity{};
    character.m_Substate           = CHAR_SUBSTATE::SUBSTATE_IN_CS;
    character.currentEvent->eventId = 0x1234;
    auto packet                    = GP_SERV_COMMAND_EVENTUCOFF(&character, GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelEvent);

    bool ok = true;
    ok      = expectEqualUInt(readU32(packet, eventUCOffModeOffset), 0x00123402, "cancel event packed mode") && ok;
    ok      = expectEqualUInt(character.m_Substate, CHAR_SUBSTATE::SUBSTATE_NONE, "cancel event substate") && ok;
    return ok;
}

} // namespace

auto runS2CEventUCOffPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testStandardConstructor() && ok;
    ok      = testCancelEventPacksEventId() && ok;
    return ok;
}
