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

#include "test_s2c_trophy_list_packet.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "common/timer.h"
#include "entities/char_entity.h"
#include "map/packets/s2c/0x0d2_trophy_list.h"
#include "treasure_pool.h"

namespace
{

constexpr auto trophyListTrophyItemNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TrophyItemNum);
constexpr auto trophyListTargetUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TargetUniqueNo);
constexpr auto trophyListGoldOffset            = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, Gold);
constexpr auto trophyListPadding00Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, padding00);
constexpr auto trophyListTrophyItemNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TrophyItemNo);
constexpr auto trophyListTargetActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TargetActIndex);
constexpr auto trophyListTrophyItemIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, TrophyItemIndex);
constexpr auto trophyListEntryOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, Entry);
constexpr auto trophyListIsContainerOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, IsContainer);
constexpr auto trophyListPadding01Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, padding01);
constexpr auto trophyListStartTimeOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, StartTime);
constexpr auto trophyListIsLocallyLottedOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, IsLocallyLotted);
constexpr auto trophyListPointOffset           = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, Point);
constexpr auto trophyListLootUniqueNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, LootUniqueNo);
constexpr auto trophyListLootActIndexOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, LootActIndex);
constexpr auto trophyListLootPointOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, LootPoint);
constexpr auto trophyListLootActNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_LIST::PacketData, LootActName);
constexpr auto trophyListFlagsOffset           = trophyListLootActNameOffset + sizeof(GP_SERV_COMMAND_TROPHY_LIST::PacketData::LootActName);
constexpr auto trophyListPadding02Offset       = trophyListFlagsOffset + 1;
constexpr auto trophyListLootActNameSize       = sizeof(GP_SERV_COMMAND_TROPHY_LIST::PacketData::LootActName);
constexpr auto trophyListPadding02Size         = sizeof(GP_SERV_COMMAND_TROPHY_LIST::PacketData::padding02);
constexpr auto trophyListPacketDataSize        = sizeof(GP_SERV_COMMAND_TROPHY_LIST::PacketData);
constexpr auto trophyListPacketSize            = sizeof(GP_SERV_HEADER) + trophyListPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TROPHY_LIST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TROPHY_LIST packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TROPHY_LIST packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto makeItem(std::uint16_t itemId, std::uint8_t slotId, std::uint32_t ageMilliseconds) -> TreasurePoolItem
{
    auto item       = TreasurePoolItem{};
    item.ID         = itemId;
    item.SlotID     = slotId;
    item.TimeStamp  = timer::start_time + std::chrono::milliseconds(ageMilliseconds);
    return item;
}

void populateEntity(CCharEntity& entity, std::uint32_t id, std::uint16_t targid, ENTITYTYPE objtype)
{
    entity.id      = id;
    entity.targid  = targid;
    entity.objtype = objtype;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(trophyListPacketDataSize, 56, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(trophyListPacketSize, 60, "packet size") && ok;
    ok      = expectEqualUInt(trophyListTrophyItemNumOffset, 4, "TrophyItemNum offset") && ok;
    ok      = expectEqualUInt(trophyListTargetUniqueNoOffset, 8, "TargetUniqueNo offset") && ok;
    ok      = expectEqualUInt(trophyListGoldOffset, 12, "Gold offset") && ok;
    ok      = expectEqualUInt(trophyListPadding00Offset, 14, "padding00 offset") && ok;
    ok      = expectEqualUInt(trophyListTrophyItemNoOffset, 16, "TrophyItemNo offset") && ok;
    ok      = expectEqualUInt(trophyListTargetActIndexOffset, 18, "TargetActIndex offset") && ok;
    ok      = expectEqualUInt(trophyListTrophyItemIndexOffset, 20, "TrophyItemIndex offset") && ok;
    ok      = expectEqualUInt(trophyListEntryOffset, 21, "Entry offset") && ok;
    ok      = expectEqualUInt(trophyListIsContainerOffset, 22, "IsContainer offset") && ok;
    ok      = expectEqualUInt(trophyListPadding01Offset, 23, "padding01 offset") && ok;
    ok      = expectEqualUInt(trophyListStartTimeOffset, 24, "StartTime offset") && ok;
    ok      = expectEqualUInt(trophyListIsLocallyLottedOffset, 28, "IsLocallyLotted offset") && ok;
    ok      = expectEqualUInt(trophyListPointOffset, 30, "Point offset") && ok;
    ok      = expectEqualUInt(trophyListLootUniqueNoOffset, 32, "LootUniqueNo offset") && ok;
    ok      = expectEqualUInt(trophyListLootActIndexOffset, 36, "LootActIndex offset") && ok;
    ok      = expectEqualUInt(trophyListLootPointOffset, 38, "LootPoint offset") && ok;
    ok      = expectEqualUInt(trophyListLootActNameOffset, 40, "LootActName offset") && ok;
    ok      = expectEqualUInt(trophyListFlagsOffset, 56, "flags bitfield offset") && ok;
    ok      = expectEqualUInt(trophyListPadding02Offset, 57, "padding02 offset") && ok;
    ok      = expectEqualUInt(trophyListLootActNameSize, 16, "LootActName size") && ok;
    ok      = expectEqualUInt(trophyListPadding02Size, 3, "padding02 size") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_TROPHY_LIST), 0x0D2, "packet id") && ok;
    return ok;
}

auto testConstructorWithNpcContainer() -> bool
{
    auto item   = makeItem(0x7788, 0x09, 0x01020304);
    auto entity = CCharEntity{};
    populateEntity(entity, 0x11223344, 0x5566, TYPE_NPC);
    auto packet = GP_SERV_COMMAND_TROPHY_LIST(&item, &entity, true);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0D2, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), trophyListPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0xD2, 0x1E, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, trophyListTrophyItemNumOffset, std::array<uint8, 4>{ 0x01, 0x00, 0x00, 0x00 }, "TrophyItemNum") && ok;
    ok      = expectBytes(packet, trophyListTargetUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "TargetUniqueNo") && ok;
    ok      = expectBytes(packet, trophyListTrophyItemNoOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "TrophyItemNo") && ok;
    ok      = expectBytes(packet, trophyListTargetActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "TargetActIndex") && ok;
    ok      = expectBytes(packet, trophyListTrophyItemIndexOffset, std::array<uint8, 4>{ 0x09, 0x01, 0x01, 0x00 }, "slot entry container padding") && ok;
    ok      = expectBytes(packet, trophyListStartTimeOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "StartTime") && ok;
    ok      = expectZeroRange(packet, trophyListGoldOffset, trophyListTrophyItemNoOffset, "gold and padding00") && ok;
    ok      = expectZeroRange(packet, trophyListIsLocallyLottedOffset, PACKET_SIZE, "lotter fields and padding") && ok;
    return ok;
}

auto testConstructorWithoutEntity() -> bool
{
    auto item   = makeItem(0x1234, 0x03, 0);
    auto packet = GP_SERV_COMMAND_TROPHY_LIST(&item, nullptr, false);

    bool ok = true;
    ok      = expectBytes(packet, trophyListTrophyItemNumOffset, std::array<uint8, 4>{ 0x01, 0x00, 0x00, 0x00 }, "nil entity TrophyItemNum") && ok;
    ok      = expectZeroRange(packet, trophyListTargetUniqueNoOffset, trophyListTrophyItemNoOffset, "nil entity target/gold fields") && ok;
    ok      = expectBytes(packet, trophyListTrophyItemNoOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "nil entity TrophyItemNo") && ok;
    ok      = expectZeroRange(packet, trophyListTargetActIndexOffset, trophyListTrophyItemIndexOffset, "nil entity act index") && ok;
    ok      = expectBytes(packet, trophyListTrophyItemIndexOffset, std::array<uint8, 4>{ 0x03, 0x00, 0x00, 0x00 }, "nil entity slot entry container padding") && ok;
    ok      = expectZeroRange(packet, trophyListStartTimeOffset, PACKET_SIZE, "nil entity tail") && ok;
    return ok;
}

} // namespace

auto runS2CTrophyListPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testConstructorWithNpcContainer() && ok;
    ok      = testConstructorWithoutEntity() && ok;
    return ok;
}
