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

#include "test_s2c_trophy_solution_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x0d3_trophy_solution.h"

namespace
{

constexpr auto trophySolutionLootUniqueNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootUniqueNo);
constexpr auto trophySolutionEntryUniqueNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, EntryUniqueNo);
constexpr auto trophySolutionLootActIndexOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootActIndex);
constexpr auto trophySolutionLootPointOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootPoint);
constexpr auto trophySolutionEntryActIndexOffset    = sizeof(GP_SERV_HEADER) + 12;
constexpr auto trophySolutionEntryPointOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, EntryPoint);
constexpr auto trophySolutionTrophyItemIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, TrophyItemIndex);
constexpr auto trophySolutionJudgeFlgOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, JudgeFlg);
constexpr auto trophySolutionLootNameOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, sLootName);
constexpr auto trophySolutionEntryNameOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, sLootName2);
constexpr auto trophySolutionPaddingOffset          = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, padding00);
constexpr auto trophySolutionNameSize               = sizeof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData::sLootName);
constexpr auto trophySolutionPaddingSize            = sizeof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData::padding00);
constexpr auto trophySolutionPacketDataSize         = sizeof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData);
constexpr auto trophySolutionPacketSize             = sizeof(GP_SERV_HEADER) + trophySolutionPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c TROPHY_SOLUTION packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c TROPHY_SOLUTION packet self-test failed: " << label << " got";
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
            std::cerr << "s2c TROPHY_SOLUTION packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void populateEntity(CCharEntity& entity, std::uint32_t id, std::uint16_t targid, const std::string& name)
{
    entity.id     = id;
    entity.targid = targid;
    entity.name   = name;
}

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(trophySolutionPacketDataSize, 56, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(trophySolutionPacketSize, 60, "packet size") && ok;
    ok      = expectEqualUInt(trophySolutionLootUniqueNoOffset, 4, "LootUniqueNo offset") && ok;
    ok      = expectEqualUInt(trophySolutionEntryUniqueNoOffset, 8, "EntryUniqueNo offset") && ok;
    ok      = expectEqualUInt(trophySolutionLootActIndexOffset, 12, "LootActIndex offset") && ok;
    ok      = expectEqualUInt(trophySolutionLootPointOffset, 14, "LootPoint offset") && ok;
    ok      = expectEqualUInt(trophySolutionEntryActIndexOffset, 16, "EntryActIndex bitfield offset") && ok;
    ok      = expectEqualUInt(trophySolutionEntryPointOffset, 18, "EntryPoint offset") && ok;
    ok      = expectEqualUInt(trophySolutionTrophyItemIndexOffset, 20, "TrophyItemIndex offset") && ok;
    ok      = expectEqualUInt(trophySolutionJudgeFlgOffset, 21, "JudgeFlg offset") && ok;
    ok      = expectEqualUInt(trophySolutionLootNameOffset, 22, "sLootName offset") && ok;
    ok      = expectEqualUInt(trophySolutionEntryNameOffset, 38, "sLootName2 offset") && ok;
    ok      = expectEqualUInt(trophySolutionPaddingOffset, 54, "padding00 offset") && ok;
    ok      = expectEqualUInt(trophySolutionNameSize, 16, "name size") && ok;
    ok      = expectEqualUInt(trophySolutionPaddingSize, 6, "padding size") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_TROPHY_SOLUTION), 0x0D3, "packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TROPHY_SOLUTION_STATE::Win), 1, "Win enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TROPHY_SOLUTION_STATE::WinError), 2, "WinError enum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint8_t>(GP_TROPHY_SOLUTION_STATE::Lost), 3, "Lost enum") && ok;
    return ok;
}

auto testMessageConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_TROPHY_SOLUTION(0x09, GP_TROPHY_SOLUTION_STATE::Lost);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0D3, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), trophySolutionPacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0xD3, 0x1E, 0xEF, 0xBE }, "header") && ok;
    ok      = expectBytes(packet, trophySolutionTrophyItemIndexOffset, std::array<uint8, 2>{ 0x09, 0x03 }, "slot and judge") && ok;
    ok      = expectZeroRange(packet, sizeof(GP_SERV_HEADER), trophySolutionTrophyItemIndexOffset, "message prefix") && ok;
    ok      = expectZeroRange(packet, trophySolutionJudgeFlgOffset + 1, PACKET_SIZE, "message tail") && ok;
    return ok;
}

auto testWinnerConstructor() -> bool
{
    auto winner = CCharEntity{};
    populateEntity(winner, 0x11223344, 0x5566, "WinnerLongNameAB");
    auto packet = GP_SERV_COMMAND_TROPHY_SOLUTION(&winner, 0x07, 0x1234, GP_TROPHY_SOLUTION_STATE::Win);

    bool ok = true;
    ok      = expectBytes(packet, trophySolutionLootUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "winner LootUniqueNo") && ok;
    ok      = expectBytes(packet, trophySolutionLootActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "winner LootActIndex") && ok;
    ok      = expectBytes(packet, trophySolutionLootPointOffset, std::array<uint8, 2>{ 0x34, 0x12 }, "winner LootPoint") && ok;
    ok      = expectBytes(packet, trophySolutionTrophyItemIndexOffset, std::array<uint8, 2>{ 0x07, 0x01 }, "winner slot and judge") && ok;
    ok      = expectBytes(packet, trophySolutionLootNameOffset, std::array<uint8, 16>{ 'W', 'i', 'n', 'n', 'e', 'r', 'L', 'o', 'n', 'g', 'N', 'a', 'm', 'e', 'A', 'B' }, "winner name") && ok;
    ok      = expectZeroRange(packet, trophySolutionEntryUniqueNoOffset, trophySolutionLootActIndexOffset, "winner entry unique") && ok;
    ok      = expectZeroRange(packet, trophySolutionEntryActIndexOffset, trophySolutionEntryPointOffset + 2, "winner entry fields") && ok;
    return ok;
}

auto testLotterConstructor() -> bool
{
    auto highest = CCharEntity{};
    auto lotter  = CCharEntity{};
    populateEntity(highest, 0x01020304, 0x4567, "TopLotter");
    populateEntity(lotter, 0xAABBCCDD, 0x2345, "EntryNameIsLonger");
    auto packet = GP_SERV_COMMAND_TROPHY_SOLUTION(&highest, 0x4321, &lotter, 0x05, 0x0F0E);

    bool ok = true;
    ok      = expectBytes(packet, trophySolutionLootUniqueNoOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "lotter LootUniqueNo") && ok;
    ok      = expectBytes(packet, trophySolutionEntryUniqueNoOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "lotter EntryUniqueNo") && ok;
    ok      = expectBytes(packet, trophySolutionLootActIndexOffset, std::array<uint8, 2>{ 0x67, 0x45 }, "lotter LootActIndex") && ok;
    ok      = expectBytes(packet, trophySolutionLootPointOffset, std::array<uint8, 2>{ 0x21, 0x43 }, "lotter LootPoint") && ok;
    ok      = expectBytes(packet, trophySolutionEntryActIndexOffset, std::array<uint8, 2>{ 0x45, 0x23 }, "lotter EntryActIndex bitfield") && ok;
    ok      = expectBytes(packet, trophySolutionEntryPointOffset, std::array<uint8, 2>{ 0x0E, 0x0F }, "lotter EntryPoint") && ok;
    ok      = expectBytes(packet, trophySolutionTrophyItemIndexOffset, std::array<uint8, 2>{ 0x05, 0x00 }, "lotter slot and judge") && ok;
    ok      = expectBytes(packet, trophySolutionLootNameOffset, std::array<uint8, 16>{ 'T', 'o', 'p', 'L', 'o', 't', 't', 'e', 'r', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "lotter highest name") && ok;
    ok      = expectBytes(packet, trophySolutionEntryNameOffset, std::array<uint8, 16>{ 'E', 'n', 't', 'r', 'y', 'N', 'a', 'm', 'e', 'I', 's', 'L', 'o', 'n', 'g', 'e' }, "lotter entry name") && ok;
    ok      = expectZeroRange(packet, trophySolutionPaddingOffset, PACKET_SIZE, "lotter padding and tail") && ok;
    return ok;
}

auto testLotterConstructorWithoutHighest() -> bool
{
    auto lotter = CCharEntity{};
    populateEntity(lotter, 0x10203040, 0x7FFF, "Solo");
    auto packet = GP_SERV_COMMAND_TROPHY_SOLUTION(nullptr, 0x9999, &lotter, 0x02, 0x7777);

    bool ok = true;
    ok      = expectZeroRange(packet, trophySolutionLootUniqueNoOffset, trophySolutionEntryUniqueNoOffset, "nil highest loot fields") && ok;
    ok      = expectBytes(packet, trophySolutionEntryUniqueNoOffset, std::array<uint8, 4>{ 0x40, 0x30, 0x20, 0x10 }, "nil highest EntryUniqueNo") && ok;
    ok      = expectBytes(packet, trophySolutionEntryActIndexOffset, std::array<uint8, 2>{ 0xFF, 0x7F }, "nil highest EntryActIndex") && ok;
    ok      = expectBytes(packet, trophySolutionEntryPointOffset, std::array<uint8, 2>{ 0x77, 0x77 }, "nil highest EntryPoint") && ok;
    ok      = expectBytes(packet, trophySolutionEntryNameOffset, std::array<uint8, 16>{ 'S', 'o', 'l', 'o', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "nil highest entry name") && ok;
    return ok;
}

} // namespace

auto runS2CTrophySolutionPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testMessageConstructor() && ok;
    ok      = testWinnerConstructor() && ok;
    ok      = testLotterConstructor() && ok;
    ok      = testLotterConstructorWithoutHighest() && ok;
    return ok;
}
