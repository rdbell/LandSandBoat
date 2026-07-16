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

#include "test_s2c_dig_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x02f_dig.h"

namespace
{

constexpr auto digTarUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_DIG::PacketData, TarUniqueNo);
constexpr auto digTarActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_DIG::PacketData, TarActIndex);
constexpr auto digFlagsOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_DIG::PacketData, Flags);
constexpr auto digPadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_DIG::PacketData, padding00);
constexpr auto digPacketSize        = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_DIG::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid)
{
    character.id     = id;
    character.targid = targid;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c DIG packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c DIG packet self-test failed: " << label << " got";
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
            std::cerr << "s2c DIG packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_DIG::PacketData), 8, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(digPacketSize, 12, "packet size") && ok;
    ok      = expectEqualUInt(digTarUniqueNoOffset, 4, "TarUniqueNo offset") && ok;
    ok      = expectEqualUInt(digTarActIndexOffset, 8, "TarActIndex offset") && ok;
    ok      = expectEqualUInt(digFlagsOffset, 10, "Flags offset") && ok;
    ok      = expectEqualUInt(digPadding00Offset, 11, "padding00 offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566);
    auto packet = GP_SERV_COMMAND_DIG(&character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x02F, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), digPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x2F, 0x06, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, digTarUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "TarUniqueNo") && ok;
    ok      = expectBytes(packet, digTarActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "TarActIndex") && ok;
    ok      = expectBytes(packet, digFlagsOffset, std::array<uint8, 1>{ 0x01 }, "Flags") && ok;
    ok      = expectBytes(packet, digPadding00Offset, std::array<uint8, 1>{ 0x00 }, "padding00") && ok;
    ok      = expectZeroRange(packet, digPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testRuntimePlan() -> bool
{
    const auto plan = dighelpers::PlanFor(0x11223344, 0x5566);

    bool ok = true;
    ok      = expectEqualUInt(plan.tarUniqueNo, 0x11223344, "plan character ID") && ok;
    ok      = expectEqualUInt(plan.tarActIndex, 0x5566, "plan targid") && ok;
    ok      = expectEqualUInt(plan.flags, 0x01, "plan fixed Flags") && ok;
    return ok;
}

} // namespace

auto runS2CDigPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testRuntimePlan() && ok;
    return ok;
}
