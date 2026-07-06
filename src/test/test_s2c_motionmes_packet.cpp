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

#include "test_s2c_motionmes_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>

#include "map/packets/s2c/0x05a_motionmes.h"

namespace
{

using MotionMesPacket = GP_SERV_COMMAND_MOTIONMES;

constexpr auto motionMesPacketDataSize    = sizeof(MotionMesPacket::PacketData);
constexpr auto motionMesPacketSize        = sizeof(GP_SERV_HEADER) + motionMesPacketDataSize;
constexpr auto motionMesCasUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, CasUniqueNo);
constexpr auto motionMesTarUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, TarUniqueNo);
constexpr auto motionMesCasActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, CasActIndex);
constexpr auto motionMesTarActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, TarActIndex);
constexpr auto motionMesMesNumOffset      = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, MesNum);
constexpr auto motionMesParamOffset       = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, Param);
constexpr auto motionMesUnknown14Offset   = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, unknown14);
constexpr auto motionMesModeOffset        = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, Mode);
constexpr auto motionMesPadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, padding00);
constexpr auto motionMesFaithUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, FaithUniqueNo);
constexpr auto motionMesFaithActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, FaithActIndex);
constexpr auto motionMesPadding01Offset     = sizeof(GP_SERV_HEADER) + offsetof(MotionMesPacket::PacketData, padding01);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MOTIONMES packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MOTIONMES packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    static_assert(std::is_same_v<std::underlying_type_t<EmoteMode>, uint8>);

    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_MOTIONMES), 0x05A, "MOTIONMES packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(motionMesPacketDataSize, 52, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(motionMesPacketSize, 56, "packet size") && ok;
    ok      = expectEqualUInt(motionMesCasUniqueNoOffset, 4, "CasUniqueNo offset") && ok;
    ok      = expectEqualUInt(motionMesTarUniqueNoOffset, 8, "TarUniqueNo offset") && ok;
    ok      = expectEqualUInt(motionMesCasActIndexOffset, 12, "CasActIndex offset") && ok;
    ok      = expectEqualUInt(motionMesTarActIndexOffset, 14, "TarActIndex offset") && ok;
    ok      = expectEqualUInt(motionMesMesNumOffset, 16, "MesNum offset") && ok;
    ok      = expectEqualUInt(motionMesParamOffset, 18, "Param offset") && ok;
    ok      = expectEqualUInt(motionMesUnknown14Offset, 20, "unknown14 offset") && ok;
    ok      = expectEqualUInt(motionMesModeOffset, 22, "Mode offset") && ok;
    ok      = expectEqualUInt(motionMesPadding00Offset, 23, "padding00 offset") && ok;
    ok      = expectEqualUInt(motionMesFaithUniqueNoOffset, 24, "FaithUniqueNo offset") && ok;
    ok      = expectEqualUInt(motionMesFaithActIndexOffset, 44, "FaithActIndex offset") && ok;
    ok      = expectEqualUInt(motionMesPadding01Offset, 54, "padding01 offset") && ok;
    ok      = expectEqualUInt(sizeof(MotionMesPacket::PacketData{}.FaithUniqueNo), 20, "FaithUniqueNo size") && ok;
    ok      = expectEqualUInt(sizeof(MotionMesPacket::PacketData{}.FaithActIndex), 10, "FaithActIndex size") && ok;
    return ok;
}

auto testEmoteModeValues() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(EmoteMode::All), 0, "EmoteMode::All") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(EmoteMode::Text), 1, "EmoteMode::Text") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(EmoteMode::Motion), 2, "EmoteMode::Motion") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = MotionMesPacket::PacketData{};

    data.CasUniqueNo = 0x01020304;
    data.TarUniqueNo = 0x05060708;
    data.CasActIndex = 0x1112;
    data.TarActIndex = 0x1314;
    data.MesNum      = 0x1516;
    data.Param       = 0x1718;
    data.unknown14   = 0x191A;
    data.Mode        = EmoteMode::Motion;
    data.padding00   = 0x1B;
    for (std::size_t i = 0; i < 5; ++i)
    {
        data.FaithUniqueNo[i] = static_cast<std::uint32_t>(0x20212223 + i);
        data.FaithActIndex[i] = static_cast<std::uint16_t>(0x3132 + i);
    }
    data.padding01 = 0x4142;

    auto expected = std::array<uint8, motionMesPacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE32(expected, 4, 0x05060708);
    putLE16(expected, 8, 0x1112);
    putLE16(expected, 10, 0x1314);
    putLE16(expected, 12, 0x1516);
    putLE16(expected, 14, 0x1718);
    putLE16(expected, 16, 0x191A);
    expected[18] = 0x02;
    expected[19] = 0x1B;
    for (std::size_t i = 0; i < 5; ++i)
    {
        putLE32(expected, 20 + (i * 4), static_cast<std::uint32_t>(0x20212223 + i));
        putLE16(expected, 40 + (i * 2), static_cast<std::uint16_t>(0x3132 + i));
    }
    putLE16(expected, 50, 0x4142);

    return expectStructBytes(data, expected, "MOTIONMES PacketData bytes");
}

} // namespace

auto runS2CMotionMesPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testEmoteModeValues() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
