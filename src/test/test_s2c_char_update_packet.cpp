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

#include "test_s2c_char_update_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "items/item_linkshell.h"
#include "map/packets/char_update.h"

namespace
{

constexpr auto charUpdateNonspecificSize    = 0x3E;
constexpr auto charUpdateGeneralSize        = 0x48;
constexpr auto charUpdateModelSize          = 0x5A;
constexpr auto charUpdateUniqueNoOffset     = 0x04;
constexpr auto charUpdateActIndexOffset     = 0x08;
constexpr auto charUpdateSendFlagsOffset    = 0x0A;
constexpr auto charUpdateDirOffset          = 0x0B;
constexpr auto charUpdateXOffset            = 0x0C;
constexpr auto charUpdateZOffset            = 0x10;
constexpr auto charUpdateYOffset            = 0x14;
constexpr auto charUpdateFlags0Offset       = 0x18;
constexpr auto charUpdateSpeedOffset        = 0x1C;
constexpr auto charUpdateSpeedBaseOffset    = 0x1D;
constexpr auto charUpdateHPPOffset          = 0x1E;
constexpr auto charUpdateServerStatusOffset = 0x1F;
constexpr auto charUpdateFlags1Offset       = 0x20;
constexpr auto charUpdateFlags2Offset       = 0x24;
constexpr auto charUpdateFlags3Offset       = 0x28;
constexpr auto charUpdateBtTargetIDOffset   = 0x2C;
constexpr auto charUpdateCostumeIDOffset    = 0x30;
constexpr auto charUpdateBallistaInfoOffset = 0x32;
constexpr auto charUpdateFlags4Offset       = 0x33;
constexpr auto charUpdateCustomPropsOffset  = 0x34;
constexpr auto charUpdatePetActIndexOffset  = 0x3C;
constexpr auto charUpdateMonstrosityOffset  = 0x3E;
constexpr auto charUpdateFlags5Offset       = 0x42;
constexpr auto charUpdateHitboxOffset       = 0x43;
constexpr auto charUpdateFlags6Offset       = 0x44;
constexpr auto charUpdateGrapIDOffset       = 0x48;
constexpr auto charUpdateNameOffset         = 0x5A;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateCharacter(CCharEntity& entity)
{
    entity.id        = 0x11223344;
    entity.targid    = 0x5566;
    entity.name      = "OmegaOne";
    entity.m_TargID  = 0x0345;
    entity.m_Costume = 0x3344;
    entity.m_mountId = 7;

    entity.loc.p.rotation = 0x7F;
    entity.loc.p.x        = 1.5f;
    entity.loc.p.y        = 2.5f;
    entity.loc.p.z        = -3.25f;
    entity.loc.p.moving   = 0x0123;

    entity.baseSpeed       = 40;
    entity.animationSpeed  = 22;
    entity.animation       = 0x1B;
    entity.health.hp       = 750;
    entity.health.modhp    = 1000;
    entity.modelHitboxSize = 2.5f;
    entity.visibleGmLevel  = 5;
    entity.wallhackEnabled = true;
    entity.isLinkDead      = true;
    entity.isCharmed       = true;
    entity.priorityRender  = true;
    entity.m_isGMHidden    = true;
    entity.allegiance      = ALLEGIANCE_TYPE::GRIFFONS;

    entity.look.size  = 2;
    entity.look.face  = 0x22;
    entity.look.race  = 5;
    entity.look.head  = 3;
    entity.look.body  = 4;
    entity.look.hands = 5;
    entity.look.legs  = 6;
    entity.look.feet  = 7;
    entity.look.main  = 8;
    entity.look.sub   = 9;
    entity.look.ranged = 10;

    entity.playerConfig.InviteFlg          = 1;
    entity.playerConfig.AwayFlg            = 1;
    entity.playerConfig.AnonymityFlg       = 1;
    entity.playerConfig.MentorFlg          = 1;
    entity.playerConfig.NewAdventurerOffFlg = 0;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CHAR_UPDATE packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CHAR_UPDATE packet self-test failed: " << label << " got";
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
            std::cerr << "s2c CHAR_UPDATE packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(charUpdateNonspecificSize, 62, "nonspecific size") && ok;
    ok      = expectEqualUInt(charUpdateGeneralSize, 72, "general size") && ok;
    ok      = expectEqualUInt(charUpdateModelSize, 90, "model/name offset size") && ok;
    ok      = expectEqualUInt(charUpdateUniqueNoOffset, 4, "unique no offset") && ok;
    ok      = expectEqualUInt(charUpdateSendFlagsOffset, 10, "send flags offset") && ok;
    ok      = expectEqualUInt(charUpdateFlags0Offset, 24, "flags0 offset") && ok;
    ok      = expectEqualUInt(charUpdateFlags1Offset, 32, "flags1 offset") && ok;
    ok      = expectEqualUInt(charUpdateFlags2Offset, 36, "flags2 offset") && ok;
    ok      = expectEqualUInt(charUpdateFlags3Offset, 40, "flags3 offset") && ok;
    ok      = expectEqualUInt(charUpdateGrapIDOffset, 72, "GrapID offset") && ok;
    ok      = expectEqualUInt(charUpdateNameOffset, 90, "name offset") && ok;
    return ok;
}

auto testSpawnConstructor() -> bool
{
    auto entity = CCharEntity{};
    auto pet    = CCharEntity{};
    populateCharacter(entity);
    pet.targid  = 0x7788;
    entity.PPet = &pet;

    auto packet = CCharUpdatePacket(&entity, ENTITY_SPAWN, UPDATE_ALL_CHAR);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x00D, "spawn type") && ok;
    ok      = expectEqualUInt(packet.getSize(), 0x68, "spawn size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0D, 0x34, 0xEF, 0xBE }, "spawn header") && ok;
    ok      = expectBytes(packet, charUpdateUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "spawn unique no") && ok;
    ok      = expectBytes(packet, charUpdateActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "spawn act index") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateSendFlagsOffset], 0x1F, "spawn send flags") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateDirOffset], 0x7F, "spawn dir") && ok;
    ok      = expectBytes(packet, charUpdateXOffset, std::array<uint8, 4>{ 0x00, 0x00, 0xC0, 0x3F }, "spawn x") && ok;
    ok      = expectBytes(packet, charUpdateZOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x20, 0x40 }, "spawn z from internal y") && ok;
    ok      = expectBytes(packet, charUpdateYOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x50, 0xC0 }, "spawn y from internal z") && ok;
    ok      = expectBytes(packet, charUpdateFlags0Offset, std::array<uint8, 4>{ 0x23, 0x81, 0x8A, 0x06 }, "spawn flags0") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateSpeedOffset], 40, "spawn speed") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateSpeedBaseOffset], 22, "spawn speed base") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateHPPOffset], 75, "spawn HPP") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateServerStatusOffset], 0x1B, "spawn server status") && ok;
    ok      = expectBytes(packet, charUpdateFlags1Offset, std::array<uint8, 4>{ 0x00, 0xDC, 0x04, 0x2D }, "spawn flags1") && ok;
    ok      = expectBytes(packet, charUpdateFlags2Offset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x08 }, "spawn flags2") && ok;
    ok      = expectBytes(packet, charUpdateFlags3Offset, std::array<uint8, 4>{ 0x20, 0x06, 0xA0, 0x01 }, "spawn flags3") && ok;
    ok      = expectZeroRange(packet, charUpdateBtTargetIDOffset, charUpdateCostumeIDOffset, "spawn battle target") && ok;
    ok      = expectBytes(packet, charUpdateCostumeIDOffset, std::array<uint8, 2>{ 0x44, 0x33 }, "spawn costume") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateBallistaInfoOffset], 0, "spawn ballista info") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateFlags4Offset], 0, "spawn flags4") && ok;
    ok      = expectZeroRange(packet, charUpdateCustomPropsOffset, charUpdatePetActIndexOffset, "spawn custom props") && ok;
    ok      = expectBytes(packet, charUpdatePetActIndexOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "spawn pet act index") && ok;
    ok      = expectZeroRange(packet, charUpdateMonstrosityOffset, charUpdateFlags5Offset, "spawn monstrosity") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateFlags5Offset], 0x10, "spawn flags5") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateHitboxOffset], 25, "spawn hitbox") && ok;
    ok      = expectBytes(packet, charUpdateFlags6Offset, std::array<uint8, 4>{ 0x70, 0x00, 0x00, 0x00 }, "spawn flags6") && ok;
    ok      = expectBytes(packet, charUpdateGrapIDOffset, std::array<uint8, 18>{ 0x22, 0x05, 0x03, 0x10, 0x04, 0x20, 0x05, 0x30, 0x06, 0x40, 0x07, 0x50, 0x08, 0x60, 0x09, 0x70, 0x0A, 0x80 }, "spawn GrapID") && ok;
    ok      = expectBytes(packet, charUpdateNameOffset, std::array<uint8, 8>{ 'O', 'm', 'e', 'g', 'a', 'O', 'n', 'e' }, "spawn name") && ok;
    ok      = expectZeroRange(packet, charUpdateNameOffset + 8, 0x68, "spawn declared tail") && ok;
    ok      = expectZeroRange(packet, 0x68, PACKET_SIZE, "spawn packet tail") && ok;
    return ok;
}

auto testNameOnlyUpdate() -> bool
{
    auto entity = CCharEntity{};
    populateCharacter(entity);
    entity.name = "NameOnly";

    auto packet = CCharUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_NAME);
    packet.setSequence(0xCAFE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 0x68, "name-only size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0D, 0x34, 0xFE, 0xCA }, "name-only header") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateSendFlagsOffset], 0x08, "name-only send flags") && ok;
    ok      = expectBytes(packet, charUpdateUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "name-only unique no") && ok;
    ok      = expectBytes(packet, charUpdateNameOffset, std::array<uint8, 8>{ 'N', 'a', 'm', 'e', 'O', 'n', 'l', 'y' }, "name-only name") && ok;
    ok      = expectZeroRange(packet, charUpdateNameOffset + 8, PACKET_SIZE, "name-only tail") && ok;
    return ok;
}

auto testLinkshellColor() -> bool
{
    auto entity    = CCharEntity{};
    auto linkshell = CItemLinkshell{ 0x0200 };
    populateCharacter(entity);
    linkshell.SetLSColor(0x0FA1);
    entity.bindEquip(SLOT_LINK1, &linkshell);

    auto packet = CCharUpdatePacket(&entity, ENTITY_UPDATE, UPDATE_HP);

    bool ok = true;
    ok      = expectBytes(packet, charUpdateFlags1Offset, std::array<uint8, 4>{ 0x00, 0xDC, 0x06, 0x2D }, "linkshell flags1") && ok;
    ok      = expectBytes(packet, charUpdateFlags2Offset, std::array<uint8, 4>{ 0x1F, 0xAF, 0xFF, 0x08 }, "linkshell flags2 color") && ok;
    return ok;
}

auto testDespawnConstructor() -> bool
{
    auto entity = CCharEntity{};
    populateCharacter(entity);

    auto packet = CCharUpdatePacket(&entity, ENTITY_DESPAWN, UPDATE_NONE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getSize(), 0x40, "despawn size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x0D, 0x20, 0x00, 0x00 }, "despawn header") && ok;
    ok      = expectEqualUInt(packetData(packet)[charUpdateSendFlagsOffset], 0x20, "despawn send flags") && ok;
    ok      = expectBytes(packet, charUpdateUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "despawn unique no") && ok;
    ok      = expectBytes(packet, charUpdateActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "despawn act index") && ok;
    ok      = expectBytes(packet, charUpdateFlags6Offset, std::array<uint8, 4>{ 0x70, 0x00, 0x00, 0x00 }, "despawn mount index") && ok;
    ok      = expectZeroRange(packet, charUpdateSendFlagsOffset + 1, charUpdateFlags6Offset, "despawn body before mount") && ok;
    ok      = expectZeroRange(packet, charUpdateFlags6Offset + 4, PACKET_SIZE, "despawn tail") && ok;
    return ok;
}

} // namespace

auto runS2CCharUpdatePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testSpawnConstructor() && ok;
    ok      = testNameOnlyUpdate() && ok;
    ok      = testLinkshellColor() && ok;
    ok      = testDespawnConstructor() && ok;
    return ok;
}
