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

#include "test_s2c_miscdata_unity_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

#include "map/packets/s2c/0x063_miscdata_unity.h"

namespace
{

using UnityBasePacket     = GP_SERV_COMMAND_MISCDATA::UNITY::BASE;
using UnityMembersPacket  = GP_SERV_COMMAND_MISCDATA::UNITY::MEMBERS;
using UnityPointsPacket   = GP_SERV_COMMAND_MISCDATA::UNITY::POINTS;
using UnityPersonalPacket = GP_SERV_COMMAND_MISCDATA::UNITY::PERSONAL;
using UnityDataPacket     = GP_SERV_COMMAND_MISCDATA::UNITY::DATA;

constexpr auto unityBasePacketDataSize     = sizeof(UnityBasePacket::PacketData);
constexpr auto unityMembersPacketDataSize  = sizeof(UnityMembersPacket::PacketData);
constexpr auto unityPointsPacketDataSize   = sizeof(UnityPointsPacket::PacketData);
constexpr auto unityPersonalPacketDataSize = sizeof(UnityPersonalPacket::PacketData);
constexpr auto unityDataPacketDataSize     = sizeof(UnityDataPacket::PacketData);
constexpr auto unityBasePacketSize         = sizeof(GP_SERV_HEADER) + unityBasePacketDataSize;
constexpr auto unityMembersPacketSize      = sizeof(GP_SERV_HEADER) + unityMembersPacketDataSize;
constexpr auto unityPointsPacketSize       = sizeof(GP_SERV_HEADER) + unityPointsPacketDataSize;
constexpr auto unityTypeOffset     = sizeof(GP_SERV_HEADER) + offsetof(UnityBasePacket::PacketData, type);
constexpr auto unityUnknown06Offset = sizeof(GP_SERV_HEADER) + offsetof(UnityBasePacket::PacketData, unknown06);
constexpr auto unityResultSetOffset = sizeof(GP_SERV_HEADER) + offsetof(UnityBasePacket::PacketData, resultSet);
constexpr auto unityDataTypeOffset  = sizeof(GP_SERV_HEADER) + offsetof(UnityBasePacket::PacketData, dataType);
constexpr auto unityValueOffset     = sizeof(GP_SERV_HEADER) + offsetof(UnityDataPacket::PacketData, value);
constexpr auto unityMembersOffset   = sizeof(GP_SERV_HEADER) + offsetof(UnityMembersPacket::PacketData, members);
constexpr auto unityPointsOffset    = sizeof(GP_SERV_HEADER) + offsetof(UnityPointsPacket::PacketData, points);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c MISCDATA UNITY packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c MISCDATA UNITY packet self-test failed: " << label << " got";
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
            std::cerr << "s2c MISCDATA UNITY packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
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

auto unityData() -> std::array<std::pair<int32, double>, 11>
{
    auto data = std::array<std::pair<int32, double>, 11>{};
    data[0]   = { 1000, 2000.75 };
    data[4]   = { 0x11223344, 0x01020304 };
    data[10]  = { 99, 12345.0 };
    return data;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_MISCDATA), 0x063, "MISCDATA packet id") && ok;
    ok      = expectEqualUInt(static_cast<uint16>(GP_SERV_COMMAND_MISCDATA_TYPE::Unity), 0x07, "Unity miscdata type") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_RESULTSET::PreviousWeek), 0x00, "PreviousWeek result set") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_RESULTSET::CurrentWeek), 0x01, "CurrentWeek result set") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_DATATYPE::Base), 0x00, "Base data type") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_DATATYPE::Members), 0x01, "Members data type") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_DATATYPE::Points), 0x02, "Points data type") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(UNITY_DATATYPE::Personal), 0x14, "Personal data type") && ok;
    ok      = expectEqualUInt(unityBasePacketDataSize, 144, "BASE sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityMembersPacketDataSize, 148, "MEMBERS sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityPointsPacketDataSize, 148, "POINTS sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityPersonalPacketDataSize, 144, "PERSONAL sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityDataPacketDataSize, 144, "DATA sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(unityBasePacketSize, 148, "BASE packet size") && ok;
    ok      = expectEqualUInt(unityMembersPacketSize, 152, "MEMBERS packet size") && ok;
    ok      = expectEqualUInt(unityPointsPacketSize, 152, "POINTS packet size") && ok;
    ok      = expectEqualUInt(unityTypeOffset, 4, "type offset") && ok;
    ok      = expectEqualUInt(unityUnknown06Offset, 6, "unknown06 offset") && ok;
    ok      = expectEqualUInt(unityResultSetOffset, 8, "resultSet offset") && ok;
    ok      = expectEqualUInt(unityDataTypeOffset, 9, "dataType offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(UnityBasePacket::PacketData, timestamp), 16, "timestamp offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(UnityMembersPacket::PacketData, statusFlag), 16, "members statusFlag offset") && ok;
    ok      = expectEqualUInt(unityMembersOffset, 20, "members offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(UnityPointsPacket::PacketData, statusFlag), 16, "points statusFlag offset") && ok;
    ok      = expectEqualUInt(unityPointsOffset, 20, "points offset") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER) + offsetof(UnityPersonalPacket::PacketData, rankingPoints), 16, "rankingPoints offset") && ok;
    ok      = expectEqualUInt(unityValueOffset, 16, "value offset") && ok;
    return ok;
}

auto testBaseConstructors() -> bool
{
    auto current = UnityBasePacket(UNITY_RESULTSET::CurrentWeek, UNITY_DATATYPE::Base);
    current.setSequence(0xBEEF);

    auto emptySubtype = UnityBasePacket(UNITY_RESULTSET::PreviousWeek, static_cast<UNITY_DATATYPE>(0x03));

    bool ok = true;
    ok      = expectEqualUInt(current.getType(), 0x063, "BASE type") && ok;
    ok      = expectEqualUInt(current.getSize(), unityBasePacketSize, "BASE size") && ok;
    ok      = expectBytes(current, 0, std::array<uint8, 16>{ 0x63, 0x4A, 0xEF, 0xBE, 0x07, 0x00, 0x90, 0x00, 0x01, 0x00, 0, 0, 0, 0, 0, 0 }, "BASE current prefix") && ok;
    ok      = expectZeroRange(current, 16, unityBasePacketSize, "BASE current timestamp and tail") && ok;
    ok      = expectEqualUInt(packetData(emptySubtype)[unityResultSetOffset], 0, "empty subtype resultSet") && ok;
    ok      = expectEqualUInt(packetData(emptySubtype)[unityDataTypeOffset], 0x03, "empty subtype dataType") && ok;
    ok      = expectZeroRange(current, unityBasePacketSize, PACKET_SIZE, "BASE packet tail") && ok;
    return ok;
}

auto testMembersConstructor() -> bool
{
    const auto data = unityData();
    auto packet     = UnityMembersPacket(UNITY_RESULTSET::CurrentWeek, data.data());

    auto expected = std::array<uint8, 64>{};
    putLE16(expected, 0, 0x0108);
    putLE32(expected, 4, 1000);
    putLE32(expected, 20, 0x11223344);
    putLE32(expected, 44, 99);

    bool ok = true;
    ok      = expectBytes(packet, 4, std::array<uint8, 6>{ 0x07, 0x00, 0x94, 0x00, 0x01, 0x01 }, "MEMBERS type prefix") && ok;
    ok      = expectBytes(packet, 16, expected, "MEMBERS status and data") && ok;
    ok      = expectZeroRange(packet, 64, unityMembersPacketSize, "MEMBERS tail") && ok;
    return ok;
}

auto testPointsConstructor() -> bool
{
    const auto data = unityData();
    auto previous   = UnityPointsPacket(UNITY_RESULTSET::PreviousWeek, data.data());
    auto current    = UnityPointsPacket(UNITY_RESULTSET::CurrentWeek, data.data());

    auto expected = std::array<uint8, 64>{};
    putLE16(expected, 0, 0x0207);
    putLE32(expected, 4, 2000);
    putLE32(expected, 20, 0x01020304);
    putLE32(expected, 44, 12345);

    bool ok = true;
    ok      = expectBytes(previous, 4, std::array<uint8, 6>{ 0x07, 0x00, 0x94, 0x00, 0x00, 0x02 }, "POINTS previous type prefix") && ok;
    ok      = expectBytes(previous, 16, expected, "POINTS previous status and data") && ok;
    ok      = expectBytes(current, 16, std::array<uint8, 2>{ 0x08, 0x02 }, "POINTS current status") && ok;
    ok      = expectZeroRange(previous, 64, unityPointsPacketSize, "POINTS tail") && ok;
    return ok;
}

auto testPersonalAndDataConstructors() -> bool
{
    auto personal = UnityPersonalPacket(UNITY_RESULTSET::PreviousWeek, 0xBEEF);
    auto data     = UnityDataPacket(UNITY_RESULTSET::CurrentWeek, 0x15, 0xCAFE);

    bool ok = true;
    ok      = expectBytes(personal, 4, std::array<uint8, 14>{ 0x07, 0x00, 0x90, 0x00, 0x00, 0x14, 0, 0, 0, 0, 0, 0, 0xEF, 0xBE }, "PERSONAL prefix and value") && ok;
    ok      = expectZeroRange(personal, 18, unityBasePacketSize, "PERSONAL tail") && ok;
    ok      = expectBytes(data, 4, std::array<uint8, 14>{ 0x07, 0x00, 0x90, 0x00, 0x01, 0x15, 0, 0, 0, 0, 0, 0, 0xFE, 0xCA }, "DATA prefix and value") && ok;
    ok      = expectZeroRange(data, 18, unityBasePacketSize, "DATA tail") && ok;
    return ok;
}

} // namespace

auto runS2CMiscDataUnityPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testBaseConstructors() && ok;
    ok      = testMembersConstructor() && ok;
    ok      = testPointsConstructor() && ok;
    ok      = testPersonalAndDataConstructors() && ok;
    return ok;
}
