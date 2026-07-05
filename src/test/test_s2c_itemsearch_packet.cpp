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

#include "test_s2c_itemsearch_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x049_itemsearch.h"

namespace
{

constexpr auto itemSearchItemNoOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEMSEARCH::PacketData, ItemNo);
constexpr auto itemSearchFlagOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEMSEARCH::PacketData, Flag);
constexpr auto itemSearchPadding07Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEMSEARCH::PacketData, padding07);
constexpr auto itemSearchItemNameOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEMSEARCH::PacketData, ItemName);
constexpr auto itemSearchPacketDataSize    = sizeof(GP_SERV_COMMAND_ITEMSEARCH::PacketData);
constexpr auto itemSearchFullPacketSize    = sizeof(GP_SERV_HEADER) + itemSearchPacketDataSize;
constexpr auto itemSearchItemNameFieldSize = sizeof(GP_SERV_COMMAND_ITEMSEARCH::PacketData::ItemName);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEMSEARCH packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEMSEARCH packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ITEMSEARCH packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[offset + i]) << " expected " << static_cast<unsigned>(expected) << '\n';
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemSearchPacketDataSize, 68, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemSearchFullPacketSize, 72, "full packet size") && ok;
    ok      = expectEqualUInt(itemSearchItemNoOffset, 4, "ItemNo offset") && ok;
    ok      = expectEqualUInt(itemSearchFlagOffset, 6, "Flag offset") && ok;
    ok      = expectEqualUInt(itemSearchPadding07Offset, 7, "padding07 offset") && ok;
    ok      = expectEqualUInt(itemSearchItemNameOffset, 8, "ItemName offset") && ok;
    ok      = expectEqualUInt(itemSearchItemNameFieldSize, 64, "ItemName field size") && ok;
    return ok;
}

auto testConstructorCopiesShortNameAndZerosTail() -> bool
{
    auto packet = GP_SERV_COMMAND_ITEMSEARCH(0x1234, "Beeswax");
    packet.setSequence(0xCAFE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x049, "short name type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemSearchFullPacketSize, "short name size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x49, 0x24, 0xFE, 0xCA }, "short name header") && ok;
    ok      = expectBytes(packet, itemSearchItemNoOffset, std::array<uint8, 4>{ 0x34, 0x12, 0x00, 0x00 }, "short name scalar fields") && ok;
    ok      = expectBytes(packet, itemSearchItemNameOffset, std::array<uint8, 7>{ 'B', 'e', 'e', 's', 'w', 'a', 'x' }, "short name bytes") && ok;
    ok      = expectRepeatedByte(packet, itemSearchItemNameOffset + 7, itemSearchItemNameFieldSize - 7, 0, "short name tail") && ok;
    ok      = expectRepeatedByte(packet, itemSearchFullPacketSize, PACKET_SIZE - itemSearchFullPacketSize, 0, "packet tail") && ok;
    return ok;
}

auto testConstructorTruncatesLongName() -> bool
{
    const auto name   = std::string(itemSearchItemNameFieldSize + 9, 'Z');
    auto       packet = GP_SERV_COMMAND_ITEMSEARCH(0xBEEF, name);

    bool ok = true;
    ok      = expectBytes(packet, itemSearchItemNoOffset, std::array<uint8, 4>{ 0xEF, 0xBE, 0x00, 0x00 }, "long name scalar fields") && ok;
    ok      = expectRepeatedByte(packet, itemSearchItemNameOffset, itemSearchItemNameFieldSize, 'Z', "long name bytes") && ok;
    ok      = expectRepeatedByte(packet, itemSearchFullPacketSize, PACKET_SIZE - itemSearchFullPacketSize, 0, "long packet tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemSearchPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorCopiesShortNameAndZerosTail() && ok;
    ok      = testConstructorTruncatesLongName() && ok;
    return ok;
}
