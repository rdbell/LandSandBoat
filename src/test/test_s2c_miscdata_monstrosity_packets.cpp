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

#include "test_s2c_miscdata_monstrosity_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x063_miscdata_monstrosity.h"

namespace
{

using Monstrosity1Packet = GP_SERV_COMMAND_MISCDATA::MONSTROSITY1;
using Monstrosity2Packet = GP_SERV_COMMAND_MISCDATA::MONSTROSITY2;

constexpr auto monstrosity1PacketDataSize      = sizeof(Monstrosity1Packet::PacketData);
constexpr auto monstrosity1PacketSize          = sizeof(GP_SERV_HEADER) + monstrosity1PacketDataSize;
constexpr auto monstrosity1TypeOffset          = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, type);
constexpr auto monstrosity1Unknown06Offset     = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, unknown06);
constexpr auto monstrosity1SpeciesOffset       = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, species);
constexpr auto monstrosity1FlagsOffset         = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, flags);
constexpr auto monstrosity1RankOffset          = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, rank);
constexpr auto monstrosity1Unknown1Offset      = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, unknown1);
constexpr auto monstrosity1InfamyOffset        = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, infamy);
constexpr auto monstrosity1Unknown2Offset      = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, unknown2);
constexpr auto monstrosity1InstinctsOffset     = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, instincts);
constexpr auto monstrosity1LevelsOffset        = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity1Packet::PacketData, levels);

constexpr auto monstrosity2PacketDataSize      = sizeof(Monstrosity2Packet::PacketData);
constexpr auto monstrosity2PacketSize          = sizeof(GP_SERV_HEADER) + monstrosity2PacketDataSize;
constexpr auto monstrosity2TypeOffset          = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, type);
constexpr auto monstrosity2Unknown06Offset     = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, unknown06);
constexpr auto monstrosity2UnknownOffset       = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, unknown);
constexpr auto monstrosity2SlimeLevelOffset    = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, slimeLevel);
constexpr auto monstrosity2SprigganLevelOffset = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, sprigganLevel);
constexpr auto monstrosity2InstinctsOffset     = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, instincts2);
constexpr auto monstrosity2PaddingOffset       = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, padding);
constexpr auto monstrosity2VariantsOffset      = sizeof(GP_SERV_HEADER) + offsetof(Monstrosity2Packet::PacketData, variants);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA MONSTROSITY packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA MONSTROSITY packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISCDATA), 0x063, "MISCDATA packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity1), 0x03, "Monstrosity1 miscdata type") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity2), 0x04, "Monstrosity2 miscdata type") && ok;
    ok      = expectEqualUInt(monstrosity1PacketDataSize, 216, "MONSTROSITY1 sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(monstrosity1PacketSize, 220, "MONSTROSITY1 packet size") && ok;
    ok      = expectEqualUInt(monstrosity1PacketDataSize - 8, 208, "MONSTROSITY1 unknown06 marker") && ok;
    ok      = expectEqualUInt(monstrosity1TypeOffset, 4, "MONSTROSITY1 type offset") && ok;
    ok      = expectEqualUInt(monstrosity1Unknown06Offset, 6, "MONSTROSITY1 unknown06 offset") && ok;
    ok      = expectEqualUInt(monstrosity1SpeciesOffset, 8, "MONSTROSITY1 species offset") && ok;
    ok      = expectEqualUInt(monstrosity1FlagsOffset, 10, "MONSTROSITY1 flags offset") && ok;
    ok      = expectEqualUInt(monstrosity1RankOffset, 12, "MONSTROSITY1 rank offset") && ok;
    ok      = expectEqualUInt(monstrosity1Unknown1Offset, 16, "MONSTROSITY1 unknown1 offset") && ok;
    ok      = expectEqualUInt(monstrosity1InfamyOffset, 18, "MONSTROSITY1 infamy offset") && ok;
    ok      = expectEqualUInt(monstrosity1Unknown2Offset, 20, "MONSTROSITY1 unknown2 offset") && ok;
    ok      = expectEqualUInt(monstrosity1InstinctsOffset, 28, "MONSTROSITY1 instincts offset") && ok;
    ok      = expectEqualUInt(monstrosity1LevelsOffset, 92, "MONSTROSITY1 levels offset") && ok;
    ok      = expectEqualUInt(sizeof(Monstrosity1Packet::PacketData::instincts), 64, "MONSTROSITY1 instincts size") && ok;
    ok      = expectEqualUInt(sizeof(Monstrosity1Packet::PacketData::levels), 128, "MONSTROSITY1 levels size") && ok;

    ok = expectEqualUInt(monstrosity2PacketDataSize, 176, "MONSTROSITY2 sizeof(PacketData)") && ok;
    ok = expectEqualUInt(monstrosity2PacketSize, 180, "MONSTROSITY2 packet size") && ok;
    ok = expectEqualUInt(monstrosity2PacketDataSize - 8, 168, "MONSTROSITY2 unknown06 marker") && ok;
    ok = expectEqualUInt(monstrosity2TypeOffset, 4, "MONSTROSITY2 type offset") && ok;
    ok = expectEqualUInt(monstrosity2Unknown06Offset, 6, "MONSTROSITY2 unknown06 offset") && ok;
    ok = expectEqualUInt(monstrosity2UnknownOffset, 8, "MONSTROSITY2 unknown offset") && ok;
    ok = expectEqualUInt(monstrosity2SlimeLevelOffset, 134, "MONSTROSITY2 slimeLevel offset") && ok;
    ok = expectEqualUInt(monstrosity2SprigganLevelOffset, 135, "MONSTROSITY2 sprigganLevel offset") && ok;
    ok = expectEqualUInt(monstrosity2InstinctsOffset, 136, "MONSTROSITY2 instincts2 offset") && ok;
    ok = expectEqualUInt(monstrosity2PaddingOffset, 140, "MONSTROSITY2 padding offset") && ok;
    ok = expectEqualUInt(monstrosity2VariantsOffset, 148, "MONSTROSITY2 variants offset") && ok;
    ok = expectEqualUInt(sizeof(Monstrosity2Packet::PacketData::unknown), 126, "MONSTROSITY2 unknown size") && ok;
    ok = expectEqualUInt(sizeof(Monstrosity2Packet::PacketData::instincts2), 4, "MONSTROSITY2 instincts2 size") && ok;
    ok = expectEqualUInt(sizeof(Monstrosity2Packet::PacketData::padding), 8, "MONSTROSITY2 padding size") && ok;
    ok = expectEqualUInt(sizeof(Monstrosity2Packet::PacketData::variants), 32, "MONSTROSITY2 variants size") && ok;
    return ok;
}

auto testMonstrosity1PacketDataBytes() -> bool
{
    auto data = Monstrosity1Packet::PacketData{};

    data.type        = GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity1;
    data.unknown06   = monstrosity1PacketDataSize - 8;
    data.species     = 0x0102;
    data.flags       = 0x0304;
    data.rank        = 0x05;
    data.unknown1[0] = 0xEC;
    data.infamy      = 0x1112;
    data.unknown2    = 0x2C;
    data.instincts[0]  = 0x21;
    data.instincts[63] = 0x63;
    data.levels[0]     = 0x31;
    data.levels[127]   = 0x7F;

    auto expected = std::array<uint8, monstrosity1PacketDataSize>{};
    putLE16(expected, 0, 0x03);
    putLE16(expected, 2, monstrosity1PacketDataSize - 8);
    putLE16(expected, 4, 0x0102);
    putLE16(expected, 6, 0x0304);
    expected[8]  = 0x05;
    expected[12] = 0xEC;
    putLE16(expected, 14, 0x1112);
    expected[16]  = 0x2C;
    expected[24]  = 0x21;
    expected[87]  = 0x63;
    expected[88]  = 0x31;
    expected[215] = 0x7F;

    return expectStructBytes(data, expected, "MONSTROSITY1 PacketData bytes");
}

auto testMonstrosity2PacketDataBytes() -> bool
{
    auto data = Monstrosity2Packet::PacketData{};

    data.type          = GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity2;
    data.unknown06     = monstrosity2PacketDataSize - 8;
    data.unknown[0]    = 0x11;
    data.unknown[125]  = 0x12;
    data.slimeLevel    = 0x31;
    data.sprigganLevel = 0x32;
    data.instincts2[0] = 0x21;
    data.instincts2[3] = 0x24;
    data.padding[7]    = 0x77;
    data.variants[0]   = 0x41;
    data.variants[31]  = 0x5F;

    auto expected = std::array<uint8, monstrosity2PacketDataSize>{};
    putLE16(expected, 0, 0x04);
    putLE16(expected, 2, monstrosity2PacketDataSize - 8);
    expected[4]   = 0x11;
    expected[129] = 0x12;
    expected[130] = 0x31;
    expected[131] = 0x32;
    expected[132] = 0x21;
    expected[135] = 0x24;
    expected[143] = 0x77;
    expected[144] = 0x41;
    expected[175] = 0x5F;

    return expectStructBytes(data, expected, "MONSTROSITY2 PacketData bytes");
}

} // namespace

auto runS2CMiscDataMonstrosityPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testMonstrosity1PacketDataBytes() && ok;
    ok      = testMonstrosity2PacketDataBytes() && ok;
    return ok;
}
