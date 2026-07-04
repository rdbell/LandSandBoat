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

#include "test_s2c_fragments_servmes_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x04d_fragments_servmes.h"

namespace
{

constexpr auto fragmentsServmesCommandOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, Command);
constexpr auto fragmentsServmesResultOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, Result);
constexpr auto fragmentsServmesValue1Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, value1);
constexpr auto fragmentsServmesValue2Offset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, value2);
constexpr auto fragmentsServmesTimestampOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, timestamp);
constexpr auto fragmentsServmesSizeTotalOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, size_total);
constexpr auto fragmentsServmesOffsetOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, offset);
constexpr auto fragmentsServmesDataSizeOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, data_size);
constexpr auto fragmentsServmesDataOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData, data);
constexpr auto fragmentsServmesPacketDataSize  = sizeof(GP_SERV_COMMAND_FRAGMENTS::SERVMES::PacketData);
constexpr auto fragmentsServmesFullPacketSize  = sizeof(GP_SERV_HEADER) + fragmentsServmesPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c FRAGMENTS SERVMES packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c FRAGMENTS SERVMES packet self-test failed: " << label << " got";
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

auto expectRepeatedByte(CBasicPacket& packet, std::size_t offset, std::size_t count, uint8 expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = 0; i < count; ++i)
    {
        if (data[offset + i] != expected)
        {
            std::cerr << "s2c FRAGMENTS SERVMES packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(fragmentsServmesPacketDataSize, 256, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(fragmentsServmesFullPacketSize, 260, "full packet size") && ok;
    ok      = expectEqualUInt(fragmentsServmesCommandOffset, 4, "Command offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesResultOffset, 5, "Result offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesValue1Offset, 6, "value1 offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesValue2Offset, 7, "value2 offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesTimestampOffset, 8, "timestamp offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesSizeTotalOffset, 12, "size_total offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesOffsetOffset, 16, "offset offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesDataSizeOffset, 20, "data_size offset") && ok;
    ok      = expectEqualUInt(fragmentsServmesDataOffset, 24, "data offset") && ok;
    return ok;
}

auto testFirstChunkConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FRAGMENTS::SERVMES("Hello", 2, 0x11223344, 0);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x04D, "first chunk type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 32, "first chunk size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4D, 0x10, 0xEF, 0xBE }, "first chunk header") && ok;
    ok      = expectBytes(packet, fragmentsServmesCommandOffset, std::array<uint8, 4>{ 0x01, 0x01, 0x01, 0x02 }, "first chunk command fields") && ok;
    ok      = expectBytes(packet, fragmentsServmesTimestampOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "first chunk timestamp") && ok;
    ok      = expectBytes(packet, fragmentsServmesSizeTotalOffset, std::array<uint8, 4>{ 0x06, 0x00, 0x00, 0x00 }, "first chunk size_total") && ok;
    ok      = expectBytes(packet, fragmentsServmesOffsetOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "first chunk offset") && ok;
    ok      = expectBytes(packet, fragmentsServmesDataSizeOffset, std::array<uint8, 4>{ 0x06, 0x00, 0x00, 0x00 }, "first chunk data_size") && ok;
    ok      = expectBytes(packet, fragmentsServmesDataOffset, std::array<uint8, 6>{ 'H', 'e', 'l', 'l', 'o', 0x00 }, "first chunk data") && ok;
    ok      = expectRepeatedByte(packet, fragmentsServmesDataOffset + 6, PACKET_SIZE - fragmentsServmesDataOffset - 6, 0, "first chunk padding and tail") && ok;
    return ok;
}

auto testLaterChunkConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_FRAGMENTS::SERVMES("Hello", 4, 0x55667788, 2);
    packet.setSequence(0x1234);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x04D, "later chunk type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 28, "later chunk size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x4D, 0x0E, 0x34, 0x12 }, "later chunk header") && ok;
    ok      = expectBytes(packet, fragmentsServmesCommandOffset, std::array<uint8, 4>{ 0x02, 0x01, 0x01, 0x04 }, "later chunk command fields") && ok;
    ok      = expectBytes(packet, fragmentsServmesTimestampOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x66, 0x55 }, "later chunk timestamp") && ok;
    ok      = expectBytes(packet, fragmentsServmesSizeTotalOffset, std::array<uint8, 4>{ 0x06, 0x00, 0x00, 0x00 }, "later chunk size_total") && ok;
    ok      = expectBytes(packet, fragmentsServmesOffsetOffset, std::array<uint8, 4>{ 0x02, 0x00, 0x00, 0x00 }, "later chunk offset") && ok;
    ok      = expectBytes(packet, fragmentsServmesDataSizeOffset, std::array<uint8, 4>{ 0x04, 0x00, 0x00, 0x00 }, "later chunk data_size") && ok;
    ok      = expectBytes(packet, fragmentsServmesDataOffset, std::array<uint8, 4>{ 'l', 'l', 'o', 0x00 }, "later chunk data") && ok;
    ok      = expectRepeatedByte(packet, fragmentsServmesDataOffset + 4, PACKET_SIZE - fragmentsServmesDataOffset - 4, 0, "later chunk padding and tail") && ok;
    return ok;
}

auto testEmptyOrOutOfRangeMessageKeepsDefaultSize() -> bool
{
    auto emptyPacket      = GP_SERV_COMMAND_FRAGMENTS::SERVMES("", 2, 0x11223344, 0);
    auto outOfRangePacket = GP_SERV_COMMAND_FRAGMENTS::SERVMES("Hello", 2, 0x11223344, 6);

    bool ok = true;
    ok      = expectEqualUInt(emptyPacket.getSize(), fragmentsServmesFullPacketSize, "empty message size") && ok;
    ok      = expectEqualUInt(outOfRangePacket.getSize(), fragmentsServmesFullPacketSize, "out-of-range message size") && ok;
    ok      = expectBytes(emptyPacket, fragmentsServmesCommandOffset, std::array<uint8, 4>{ 0x01, 0x01, 0x01, 0x02 }, "empty command fields") && ok;
    ok      = expectBytes(outOfRangePacket, fragmentsServmesCommandOffset, std::array<uint8, 4>{ 0x02, 0x01, 0x01, 0x02 }, "out-of-range command fields") && ok;
    ok      = expectRepeatedByte(emptyPacket, fragmentsServmesSizeTotalOffset, PACKET_SIZE - fragmentsServmesSizeTotalOffset, 0, "empty payload fields") && ok;
    ok      = expectRepeatedByte(outOfRangePacket, fragmentsServmesSizeTotalOffset, PACKET_SIZE - fragmentsServmesSizeTotalOffset, 0, "out-of-range payload fields") && ok;
    return ok;
}

} // namespace

auto runS2CFragmentsServmesPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testFirstChunkConstructor() && ok;
    ok      = testLaterChunkConstructor() && ok;
    ok      = testEmptyOrOutOfRangeMessageKeepsDefaultSize() && ok;
    return ok;
}
