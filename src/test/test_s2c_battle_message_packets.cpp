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

#include "test_s2c_battle_message_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/base_entity.h"
#include "map/enums/msg_basic.h"
#include "map/enums/msg_std.h"
#include "map/packets/s2c/0x029_battle_message.h"
#include "map/packets/s2c/0x02d_battle_message2.h"

namespace
{

constexpr auto battleMsgUniqueNoCasOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, UniqueNoCas);
constexpr auto battleMsgUniqueNoTarOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, UniqueNoTar);
constexpr auto battleMsgDataOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, Data);
constexpr auto battleMsgData2Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, Data2);
constexpr auto battleMsgActIndexCasOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, ActIndexCas);
constexpr auto battleMsgActIndexTarOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, ActIndexTar);
constexpr auto battleMsgMessageNumOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, MessageNum);
constexpr auto battleMsgTypeOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, Type);
constexpr auto battleMsgPadding1BOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData, padding1B);
constexpr auto battleMsgPacketSize        = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData);

constexpr auto battleMsg2UniqueNoCasOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, UniqueNoCas);
constexpr auto battleMsg2UniqueNoTarOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, UniqueNoTar);
constexpr auto battleMsg2ActIndexCasOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, ActIndexCas);
constexpr auto battleMsg2ActIndexTarOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, ActIndexTar);
constexpr auto battleMsg2DataOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, Data);
constexpr auto battleMsg2Data2Offset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, Data2);
constexpr auto battleMsg2MessageNumOffset  = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, MessageNum);
constexpr auto battleMsg2TypeOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, Type);
constexpr auto battleMsg2Padding1BOffset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData, padding1B);
constexpr auto battleMsg2PacketSize        = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData);

struct EntityStorage
{
    alignas(CBaseEntity) std::array<std::byte, sizeof(CBaseEntity)> bytes{};

    auto entity() -> CBaseEntity*
    {
        return reinterpret_cast<CBaseEntity*>(bytes.data());
    }
};

template <typename T>
void writeEntityField(EntityStorage& storage, std::size_t offset, const T& value)
{
    std::memcpy(storage.bytes.data() + offset, &value, sizeof(value));
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

auto makeEntity(std::uint32_t id, std::uint16_t targid) -> EntityStorage
{
    auto storage = EntityStorage{};
    writeEntityField(storage, offsetof(CBaseEntity, id), id);
    writeEntityField(storage, offsetof(CBaseEntity, targid), targid);
    return storage;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c battle message packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c battle message packet self-test failed: " << label << " got";
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
            std::cerr << "s2c battle message packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testBattleMessageLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_BATTLE_MESSAGE::PacketData), 24, "sizeof(BATTLE_MESSAGE PacketData)") && ok;
    ok      = expectEqualUInt(battleMsgPacketSize, 28, "BATTLE_MESSAGE packet size") && ok;
    ok      = expectEqualUInt(battleMsgUniqueNoCasOffset, 4, "BATTLE_MESSAGE UniqueNoCas offset") && ok;
    ok      = expectEqualUInt(battleMsgUniqueNoTarOffset, 8, "BATTLE_MESSAGE UniqueNoTar offset") && ok;
    ok      = expectEqualUInt(battleMsgDataOffset, 12, "BATTLE_MESSAGE Data offset") && ok;
    ok      = expectEqualUInt(battleMsgData2Offset, 16, "BATTLE_MESSAGE Data2 offset") && ok;
    ok      = expectEqualUInt(battleMsgActIndexCasOffset, 20, "BATTLE_MESSAGE ActIndexCas offset") && ok;
    ok      = expectEqualUInt(battleMsgActIndexTarOffset, 22, "BATTLE_MESSAGE ActIndexTar offset") && ok;
    ok      = expectEqualUInt(battleMsgMessageNumOffset, 24, "BATTLE_MESSAGE MessageNum offset") && ok;
    ok      = expectEqualUInt(battleMsgTypeOffset, 26, "BATTLE_MESSAGE Type offset") && ok;
    ok      = expectEqualUInt(battleMsgPadding1BOffset, 27, "BATTLE_MESSAGE padding1B offset") && ok;
    return ok;
}

auto testBattleMessageConstructor() -> bool
{
    auto sender = makeEntity(0x11223344, 0x5566);
    auto target = makeEntity(0xAABBCCDD, 0xEEFF);
    auto packet = GP_SERV_COMMAND_BATTLE_MESSAGE(sender.entity(), target.entity(), -1, -1599029040, MsgStd::DiceRoll);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x029, "BATTLE_MESSAGE type") && ok;
    ok      = expectEqualUInt(packet.getSize(), battleMsgPacketSize, "BATTLE_MESSAGE size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x29, 0x07, 0xEF, 0xBE }, "BATTLE_MESSAGE header") && ok;
    ok      = expectBytes(packet, battleMsgUniqueNoCasOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "BATTLE_MESSAGE UniqueNoCas") && ok;
    ok      = expectBytes(packet, battleMsgUniqueNoTarOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "BATTLE_MESSAGE UniqueNoTar") && ok;
    ok      = expectBytes(packet, battleMsgDataOffset, std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "BATTLE_MESSAGE Data") && ok;
    ok      = expectBytes(packet, battleMsgData2Offset, std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "BATTLE_MESSAGE Data2") && ok;
    ok      = expectBytes(packet, battleMsgActIndexCasOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "BATTLE_MESSAGE ActIndexCas") && ok;
    ok      = expectBytes(packet, battleMsgActIndexTarOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "BATTLE_MESSAGE ActIndexTar") && ok;
    ok      = expectBytes(packet, battleMsgMessageNumOffset, std::array<uint8, 2>{ 0x58, 0x00 }, "BATTLE_MESSAGE MessageNum") && ok;
    ok      = expectZeroRange(packet, battleMsgTypeOffset, battleMsgPacketSize, "BATTLE_MESSAGE Type/padding") && ok;
    ok      = expectZeroRange(packet, battleMsgPacketSize, PACKET_SIZE, "BATTLE_MESSAGE tail") && ok;
    return ok;
}

auto testBattleMessageBasicConstructor() -> bool
{
    auto sender = makeEntity(0x01020304, 0x0506);
    auto target = makeEntity(0x0708090A, 0x0B0C);
    auto packet = GP_SERV_COMMAND_BATTLE_MESSAGE(sender.entity(), target.entity(), 1, 2, MsgBasic::ExperiencePointsGained);

    bool ok = true;
    ok      = expectBytes(packet, battleMsgMessageNumOffset, std::array<uint8, 2>{ 0x08, 0x00 }, "BATTLE_MESSAGE basic MessageNum") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(packet.getMessageId()), static_cast<std::uint16_t>(MsgBasic::ExperiencePointsGained), "BATTLE_MESSAGE getMessageId") && ok;
    return ok;
}

auto testBattleMessage2Layout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_BATTLE_MESSAGE2::PacketData), 24, "sizeof(BATTLE_MESSAGE2 PacketData)") && ok;
    ok      = expectEqualUInt(battleMsg2PacketSize, 28, "BATTLE_MESSAGE2 packet size") && ok;
    ok      = expectEqualUInt(battleMsg2UniqueNoCasOffset, 4, "BATTLE_MESSAGE2 UniqueNoCas offset") && ok;
    ok      = expectEqualUInt(battleMsg2UniqueNoTarOffset, 8, "BATTLE_MESSAGE2 UniqueNoTar offset") && ok;
    ok      = expectEqualUInt(battleMsg2ActIndexCasOffset, 12, "BATTLE_MESSAGE2 ActIndexCas offset") && ok;
    ok      = expectEqualUInt(battleMsg2ActIndexTarOffset, 14, "BATTLE_MESSAGE2 ActIndexTar offset") && ok;
    ok      = expectEqualUInt(battleMsg2DataOffset, 16, "BATTLE_MESSAGE2 Data offset") && ok;
    ok      = expectEqualUInt(battleMsg2Data2Offset, 20, "BATTLE_MESSAGE2 Data2 offset") && ok;
    ok      = expectEqualUInt(battleMsg2MessageNumOffset, 24, "BATTLE_MESSAGE2 MessageNum offset") && ok;
    ok      = expectEqualUInt(battleMsg2TypeOffset, 26, "BATTLE_MESSAGE2 Type offset") && ok;
    ok      = expectEqualUInt(battleMsg2Padding1BOffset, 27, "BATTLE_MESSAGE2 padding1B offset") && ok;
    return ok;
}

auto testBattleMessage2Constructor() -> bool
{
    auto sender = makeEntity(0x11223344, 0x5566);
    auto target = makeEntity(0xAABBCCDD, 0xEEFF);
    auto packet = GP_SERV_COMMAND_BATTLE_MESSAGE2(sender.entity(), target.entity(), -1, -1599029040, MsgBasic::ExperiencePointsGained);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x02D, "BATTLE_MESSAGE2 type") && ok;
    ok      = expectEqualUInt(packet.getSize(), battleMsg2PacketSize, "BATTLE_MESSAGE2 size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x2D, 0x07, 0xEF, 0xBE }, "BATTLE_MESSAGE2 header") && ok;
    ok      = expectBytes(packet, battleMsg2UniqueNoCasOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "BATTLE_MESSAGE2 UniqueNoCas") && ok;
    ok      = expectBytes(packet, battleMsg2UniqueNoTarOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "BATTLE_MESSAGE2 UniqueNoTar") && ok;
    ok      = expectBytes(packet, battleMsg2ActIndexCasOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "BATTLE_MESSAGE2 ActIndexCas") && ok;
    ok      = expectBytes(packet, battleMsg2ActIndexTarOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "BATTLE_MESSAGE2 ActIndexTar") && ok;
    ok      = expectBytes(packet, battleMsg2DataOffset, std::array<uint8, 4>{ 0xFF, 0xFF, 0xFF, 0xFF }, "BATTLE_MESSAGE2 Data") && ok;
    ok      = expectBytes(packet, battleMsg2Data2Offset, std::array<uint8, 4>{ 0xD0, 0xC0, 0xB0, 0xA0 }, "BATTLE_MESSAGE2 Data2") && ok;
    ok      = expectBytes(packet, battleMsg2MessageNumOffset, std::array<uint8, 2>{ 0x08, 0x00 }, "BATTLE_MESSAGE2 MessageNum") && ok;
    ok      = expectZeroRange(packet, battleMsg2TypeOffset, battleMsg2PacketSize, "BATTLE_MESSAGE2 Type/padding") && ok;
    ok      = expectZeroRange(packet, battleMsg2PacketSize, PACKET_SIZE, "BATTLE_MESSAGE2 tail") && ok;
    return ok;
}

} // namespace

auto runS2CBattleMessagePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testBattleMessageLayout() && ok;
    ok      = testBattleMessageConstructor() && ok;
    ok      = testBattleMessageBasicConstructor() && ok;
    ok      = testBattleMessage2Layout() && ok;
    ok      = testBattleMessage2Constructor() && ok;
    return ok;
}
