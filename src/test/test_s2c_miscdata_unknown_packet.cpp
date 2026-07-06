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

#include "test_s2c_miscdata_unknown_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x063_miscdata_unknown.h"

namespace
{

using MiscDataUnknownPacket = GP_SERV_COMMAND_MISCDATA::UNKNOWN;

constexpr auto miscDataUnknownTypeOffset      = sizeof(GP_SERV_HEADER) + offsetof(MiscDataUnknownPacket::PacketData, type);
constexpr auto miscDataUnknownUnknown06Offset = sizeof(GP_SERV_HEADER) + offsetof(MiscDataUnknownPacket::PacketData, unknown06);
constexpr auto miscDataUnknownDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(MiscDataUnknownPacket::PacketData, data);
constexpr auto miscDataUnknownPacketDataSize  = sizeof(MiscDataUnknownPacket::PacketData);
constexpr auto miscDataUnknownPacketSize      = sizeof(GP_SERV_HEADER) + miscDataUnknownPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA UNKNOWN packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA UNKNOWN packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MISCDATA UNKNOWN packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISCDATA), 0x063, "MISCDATA packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Unknown), 0x0A, "UNKNOWN type") && ok;
    ok      = expectEqualUInt(miscDataUnknownPacketDataSize, 36, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(sizeof(MiscDataUnknownPacket::PacketData::data), 32, "data size") && ok;
    ok      = expectEqualUInt(miscDataUnknownPacketSize, 40, "packet size") && ok;
    ok      = expectEqualUInt(miscDataUnknownTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(miscDataUnknownUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(miscDataUnknownDataOffset, 8, "data offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = MiscDataUnknownPacket(nullptr);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x063, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), miscDataUnknownPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 8>{ 0x63, 0x14, 0xEF, 0xBE, 0x0A, 0x00, 0x24, 0x00 }, "encoded header and fields") && ok;
    ok      = expectZeroRange(packet, miscDataUnknownDataOffset, miscDataUnknownPacketSize, "data") && ok;
    ok      = expectZeroRange(packet, miscDataUnknownPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

} // namespace

auto runS2CMiscDataUnknownPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    return ok;
}
