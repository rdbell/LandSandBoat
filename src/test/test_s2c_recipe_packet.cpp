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

#include "test_s2c_recipe_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x031_recipe.h"

namespace
{

using RecipePacket = GP_SERV_COMMAND_RECIPE;

constexpr auto recipePacketDataSize = sizeof(RecipePacket::PacketData);
constexpr auto recipePacketSize     = sizeof(GP_SERV_HEADER) + recipePacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c RECIPE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c RECIPE packet self-test failed: " << label << " got";
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
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_RECIPE), 0x031, "RECIPE packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RECIPE_TYPE::RecipeDetail1), 1, "RecipeDetail1 type") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RECIPE_TYPE::RecipeList), 2, "RecipeList type") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RECIPE_TYPE::RecipeDetail2), 3, "RecipeDetail2 type") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(GP_SERV_COMMAND_RECIPE_TYPE::Unknown), 4, "Unknown type") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_RECIPE_TYPE1_3), 48, "sizeof(TYPE1_3)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_RECIPE_TYPE2), 48, "sizeof(TYPE2)") && ok;
    ok      = expectEqualUInt(recipePacketDataSize, 48, "PacketData size") && ok;
    ok      = expectEqualUInt(recipePacketSize, 52, "packet size") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, productitem), 0, "TYPE1_3 productitem offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_1), 2, "TYPE1_3 need_skill_1 offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_2), 4, "TYPE1_3 need_skill_2 offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_3), 6, "TYPE1_3 need_skill_3 offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_item), 8, "TYPE1_3 need_item offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_key_item), 10, "TYPE1_3 need_key_item offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemnum), 12, "TYPE1_3 itemnum offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemcount), 28, "TYPE1_3 itemcount offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, Type), 44, "TYPE1_3 Type offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, unknown32), 46, "TYPE1_3 unknown32 offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, unused04), 0, "TYPE2 unused04 offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum), 12, "TYPE2 itemnum offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, Type), 44, "TYPE2 Type offset") && ok;
    ok      = expectEqualUInt(offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum_next), 46, "TYPE2 itemnum_next offset") && ok;
    return ok;
}

auto testDetailPacketDataBytes() -> bool
{
    auto data                    = RecipePacket::PacketData{};
    data.Details.productitem     = 0x1001;
    data.Details.need_skill_1    = 0x0002;
    data.Details.need_skill_2    = 0x0003;
    data.Details.need_skill_3    = 0x0004;
    data.Details.need_item       = 0x1002;
    data.Details.need_key_item   = 0x1003;
    data.Details.itemnum[0]      = 0x2001;
    data.Details.itemnum[1]      = 0x2002;
    data.Details.itemnum[7]      = 0x2008;
    data.Details.itemcount[0]    = 0x0001;
    data.Details.itemcount[1]    = 0x0002;
    data.Details.itemcount[7]    = 0x0008;
    data.Details.Type            = GP_SERV_COMMAND_RECIPE_TYPE::RecipeDetail2;
    data.Details.unknown32       = 0x9988;

    auto expected = std::array<uint8, recipePacketDataSize>{};
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, productitem), 0x1001);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_1), 0x0002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_2), 0x0003);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_skill_3), 0x0004);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_item), 0x1002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, need_key_item), 0x1003);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemnum) + 0 * 2, 0x2001);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemnum) + 1 * 2, 0x2002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemnum) + 7 * 2, 0x2008);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemcount) + 0 * 2, 0x0001);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemcount) + 1 * 2, 0x0002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, itemcount) + 7 * 2, 0x0008);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, Type), 0x0003);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE1_3, unknown32), 0x9988);

    return expectStructBytes(data, expected, "RECIPE detail PacketData bytes");
}

auto testListPacketDataBytes() -> bool
{
    auto data               = RecipePacket::PacketData{};
    data.List.unused04[0]  = 0x1001;
    data.List.unused04[5]  = 0x1006;
    data.List.itemnum[0]   = 0x2001;
    data.List.itemnum[1]   = 0x2002;
    data.List.itemnum[15]  = 0x2010;
    data.List.Type         = GP_SERV_COMMAND_RECIPE_TYPE::RecipeList;
    data.List.itemnum_next = 0x3030;

    auto expected = std::array<uint8, recipePacketDataSize>{};
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, unused04) + 0 * 2, 0x1001);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, unused04) + 5 * 2, 0x1006);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum) + 0 * 2, 0x2001);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum) + 1 * 2, 0x2002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum) + 15 * 2, 0x2010);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, Type), 0x0002);
    putLE16(expected, offsetof(GP_SERV_COMMAND_RECIPE_TYPE2, itemnum_next), 0x3030);

    return expectStructBytes(data, expected, "RECIPE list PacketData bytes");
}

} // namespace

auto runS2CRecipePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testDetailPacketDataBytes() && ok;
    ok      = testListPacketDataBytes() && ok;
    return ok;
}
