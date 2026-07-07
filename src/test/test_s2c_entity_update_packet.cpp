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

#include "test_s2c_entity_update_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/mob_entity.h"
#include "entities/npc_entity.h"
#include "map/packets/entity_update.h"

namespace
{

constexpr auto entityUpdateDefaultSize     = 0x58;
constexpr auto entityUpdateCompactSize     = 0x48;
constexpr auto entityUpdateRenamedSize     = 0x58;
constexpr auto entityUpdateUniqueNoOffset  = 0x04;
constexpr auto entityUpdateActIndexOffset  = 0x08;
constexpr auto entityUpdateSendFlagsOffset = 0x0A;
constexpr auto entityUpdateDirOffset       = 0x0B;
constexpr auto entityUpdateXOffset         = 0x0C;
constexpr auto entityUpdateZOffset         = 0x10;
constexpr auto entityUpdateYOffset         = 0x14;
constexpr auto entityUpdateFlags0Offset    = 0x18;
constexpr auto entityUpdateSpeedOffset     = 0x1C;
constexpr auto entityUpdateSpeedBaseOffset = 0x1D;
constexpr auto entityUpdateHPPOffset       = 0x1E;
constexpr auto entityUpdateAnimationOffset = 0x1F;
constexpr auto entityUpdateStatusOffset    = 0x20;
constexpr auto entityUpdateFlags1Offset    = 0x21;
constexpr auto entityUpdateFlags2Offset    = 0x25;
constexpr auto entityUpdateFlags3Offset    = 0x29;
constexpr auto entityUpdateOwnerIDOffset   = 0x2C;
constexpr auto entityUpdateSubKindOffset   = 0x30;
constexpr auto entityUpdateDataOffset      = 0x34;
constexpr auto entityUpdateRenamedOffset   = 0x44;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateNPC(CNpcEntity& entity)
{
    entity.id             = 0x10203040;
    entity.targid         = 0x0123;
    entity.name           = "GateGuard";
    entity.loc.p.rotation = 0x44;
    entity.loc.p.x        = 12.5f;
    entity.loc.p.y        = -6.25f;
    entity.loc.p.z        = 3.75f;
    entity.loc.p.moving   = 0x0456;
    entity.m_TargID       = 0x01AB;
    entity.baseSpeed      = 35;
    entity.UpdateSpeed(false);
    entity.animationSpeed = 17;
    entity.animation      = 0x09;
    entity.animationsub   = 0x03;
    entity.status         = STATUS_TYPE::NORMAL;
    entity.m_flags        = 0x00C0FFEE;
    entity.name_prefix    = 0x05;
    entity.allegiance     = ALLEGIANCE_TYPE::WINDURST;
    entity.namevis        = 0x80;
    entity.look.size      = MODEL_STANDARD;
    entity.look.modelid   = 0x2222;
}

void populateMob(CMobEntity& entity)
{
    entity.id              = 0x55667788;
    entity.targid          = 0x0789;
    entity.name            = "MobBase";
    entity.packetName      = "RenamedMob";
    entity.loc.p.rotation  = 0x7E;
    entity.loc.p.x         = -1.5f;
    entity.loc.p.y         = 2.25f;
    entity.loc.p.z         = 9.5f;
    entity.loc.p.moving    = 0x0121;
    entity.m_TargID        = 0x0444;
    entity.baseSpeed       = 28;
    entity.UpdateSpeed(false);
    entity.animationSpeed  = 14;
    entity.animation       = ANIMATION_ATTACK;
    entity.animationsub    = 0x02;
    entity.status          = STATUS_TYPE::NORMAL;
    entity.health.hp       = 333;
    entity.health.modhp    = 999;
    entity.m_flags         = 0x0000AA55;
    entity.m_name_prefix   = 0x06;
    entity.allegiance      = ALLEGIANCE_TYPE::MOB;
    entity.namevis         = 0x08;
    entity.look.size       = MODEL_EQUIPPED;
    entity.look.face       = 0x12;
    entity.look.race       = 0x04;
    entity.look.head       = 0x0101;
    entity.look.body       = 0x0202;
    entity.look.hands      = 0x0303;
    entity.look.legs       = 0x0404;
    entity.look.feet       = 0x0505;
    entity.look.main       = 0x0606;
    entity.look.sub        = 0x0707;
    entity.look.ranged     = 0x0808;
    entity.modelSize       = 2;
    entity.modelHitboxSize = 3.5f;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ENTITY_UPDATE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ENTITY_UPDATE packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ENTITY_UPDATE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testNPCSpawnConstructor() -> bool
{
    auto entity = CNpcEntity{};
    populateNPC(entity);

    auto packet = CEntityUpdatePacket(&entity, ENTITY_SPAWN, UPDATE_ALL_MOB);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x00E, "npc spawn type") && ok;
    ok      = expectEqualUInt(packet.getSize(), entityUpdateCompactSize, "npc spawn size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0E, 0x24, 0xEF, 0xBE }, "npc spawn header") && ok;
    ok      = expectBytes(packet, entityUpdateUniqueNoOffset, std::array<uint8, 4>{ 0x40, 0x30, 0x20, 0x10 }, "npc spawn unique no") && ok;
    ok      = expectBytes(packet, entityUpdateActIndexOffset, std::array<uint8, 2>{ 0x23, 0x01 }, "npc spawn act index") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSendFlagsOffset], 0x0F, "npc spawn send flags") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateDirOffset], 0x44, "npc spawn dir") && ok;
    ok      = expectBytes(packet, entityUpdateXOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x48, 0x41 }, "npc spawn x") && ok;
    ok      = expectBytes(packet, entityUpdateZOffset, std::array<uint8, 4>{ 0x00, 0x00, 0xC8, 0xC0 }, "npc spawn z from internal y") && ok;
    ok      = expectBytes(packet, entityUpdateYOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x70, 0x40 }, "npc spawn y from internal z") && ok;
    ok      = expectBytes(packet, entityUpdateFlags0Offset, std::array<uint8, 4>{ 0x56, 0x04, 0x56, 0x03 }, "npc spawn flags0") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSpeedOffset], 35, "npc spawn speed") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSpeedBaseOffset], 17, "npc spawn speed base") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateHPPOffset], 100, "npc spawn HPP") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateAnimationOffset], 0x09, "npc spawn animation") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateStatusOffset], 0, "npc spawn status") && ok;
    ok      = expectBytes(packet, entityUpdateFlags1Offset, std::array<uint8, 4>{ 0xEE, 0xFF, 0xC0, 0x00 }, "npc spawn flags1") && ok;
    ok      = expectBytes(packet, entityUpdateFlags2Offset, std::array<uint8, 4>{ 0x00, 0x00, 0x05, 0x00 }, "npc spawn flags2") && ok;
    ok      = expectBytes(packet, entityUpdateFlags3Offset, std::array<uint8, 3>{ 0x04, 0x07, 0x80 }, "npc spawn flags3") && ok;
    ok      = expectZeroRange(packet, entityUpdateOwnerIDOffset, entityUpdateSubKindOffset, "npc spawn owner id") && ok;
    ok      = expectBytes(packet, entityUpdateSubKindOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x22, 0x22 }, "npc spawn model") && ok;
    ok      = expectBytes(packet, entityUpdateDataOffset, std::array<uint8, 9>{ 'G', 'a', 't', 'e', 'G', 'u', 'a', 'r', 'd' }, "npc spawn name") && ok;
    ok      = expectZeroRange(packet, entityUpdateDataOffset + 9, PACKET_SIZE, "npc spawn tail") && ok;
    return ok;
}

auto testMobEquippedRenamedSpawn() -> bool
{
    auto entity = CMobEntity{};
    populateMob(entity);
    entity.isRenamed = true;

    auto packet = CEntityUpdatePacket(&entity, ENTITY_SPAWN, UPDATE_ALL_MOB);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), entityUpdateRenamedSize, "renamed spawn size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0E, 0x2C, 0x00, 0x00 }, "renamed spawn header") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSendFlagsOffset], 0x57, "renamed spawn send flags") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateFlags0Offset], 0x01, "renamed spawn long-name marker") && ok;
    ok      = expectBytes(packet, entityUpdateSubKindOffset, std::array<uint8, 20>{ 0x01, 0x00, 0x12, 0x04, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x08, 0x08 }, "renamed spawn look") && ok;
    ok      = expectBytes(packet, entityUpdateRenamedOffset, std::array<uint8, 10>{ 'R', 'e', 'n', 'a', 'm', 'e', 'd', 'M', 'o', 'b' }, "renamed spawn name") && ok;
    ok      = expectZeroRange(packet, entityUpdateRenamedOffset + 10, PACKET_SIZE, "renamed spawn tail") && ok;
    return ok;
}

auto testMobRenamedUpdate() -> bool
{
    auto entity = CMobEntity{};
    populateMob(entity);
    entity.isRenamed = true;
    entity.targid    = 0x0123;

    auto packet = CEntityUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_HP);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), entityUpdateCompactSize, "renamed update size") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSendFlagsOffset], UPDATE_HP | UPDATE_NAME, "renamed update send flags") && ok;
    ok      = expectBytes(packet, entityUpdateHPPOffset, std::array<uint8, 2>{ 0x21, 0x01 }, "renamed update HPP/animation") && ok;
    ok      = expectBytes(packet, entityUpdateFlags1Offset, std::array<uint8, 4>{ 0x55, 0xAA, 0x00, 0x00 }, "renamed update flags1") && ok;
    ok      = expectBytes(packet, entityUpdateFlags2Offset, std::array<uint8, 4>{ 0x23, 0x00, 0x06, 0x40 }, "renamed update flags2") && ok;
    ok      = expectBytes(packet, entityUpdateDataOffset, std::array<uint8, 1>{ 0x01 }, "renamed update low-targid marker") && ok;
    ok      = expectBytes(packet, entityUpdateDataOffset + 1, std::array<uint8, 10>{ 'R', 'e', 'n', 'a', 'm', 'e', 'd', 'M', 'o', 'b' }, "renamed update name") && ok;
    ok      = expectZeroRange(packet, entityUpdateDataOffset + 11, PACKET_SIZE, "renamed update tail") && ok;
    return ok;
}

auto testMobGraphSizeAndPlayerUpdateStatus() -> bool
{
    auto entity = CMobEntity{};
    populateMob(entity);
    entity.status     = STATUS_TYPE::UPDATE;
    entity.allegiance = ALLEGIANCE_TYPE::PLAYER;
    entity.m_flags    = 0;

    auto packet = CEntityUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_HP);

    bool ok = true;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateStatusOffset], static_cast<uint8>(STATUS_TYPE::NORMAL), "graph/status normalized status") && ok;
    ok      = expectBytes(packet, entityUpdateFlags1Offset, std::array<uint8, 4>{ 0x04, 0x00, 0x00, 0x00 }, "graph/status flags1") && ok;
    ok      = expectBytes(packet, entityUpdateFlags2Offset, std::array<uint8, 4>{ 0x23, 0x00, 0x06, 0x00 }, "graph/status flags2") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateFlags3Offset], static_cast<uint8>(ALLEGIANCE_TYPE::PLAYER), "graph/status allegiance") && ok;
    return ok;
}

auto testRenamedMobNameTruncation() -> bool
{
    auto entity = CMobEntity{};
    populateMob(entity);
    entity.isRenamed = true;
    entity.targid    = 0x0123;
    entity.packetName = "SixteenByteName++";

    auto packet = CEntityUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_HP);

    bool ok = true;
    ok      = expectBytes(packet, entityUpdateDataOffset, std::array<uint8, 1>{ 0x01 }, "truncated rename low-targid marker") && ok;
    ok      = expectBytes(packet, entityUpdateDataOffset + 1, std::array<uint8, 16>{ 'S', 'i', 'x', 't', 'e', 'e', 'n', 'B', 'y', 't', 'e', 'N', 'a', 'm', 'e', '+' }, "truncated rename name") && ok;
    ok      = expectZeroRange(packet, entityUpdateDataOffset + 17, PACKET_SIZE, "truncated rename tail") && ok;
    return ok;
}

auto testDoorNameTruncation() -> bool
{
    auto entity = CNpcEntity{};
    populateNPC(entity);
    entity.name      = "VeryLongDoorName";
    entity.look.size = MODEL_DOOR;

    auto packet = CEntityUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_NONE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), entityUpdateCompactSize, "door truncation size") && ok;
    ok      = expectBytes(packet, entityUpdateSubKindOffset, std::array<uint8, 2>{ 0x02, 0x00 }, "door truncation subkind") && ok;
    ok      = expectBytes(packet, entityUpdateDataOffset, std::array<uint8, 12>{ 'V', 'e', 'r', 'y', 'L', 'o', 'n', 'g', 'D', 'o', 'o', 'r' }, "door truncation name") && ok;
    ok      = expectZeroRange(packet, entityUpdateDataOffset + 12, PACKET_SIZE, "door truncation tail") && ok;
    return ok;
}

auto testMobDespawnConstructor() -> bool
{
    auto entity = CMobEntity{};
    populateMob(entity);

    auto packet = CEntityUpdatePacket(&entity, ENTITY_DESPAWN, UPDATE_NONE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), entityUpdateCompactSize, "despawn size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0E, 0x24, 0x00, 0x00 }, "despawn header") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateSendFlagsOffset], 0x30, "despawn send flags") && ok;
    ok      = expectBytes(packet, entityUpdateUniqueNoOffset, std::array<uint8, 4>{ 0x88, 0x77, 0x66, 0x55 }, "despawn unique no") && ok;
    ok      = expectEqualUInt(packetData(packet)[entityUpdateAnimationOffset], ANIMATION_ATTACK, "despawn animation") && ok;
    ok      = expectBytes(packet, entityUpdateSubKindOffset, std::array<uint8, 20>{ 0x01, 0x00, 0x12, 0x04, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x08, 0x08 }, "despawn look") && ok;
    ok      = expectZeroRange(packet, entityUpdateDataOffset + 20, PACKET_SIZE, "despawn tail") && ok;
    return ok;
}

} // namespace

auto runS2CEntityUpdatePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(entityUpdateDefaultSize, 0x58, "default size") && ok;
    ok      = expectEqualUInt(entityUpdateCompactSize, 0x48, "compact size") && ok;
    ok      = expectEqualUInt(entityUpdateRenamedSize, 0x58, "renamed size") && ok;
    ok      = testNPCSpawnConstructor() && ok;
    ok      = testMobEquippedRenamedSpawn() && ok;
    ok      = testMobRenamedUpdate() && ok;
    ok      = testMobGraphSizeAndPlayerUpdateStatus() && ok;
    ok      = testRenamedMobNameTruncation() && ok;
    ok      = testDoorNameTruncation() && ok;
    ok      = testMobDespawnConstructor() && ok;
    return ok;
}
