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

#include "test_s2c_combine_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/enums/synthesis_result.h"
#include "map/packets/s2c/0x06f_combine_ans.h"
#include "map/packets/s2c/0x070_combine_inf.h"

namespace
{

constexpr auto combineAnsResultOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, Result);
constexpr auto combineAnsGradeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, Grade);
constexpr auto combineAnsCountOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, Count);
constexpr auto combineAnsPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, padding00);
constexpr auto combineAnsItemNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, ItemNo);
constexpr auto combineAnsBreakNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, BreakNo);
constexpr auto combineAnsUpKindOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, UpKind);
constexpr auto combineAnsUpLevelOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, UpLevel);
constexpr auto combineAnsCrystalNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, CrystalNo);
constexpr auto combineAnsMaterialNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_ANS::PacketData, MaterialNo);
constexpr auto combineAnsPadding01Offset  = sizeof(GP_SERV_COMMAND_COMBINE_ANS::PacketData) + sizeof(GP_SERV_HEADER) - sizeof(uint32_t);
constexpr auto combineAnsPacketDataSize   = sizeof(GP_SERV_COMMAND_COMBINE_ANS::PacketData);
constexpr auto combineAnsPacketSize       = sizeof(GP_SERV_HEADER) + combineAnsPacketDataSize;

constexpr auto combineInfResultOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, Result);
constexpr auto combineInfGradeOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, Grade);
constexpr auto combineInfCountOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, Count);
constexpr auto combineInfPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, padding00);
constexpr auto combineInfItemNoOffset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, ItemNo);
constexpr auto combineInfBreakNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, BreakNo);
constexpr auto combineInfUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, UniqueNo);
constexpr auto combineInfActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, ActIndex);
constexpr auto combineInfNameOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, name);
constexpr auto combineInfPadding01Offset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_COMBINE_INF::PacketData, padding01);
constexpr auto combineInfPacketDataSize   = sizeof(GP_SERV_COMMAND_COMBINE_INF::PacketData);
constexpr auto combineInfPacketSize       = sizeof(GP_SERV_HEADER) + combineInfPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c combine packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c combine packet self-test failed: " << label << " got";
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
            std::cerr << "s2c combine packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid, std::string name)
{
    character.id    = id;
    character.targid = targid;
    character.name   = std::move(name);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(combineAnsPacketDataSize, 52, "sizeof(COMBINE_ANS::PacketData)") && ok;
    ok      = expectEqualUInt(combineAnsPacketSize, 56, "COMBINE_ANS packet size") && ok;
    ok      = expectEqualUInt(combineAnsResultOffset, 4, "COMBINE_ANS Result offset") && ok;
    ok      = expectEqualUInt(combineAnsGradeOffset, 5, "COMBINE_ANS Grade offset") && ok;
    ok      = expectEqualUInt(combineAnsCountOffset, 6, "COMBINE_ANS Count offset") && ok;
    ok      = expectEqualUInt(combineAnsPadding00Offset, 7, "COMBINE_ANS padding00 offset") && ok;
    ok      = expectEqualUInt(combineAnsItemNoOffset, 8, "COMBINE_ANS ItemNo offset") && ok;
    ok      = expectEqualUInt(combineAnsBreakNoOffset, 10, "COMBINE_ANS BreakNo offset") && ok;
    ok      = expectEqualUInt(combineAnsUpKindOffset, 26, "COMBINE_ANS UpKind offset") && ok;
    ok      = expectEqualUInt(combineAnsUpLevelOffset, 30, "COMBINE_ANS UpLevel offset") && ok;
    ok      = expectEqualUInt(combineAnsCrystalNoOffset, 34, "COMBINE_ANS CrystalNo offset") && ok;
    ok      = expectEqualUInt(combineAnsMaterialNoOffset, 36, "COMBINE_ANS MaterialNo offset") && ok;
    ok      = expectEqualUInt(combineAnsPadding01Offset, 52, "COMBINE_ANS padding01 offset") && ok;
    ok      = expectEqualUInt(combineInfPacketDataSize, 44, "sizeof(COMBINE_INF::PacketData)") && ok;
    ok      = expectEqualUInt(combineInfPacketSize, 48, "COMBINE_INF packet size") && ok;
    ok      = expectEqualUInt(combineInfResultOffset, 4, "COMBINE_INF Result offset") && ok;
    ok      = expectEqualUInt(combineInfGradeOffset, 5, "COMBINE_INF Grade offset") && ok;
    ok      = expectEqualUInt(combineInfCountOffset, 6, "COMBINE_INF Count offset") && ok;
    ok      = expectEqualUInt(combineInfPadding00Offset, 7, "COMBINE_INF padding00 offset") && ok;
    ok      = expectEqualUInt(combineInfItemNoOffset, 8, "COMBINE_INF ItemNo offset") && ok;
    ok      = expectEqualUInt(combineInfBreakNoOffset, 10, "COMBINE_INF BreakNo offset") && ok;
    ok      = expectEqualUInt(combineInfUniqueNoOffset, 26, "COMBINE_INF UniqueNo offset") && ok;
    ok      = expectEqualUInt(combineInfActIndexOffset, 28, "COMBINE_INF ActIndex offset") && ok;
    ok      = expectEqualUInt(combineInfNameOffset, 30, "COMBINE_INF name offset") && ok;
    ok      = expectEqualUInt(combineInfPadding01Offset, 46, "COMBINE_INF padding01 offset") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(SynthesisResult::Success), 0, "SynthesisResult::Success") && ok;
    return ok;
}

auto testCombineAnsConstructor() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, "CrafterName123456789");
    auto packet = GP_SERV_COMMAND_COMBINE_ANS(&character, SynthesisResult::Success, CCraftState::Result{ .itemId = 0x2345, .qty = 3 });
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x06F, "COMBINE_ANS type") && ok;
    ok      = expectEqualUInt(packet.getSize(), combineAnsPacketSize, "COMBINE_ANS size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 10>{ 0x6F, 0x1C, 0xEF, 0xBE, 0x00, 0x00, 0x03, 0x00, 0x45, 0x23 }, "COMBINE_ANS constructor prefix") && ok;
    ok      = expectZeroTail(packet, 10, "COMBINE_ANS tail") && ok;
    return ok;
}

auto testCombineInfConstructor() -> bool
{
    auto character = CCharEntity{};
    makeChar(character, 0x11223344, 0x5566, "CrafterName123456789");
    auto packet = GP_SERV_COMMAND_COMBINE_INF(&character, SynthesisResult::Success, CCraftState::Result{ .itemId = 0x2345, .qty = 3 });
    packet.setSequence(0xBEEF);

    const auto expectedPrefix = std::array<uint8, 46>{
        0x70, 0x18, 0xEF, 0xBE,
        0x00, 0x00, 0x03, 0x00,
        0x45, 0x23,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x44, 0x33, 0x66, 0x55,
        'C', 'r', 'a', 'f', 't', 'e', 'r', 'N',
        'a', 'm', 'e', '1', '2', '3', '4', '5',
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x070, "COMBINE_INF type") && ok;
    ok      = expectEqualUInt(packet.getSize(), combineInfPacketSize, "COMBINE_INF size") && ok;
    ok      = expectBytes(packet, 0, expectedPrefix, "COMBINE_INF constructor prefix") && ok;
    ok      = expectZeroTail(packet, combineInfPacketSize, "COMBINE_INF tail") && ok;
    return ok;
}

} // namespace

auto runS2CCombinePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testCombineAnsConstructor() && ok;
    ok      = testCombineInfConstructor() && ok;
    return ok;
}
