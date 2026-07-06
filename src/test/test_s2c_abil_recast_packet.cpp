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

#include "test_s2c_abil_recast_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x119_abil_recast.h"

namespace
{

using AbilRecastPacket = GP_SERV_COMMAND_ABIL_RECAST;

constexpr auto abilRecastTimerSize         = sizeof(recasttimer_t);
constexpr auto abilRecastPacketDataSize    = sizeof(AbilRecastPacket::PacketData);
constexpr auto abilRecastPacketSize        = sizeof(GP_SERV_HEADER) + abilRecastPacketDataSize;
constexpr auto abilRecastTimersOffset      = sizeof(GP_SERV_HEADER) + offsetof(AbilRecastPacket::PacketData, Timers);
constexpr auto abilRecastTimer0Offset      = abilRecastTimersOffset;
constexpr auto abilRecastTimer1Offset      = abilRecastTimersOffset + abilRecastTimerSize;
constexpr auto abilRecastTimer30Offset     = abilRecastTimersOffset + (30 * abilRecastTimerSize);
constexpr auto abilRecastMountRecastOffset = sizeof(GP_SERV_HEADER) + offsetof(AbilRecastPacket::PacketData, MountRecast);
constexpr auto abilRecastMountIDOffset     = sizeof(GP_SERV_HEADER) + offsetof(AbilRecastPacket::PacketData, MountRecastId);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ABIL_RECAST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ABIL_RECAST packet self-test failed: " << label << " got";
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

template <std::size_t Size>
void putLE16(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
}

template <std::size_t Size>
void putLE32(std::array<uint8, Size>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

template <std::size_t Size>
void putTimer(std::array<uint8, Size>& buffer, std::size_t timerIndex, std::uint16_t timer, std::uint8_t calc1, std::uint8_t timerId, std::uint16_t calc2, std::uint16_t padding06)
{
    const auto offset = timerIndex * abilRecastTimerSize;
    putLE16(buffer, offset, timer);
    buffer[offset + 2] = calc1;
    buffer[offset + 3] = timerId;
    putLE16(buffer, offset + 4, calc2);
    putLE16(buffer, offset + 6, padding06);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_ABIL_RECAST), 0x119, "ABIL_RECAST packet id") && ok;
    ok      = expectEqualUInt(abilRecastTimerSize, 8, "sizeof(recasttimer_t)") && ok;
    ok      = expectEqualUInt(abilRecastPacketDataSize, 256, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(abilRecastPacketSize, 260, "packet size") && ok;
    ok      = expectEqualUInt(abilRecastTimersOffset, 4, "Timers offset") && ok;
    ok      = expectEqualUInt(abilRecastTimer0Offset, 4, "Timers[0] offset") && ok;
    ok      = expectEqualUInt(abilRecastTimer1Offset, 12, "Timers[1] offset") && ok;
    ok      = expectEqualUInt(abilRecastTimer30Offset, 244, "Timers[30] offset") && ok;
    ok      = expectEqualUInt(abilRecastMountRecastOffset, 252, "MountRecast offset") && ok;
    ok      = expectEqualUInt(abilRecastMountIDOffset, 256, "MountRecastId offset") && ok;
    ok      = expectEqualUInt(sizeof(AbilRecastPacket::PacketData::Timers), 248, "Timers size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = AbilRecastPacket::PacketData{};

    data.Timers[0].Timer     = 0x0102;
    data.Timers[0].Calc1     = 0x03;
    data.Timers[0].TimerId   = 0x04;
    data.Timers[0].Calc2     = 0x0506;
    data.Timers[0].padding06 = 0x0708;

    data.Timers[1].Timer     = 0x1112;
    data.Timers[1].Calc1     = 0x13;
    data.Timers[1].TimerId   = 0x14;
    data.Timers[1].Calc2     = 0x1516;
    data.Timers[1].padding06 = 0x1718;

    data.Timers[30].Timer     = 0x3132;
    data.Timers[30].Calc1     = 0x33;
    data.Timers[30].TimerId   = 0x34;
    data.Timers[30].Calc2     = 0x3536;
    data.Timers[30].padding06 = 0x3738;

    data.MountRecast   = 0x41424344;
    data.MountRecastId = 0x51525354;

    auto expected = std::array<uint8, abilRecastPacketDataSize>{};
    putTimer(expected, 0, 0x0102, 0x03, 0x04, 0x0506, 0x0708);
    putTimer(expected, 1, 0x1112, 0x13, 0x14, 0x1516, 0x1718);
    putTimer(expected, 30, 0x3132, 0x33, 0x34, 0x3536, 0x3738);
    putLE32(expected, 248, 0x41424344);
    putLE32(expected, 252, 0x51525354);

    return expectStructBytes(data, expected, "ABIL_RECAST PacketData bytes");
}

} // namespace

auto runS2CAbilRecastPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
