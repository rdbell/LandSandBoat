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

#include "test_s2c_weather_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/enums/weather.h"
#include "map/packets/s2c/0x057_weather.h"

namespace
{

constexpr auto weatherStartTimeOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WEATHER::PacketData, StartTime);
constexpr auto weatherNumberOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WEATHER::PacketData, WeatherNumber);
constexpr auto weatherOffsetTimeOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_WEATHER::PacketData, WeatherOffsetTime);
constexpr auto weatherPacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_WEATHER::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c WEATHER packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(CBasicPacket& packet, const std::array<uint8, 12>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c WEATHER packet self-test failed: " << label << " got";
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
            std::cerr << "s2c WEATHER packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_WEATHER::PacketData), 8, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(weatherPacketDefaultSize, 12, "packet default size") && ok;
    ok      = expectEqualUInt(weatherStartTimeOffset, 4, "StartTime offset") && ok;
    ok      = expectEqualUInt(weatherNumberOffset, 8, "WeatherNumber offset") && ok;
    ok      = expectEqualUInt(weatherOffsetTimeOffset, 10, "WeatherOffsetTime offset") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(Weather::Blizzards), 13, "Weather::Blizzards") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_WEATHER(0x11223344, Weather::Blizzards, 0x5566);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 12>{
        0x57, 0x06, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x0D, 0x00, 0x66, 0x55,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x057, "WEATHER type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "WEATHER size") && ok;
    ok      = expectBytes(packet, expected, "encoded WEATHER prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "WEATHER tail") && ok;
    return ok;
}

} // namespace

auto runS2CWeatherPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
