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

#include "test_s2c_item_same_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/packets/s2c/0x01d_item_same.h"

namespace
{

constexpr auto itemSameStateOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SAME::PacketData, State);
constexpr auto itemSamePadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SAME::PacketData, padding00);
constexpr auto itemSameFlagsOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_ITEM_SAME::PacketData, Flags);
constexpr auto itemSamePacketDataSize   = sizeof(GP_SERV_COMMAND_ITEM_SAME::PacketData);
constexpr auto itemSameFullPacketSize   = sizeof(GP_SERV_HEADER) + itemSamePacketDataSize;
constexpr auto representativeSyncFlags  = (1U << LOC_INVENTORY) | (1U << LOC_WARDROBE3) | (1U << LOC_RECYCLEBIN);
constexpr auto partialSyncFlags         = (1U << LOC_INVENTORY) | (1U << LOC_RECYCLEBIN);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ITEM_SAME packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ITEM_SAME packet self-test failed: " << label << " got";
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

auto expectZeroTail(CBasicPacket& packet, std::size_t offset, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    for (std::size_t i = offset; i < PACKET_SIZE; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c ITEM_SAME packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void syncCharacter(CCharEntity& character, bool includeWardrobe3)
{
    character.inventorySyncState().markSynced(LOC_INVENTORY);
    if (includeWardrobe3)
    {
        character.inventorySyncState().markSynced(LOC_WARDROBE3);
    }
    character.inventorySyncState().markSynced(LOC_RECYCLEBIN);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(itemSamePacketDataSize, 8, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(itemSameFullPacketSize, 12, "full packet size") && ok;
    ok      = expectEqualUInt(itemSameStateOffset, 4, "State offset") && ok;
    ok      = expectEqualUInt(itemSamePadding00Offset, 5, "padding00 offset") && ok;
    ok      = expectEqualUInt(itemSameFlagsOffset, 8, "Flags offset") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_ITEM_SAME_STATE::StillLoading), 0, "StillLoading state") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_ITEM_SAME_STATE::AllLoaded), 1, "AllLoaded state") && ok;
    ok      = expectEqualUInt(MAX_CONTAINER_ID, 18, "MAX_CONTAINER_ID") && ok;
    ok      = expectEqualUInt(representativeSyncFlags, 0x00020801, "representative sync flags") && ok;
    return ok;
}

auto testAllLoadedConstructor() -> bool
{
    auto character = CCharEntity{};
    syncCharacter(character, true);
    auto packet = GP_SERV_COMMAND_ITEM_SAME(&character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x01D, "ITEM_SAME type") && ok;
    ok      = expectEqualUInt(packet.getSize(), itemSameFullPacketSize, "ITEM_SAME size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 12>{ 0x1D, 0x06, 0xEF, 0xBE, 0x01, 0x12, 0x00, 0x00, 0x01, 0x08, 0x02, 0x00 }, "all-loaded constructor bytes") && ok;
    ok      = expectZeroTail(packet, itemSameFullPacketSize, "all-loaded constructor tail") && ok;
    return ok;
}

auto testStillLoadingConstructor() -> bool
{
    auto character = CCharEntity{};
    syncCharacter(character, false);
    auto packet = GP_SERV_COMMAND_ITEM_SAME(LOC_WARDROBE3, &character);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(character.inventorySyncState().getSyncedFlags(), partialSyncFlags, "partial sync flags") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 12>{ 0x1D, 0x06, 0xEF, 0xBE, 0x00, 0x0B, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00 }, "still-loading constructor bytes") && ok;
    ok      = expectZeroTail(packet, itemSameFullPacketSize, "still-loading constructor tail") && ok;
    return ok;
}

} // namespace

auto runS2CItemSamePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testAllLoadedConstructor() && ok;
    ok      = testStillLoadingConstructor() && ok;
    return ok;
}
