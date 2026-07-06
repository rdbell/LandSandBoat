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

#include "test_s2c_scenarioitem_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x055_scenarioitem.h"

namespace
{

using ScenarioItemPacket = GP_SERV_COMMAND_SCENARIOITEM;

constexpr auto scenarioItemGetItemFlagOffset  = sizeof(GP_SERV_HEADER);
constexpr auto scenarioItemLookItemFlagOffset = sizeof(GP_SERV_HEADER) + offsetof(ScenarioItemPacket::PacketData, LookItemFlag);
constexpr auto scenarioItemTableIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(ScenarioItemPacket::PacketData, TableIndex);
constexpr auto scenarioItemPadding00Offset    = sizeof(GP_SERV_HEADER) + offsetof(ScenarioItemPacket::PacketData, padding00);
constexpr auto scenarioItemPacketDataSize     = sizeof(ScenarioItemPacket::PacketData);
constexpr auto scenarioItemPacketSize         = sizeof(GP_SERV_HEADER) + scenarioItemPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c SCENARIOITEM packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c SCENARIOITEM packet self-test failed: " << label << " got";
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
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_SCENARIOITEM), 0x055, "SCENARIOITEM packet id") && ok;
    ok      = expectEqualUInt(scenarioItemPacketDataSize, 132, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(scenarioItemPacketSize, 136, "packet size") && ok;
    ok      = expectEqualUInt(scenarioItemGetItemFlagOffset, 4, "GetItemFlag offset") && ok;
    ok      = expectEqualUInt(scenarioItemLookItemFlagOffset, 68, "LookItemFlag offset") && ok;
    ok      = expectEqualUInt(scenarioItemTableIndexOffset, 132, "TableIndex offset") && ok;
    ok      = expectEqualUInt(scenarioItemPadding00Offset, 134, "padding00 offset") && ok;
    ok      = expectEqualUInt(sizeof(ScenarioItemPacket::PacketData::GetItemFlag), 64, "GetItemFlag size") && ok;
    ok      = expectEqualUInt(sizeof(ScenarioItemPacket::PacketData::LookItemFlag), 64, "LookItemFlag size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = ScenarioItemPacket::PacketData{};

    data.GetItemFlag[0]   = 0x80000005;
    data.GetItemFlag[3]   = 0x11223344;
    data.GetItemFlag[15]  = 0xCAFEBABE;
    data.LookItemFlag[0]  = 0x00000001;
    data.LookItemFlag[1]  = 0x00000002;
    data.LookItemFlag[5]  = 0x00010000;
    data.LookItemFlag[15] = 0x80000000;
    data.TableIndex       = 3;
    data.padding00        = 0xBEEF;

    auto expected = std::array<uint8, scenarioItemPacketDataSize>{};
    putLE32(expected, 0, 0x80000005);
    putLE32(expected, 12, 0x11223344);
    putLE32(expected, 60, 0xCAFEBABE);
    putLE32(expected, 64, 0x00000001);
    putLE32(expected, 68, 0x00000002);
    putLE32(expected, 84, 0x00010000);
    putLE32(expected, 124, 0x80000000);
    putLE16(expected, 128, 3);
    putLE16(expected, 130, 0xBEEF);

    return expectStructBytes(data, expected, "SCENARIOITEM PacketData bytes");
}

} // namespace

auto runS2CScenarioItemPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
