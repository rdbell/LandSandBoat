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

#include "test_s2c_pendingnum_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "map/packets/s2c/0x05c_pendingnum.h"

namespace
{

constexpr auto pendingNumNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_PENDINGNUM::PacketData, num);
constexpr auto pendingNumPacketSize  = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_PENDINGNUM::PacketData);
constexpr auto pendingNumElementSize = sizeof(GP_SERV_COMMAND_PENDINGNUM::PacketData::num[0]);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PENDINGNUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualInt(std::int64_t actual, std::int64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PENDINGNUM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c PENDINGNUM packet self-test failed: " << label << " got";
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
            std::cerr << "s2c PENDINGNUM packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PENDINGNUM::PacketData::num) / pendingNumElementSize, 8, "num element count") && ok;
    ok      = expectEqualUInt(pendingNumElementSize, 4, "num element size") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_PENDINGNUM::PacketData), 32, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(pendingNumPacketSize, 36, "packet size") && ok;
    ok      = expectEqualUInt(pendingNumNumOffset, 4, "num offset") && ok;
    return ok;
}

auto testConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGNUM({ { 0, 0x11223344 }, { 3, 0xAABBCCDD }, { 7, 0xFFFFFFFF } });
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x05C, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), pendingNumPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x5C, 0x12, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, pendingNumNumOffset + 0 * pendingNumElementSize, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "num[0]") && ok;
    ok      = expectZeroRange(packet, pendingNumNumOffset + 1 * pendingNumElementSize, pendingNumNumOffset + 3 * pendingNumElementSize, "num[1..2]") && ok;
    ok      = expectBytes(packet, pendingNumNumOffset + 3 * pendingNumElementSize, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "num[3]") && ok;
    ok      = expectZeroRange(packet, pendingNumNumOffset + 4 * pendingNumElementSize, pendingNumNumOffset + 7 * pendingNumElementSize, "num[4..6]") && ok;
    ok      = expectBytes(packet, pendingNumNumOffset + 7 * pendingNumElementSize, std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "num[7]") && ok;
    ok      = expectZeroRange(packet, pendingNumPacketSize, PACKET_SIZE, "tail") && ok;
    return ok;
}

auto testOutOfRangeIndexesIgnoredAndDuplicatesOverwrite() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGNUM({ { 8, 0x01020304 }, { 255, 0x05060708 }, { 2, 1 }, { 2, 9 } });

    bool ok = true;
    ok      = expectZeroRange(packet, pendingNumNumOffset, pendingNumNumOffset + 2 * pendingNumElementSize, "ignored prefix") && ok;
    ok      = expectBytes(packet, pendingNumNumOffset + 2 * pendingNumElementSize, std::array<uint8, 4>{ 0x09, 0x00, 0x00, 0x00 }, "duplicate num[2]") && ok;
    ok      = expectZeroRange(packet, pendingNumNumOffset + 3 * pendingNumElementSize, pendingNumPacketSize, "ignored suffix") && ok;
    return ok;
}

auto testEmptyConstructorZeroesPayload() -> bool
{
    auto packet = GP_SERV_COMMAND_PENDINGNUM({});

    bool ok = true;
    ok      = expectZeroRange(packet, pendingNumNumOffset, pendingNumPacketSize, "empty payload") && ok;
    ok      = expectZeroRange(packet, pendingNumPacketSize, PACKET_SIZE, "empty tail") && ok;
    return ok;
}

} // namespace

auto runS2CPendingNumPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructor() && ok;
    ok      = testOutOfRangeIndexesIgnoredAndDuplicatesOverwrite() && ok;
    ok      = testEmptyConstructorZeroesPayload() && ok;
    return ok;
}
