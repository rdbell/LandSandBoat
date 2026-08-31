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

#include "test_s2c_entity_set_name_packet.h"

#include "test/omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

#include "common/utils.h"
#include "entities/char_entity.h"
#include "entities/trust_entity.h"
#include "map/packets/entity_set_name.h"

namespace
{

constexpr auto entitySetNamePacketSize         = 0x2C;
constexpr auto entitySetNameControlOffset      = 0x04;
constexpr auto entitySetNameTargIDOffset       = 0x06;
constexpr auto entitySetNameEntityIDOffset     = 0x08;
constexpr auto entitySetNameMasterTargIDOffset = 0x0C;
constexpr auto entitySetNameUnknown10Offset    = 0x10;
constexpr auto entitySetNameNameOffset         = 0x18;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateEntity(CBaseEntity& entity, std::uint32_t id, std::uint16_t targid, std::string packetName)
{
    entity.id         = id;
    entity.targid     = targid;
    entity.packetName = std::move(packetName);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ENTITY_SET_NAME packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c ENTITY_SET_NAME packet self-test failed: " << label << " got";
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
            std::cerr << "s2c ENTITY_SET_NAME packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectPackedMessageSize(CBasicPacket& packet, std::uint16_t expected, const std::string& label) -> bool
{
    auto* data = packetData(packet);
    bool  ok   = true;
    ok         = expectEqualUInt(data[entitySetNameControlOffset] & 0x3F, 0x03, label + " low control bits") && ok;
    ok         = expectEqualUInt(unpackBitsBE(data + entitySetNameControlOffset, 0, 6, 10), expected, label + " message size") && ok;
    return ok;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(entitySetNamePacketSize - sizeof(GP_SERV_HEADER), 40, "packet data size") && ok;
    ok      = expectEqualUInt(entitySetNamePacketSize, 44, "packet size") && ok;
    ok      = expectEqualUInt(entitySetNameControlOffset, 4, "control offset") && ok;
    ok      = expectEqualUInt(entitySetNameTargIDOffset, 6, "targid offset") && ok;
    ok      = expectEqualUInt(entitySetNameEntityIDOffset, 8, "entity id offset") && ok;
    ok      = expectEqualUInt(entitySetNameMasterTargIDOffset, 12, "master targid offset") && ok;
    ok      = expectEqualUInt(entitySetNameUnknown10Offset, 16, "unknown offset") && ok;
    ok      = expectEqualUInt(entitySetNameNameOffset, 24, "name offset") && ok;
    return ok;
}

auto testNormalEntityConstructor() -> bool
{
    auto entity = CCharEntity{};
    populateEntity(entity, 0x11223344, 0x5566, "OmegaTrust");

    auto packet = CEntitySetNamePacket(&entity);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x067, "type") && ok;
    ok      = expectEqualUInt(packet.getSize(), entitySetNamePacketSize, "size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x67, 0x16, 0xEF, 0xBE }, "header") && ok;
    ok      = expectPackedMessageSize(packet, 0x18 + 10, "normal") && ok;
    ok      = expectBytes(packet, entitySetNameTargIDOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "normal targid") && ok;
    ok      = expectBytes(packet, entitySetNameEntityIDOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "normal id") && ok;
    ok      = expectBytes(packet, entitySetNameMasterTargIDOffset, std::array<uint8, 2>{ 0x00, 0x00 }, "normal master targid") && ok;
    ok      = expectBytes(packet, entitySetNameUnknown10Offset, std::array<uint8, 1>{ 0x04 }, "normal unknown") && ok;
    ok      = expectZeroRange(packet, entitySetNameUnknown10Offset + 1, entitySetNameNameOffset, "normal pre-name padding") && ok;
    ok      = expectBytes(packet, entitySetNameNameOffset, std::array<uint8, 10>{ 'O', 'm', 'e', 'g', 'a', 'T', 'r', 'u', 's', 't' }, "normal name") && ok;
    ok      = expectZeroRange(packet, entitySetNameNameOffset + 10, entitySetNamePacketSize, "normal declared tail") && ok;
    ok      = expectZeroRange(packet, entitySetNamePacketSize, PACKET_SIZE, "normal packet tail") && ok;
    return ok;
}

auto testTrustConstructorIncludesMasterTargID() -> bool
{
    auto master = CCharEntity{};
    populateEntity(master, 0x01020304, 0x3344, "Master");

    auto trust = CTrustEntity(&master, 0x777, IsPassiveTrust::No);
    populateEntity(trust, 0xAABBCCDD, 0xEEFF, "Ajido");

    auto packet = CEntitySetNamePacket(&trust);

    bool ok = true;
    ok      = expectEqualUInt(trust.trustID(), 0x777, "trust id") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(trust.passiveTrust()),
                              static_cast<uint8>(IsPassiveTrust::No),
                              "active trust flag") && ok;
    ok      = expectEqualUInt(trust.released(), false, "trust initially unreleased") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(trust.shieldSize()), 3, "default trust shield size") && ok;
    trust.setReleased(true);
    ok = expectEqualUInt(trust.released(), true, "trust released transition") && ok;
    ok      = expectPackedMessageSize(packet, 0x18 + 5, "trust") && ok;
    ok      = expectBytes(packet, entitySetNameTargIDOffset, std::array<uint8, 2>{ 0xFF, 0xEE }, "trust targid") && ok;
    ok      = expectBytes(packet, entitySetNameEntityIDOffset, std::array<uint8, 4>{ 0xDD, 0xCC, 0xBB, 0xAA }, "trust id") && ok;
    ok      = expectBytes(packet, entitySetNameMasterTargIDOffset, std::array<uint8, 2>{ 0x44, 0x33 }, "trust master targid") && ok;
    ok      = expectBytes(packet, entitySetNameNameOffset, std::array<uint8, 5>{ 'A', 'j', 'i', 'd', 'o' }, "trust name") && ok;
    ok      = expectZeroRange(packet, entitySetNameNameOffset + 5, entitySetNamePacketSize, "trust declared tail") && ok;
    return ok;
}

auto testMaximumDeclaredNameFillsPacketTail() -> bool
{
    auto entity = CCharEntity{};
    populateEntity(entity, 0x01020304, 0x3344, "ABCDEFGHIJKLMNOPQRST");

    auto packet = CEntitySetNamePacket(&entity);

    bool ok = true;
    ok      = expectPackedMessageSize(packet, entitySetNamePacketSize, "maximum name") && ok;
    ok      = expectBytes(packet, entitySetNameNameOffset,
                          std::array<uint8, 20>{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                                                 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T' },
                          "maximum name bytes") && ok;
    ok      = expectZeroRange(packet, entitySetNamePacketSize, PACKET_SIZE, "maximum packet tail") && ok;
    return ok;
}

} // namespace

auto runS2CEntitySetNamePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testNormalEntityConstructor() && ok;
    ok      = testTrustConstructorIncludesMasterTargID() && ok;
    ok      = testMaximumDeclaredNameFillsPacketTail() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("s2c-entity-set-name-packet", runS2CEntitySetNamePacketSelfTests);
