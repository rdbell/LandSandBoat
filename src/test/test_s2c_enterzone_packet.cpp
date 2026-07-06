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

#include "test_s2c_enterzone_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x008_enterzone.h"

namespace
{

constexpr auto enterZoneTableOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ENTERZONE::PacketData, EnterZoneTbl);
constexpr auto enterZoneTableLen        = sizeof(GP_SERV_COMMAND_ENTERZONE::PacketData{}.EnterZoneTbl);
constexpr auto enterZonePacketDataSize  = sizeof(GP_SERV_COMMAND_ENTERZONE::PacketData);
constexpr auto enterZonePacketSize      = sizeof(GP_SERV_HEADER) + enterZonePacketDataSize;
constexpr auto characterZonesVisitedLen = sizeof(CCharEntity{}.m_ZonesVisitedList);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ENTERZONE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(const uint8* data, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c ENTERZONE packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ENTERZONE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto sampleEnterZoneTable() -> std::array<uint8, enterZoneTableLen>
{
    auto table = std::array<uint8, enterZoneTableLen>{};
    for (std::size_t i = 0; i < table.size(); ++i)
    {
        table[i] = static_cast<uint8>(0x20 + i);
    }
    return table;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_ENTERZONE), 0x008, "ENTERZONE packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(enterZoneTableLen, 48, "EnterZoneTbl length") && ok;
    ok      = expectEqualUInt(enterZonePacketDataSize, 48, "PacketData size") && ok;
    ok      = expectEqualUInt(enterZonePacketSize, 52, "packet size") && ok;
    ok      = expectEqualUInt(enterZoneTableOffset, 4, "EnterZoneTbl offset") && ok;
    ok      = expectEqualUInt(characterZonesVisitedLen, 38, "CCharEntity m_ZonesVisitedList length") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = GP_SERV_COMMAND_ENTERZONE::PacketData{};
    auto want = sampleEnterZoneTable();
    std::memcpy(data.EnterZoneTbl, want.data(), want.size());

    return expectBytes(data.EnterZoneTbl, want, "PacketData EnterZoneTbl bytes");
}

auto testConstructorCopiesCharacterVisitedZones() -> bool
{
    auto character = CCharEntity{};
    auto source    = sampleEnterZoneTable();
    std::memcpy(character.m_ZonesVisitedList, source.data(), characterZonesVisitedLen);

    auto packet = GP_SERV_COMMAND_ENTERZONE(&character);
    packet.setSequence(0xBEEF);

    auto expected = std::array<uint8, enterZonePacketSize>{
        0x08, 0x1A, 0xEF, 0xBE,
    };
    std::memcpy(expected.data() + sizeof(GP_SERV_HEADER), source.data(), characterZonesVisitedLen);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x008, "ENTERZONE type") && ok;
    ok      = expectEqualUInt(packet.getSize(), enterZonePacketSize, "ENTERZONE size") && ok;
    ok      = expectBytes(packetData(packet), expected, "encoded ENTERZONE packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "ENTERZONE tail") && ok;
    return ok;
}

} // namespace

auto runS2CEnterZonePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testConstructorCopiesCharacterVisitedZones() && ok;
    return ok;
}
