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

#include "test_s2c_dungeon_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0ad_dungeon.h"

namespace
{

using DungeonPacket = GP_SERV_COMMAND_DUNGEON;

constexpr auto dungeonVouchersOffset      = sizeof(GP_SERV_HEADER) + offsetof(DungeonPacket::PacketData, Vouchers);
constexpr auto dungeonRunesOffset         = sizeof(GP_SERV_HEADER) + offsetof(DungeonPacket::PacketData, Runes);
constexpr auto dungeonUnused00Offset      = sizeof(GP_SERV_HEADER) + offsetof(DungeonPacket::PacketData, unused00);
constexpr auto dungeonVouchersSize        = sizeof(DungeonPacket::PacketData{}.Vouchers);
constexpr auto dungeonRunesSize           = sizeof(DungeonPacket::PacketData{}.Runes);
constexpr auto dungeonUnused00Size        = sizeof(DungeonPacket::PacketData{}.unused00);
constexpr auto dungeonPacketDataSize      = sizeof(DungeonPacket::PacketData);
constexpr auto dungeonPacketSize          = sizeof(GP_SERV_HEADER) + dungeonPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c DUNGEON packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c DUNGEON packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_DUNGEON), 0x0AD, "DUNGEON packet id") && ok;
    ok      = expectEqualUInt(dungeonVouchersSize, 8, "Vouchers size") && ok;
    ok      = expectEqualUInt(dungeonRunesSize, 64, "Runes size") && ok;
    ok      = expectEqualUInt(dungeonUnused00Size, 56, "unused00 size") && ok;
    ok      = expectEqualUInt(dungeonPacketDataSize, 128, "sizeof(DUNGEON::PacketData)") && ok;
    ok      = expectEqualUInt(dungeonPacketSize, 132, "DUNGEON packet size") && ok;
    ok      = expectEqualUInt(dungeonVouchersOffset, 4, "Vouchers offset") && ok;
    ok      = expectEqualUInt(dungeonRunesOffset, 12, "Runes offset") && ok;
    ok      = expectEqualUInt(dungeonUnused00Offset, 76, "unused00 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data         = DungeonPacket::PacketData{};
    data.Vouchers[0] = 0x11;
    data.Vouchers[7] = 0x22;
    data.Runes[0]    = 0x33;
    data.Runes[63]   = 0x44;
    data.unused00[0]  = 0x55;
    data.unused00[55] = 0x66;

    auto expected = std::array<uint8, 128>{};
    expected[0]   = 0x11;
    expected[7]   = 0x22;
    expected[8]   = 0x33;
    expected[71]  = 0x44;
    expected[72]  = 0x55;
    expected[127] = 0x66;

    return expectStructBytes(data, expected, "DUNGEON PacketData bytes");
}

} // namespace

auto runS2CDungeonPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
