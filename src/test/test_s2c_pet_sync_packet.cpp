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

#include "test_s2c_pet_sync_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

#include "common/utils.h"
#include "entities/char_entity.h"
#include "map/packets/pet_sync.h"

namespace
{

constexpr auto petSyncOwnerOnlyPacketSize  = 0x1C;
constexpr auto petSyncWithPetPacketSize    = 0x2C;
constexpr auto petSyncControlOffset        = 0x04;
constexpr auto petSyncOwnerTargIDOffset    = 0x06;
constexpr auto petSyncOwnerIDOffset        = 0x08;
constexpr auto petSyncPetTargIDOffset      = 0x0C;
constexpr auto petSyncPetHPPOffset         = 0x0E;
constexpr auto petSyncPetMPPOffset         = 0x0F;
constexpr auto petSyncPetTPOffset          = 0x10;
constexpr auto petSyncBattleTargetIDOffset = 0x14;
constexpr auto petSyncNameOffset           = 0x18;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

void populateOwner(CCharEntity& entity, std::uint32_t id, std::uint16_t targid)
{
    entity.id     = id;
    entity.targid = targid;
}

void populatePet(CCharEntity& pet, std::uint16_t targid, std::string name)
{
    pet.targid       = targid;
    pet.name         = std::move(name);
    pet.health.hp    = 375;
    pet.health.modhp = 500;
    pet.health.mp    = 120;
    pet.health.modmp = 200;
    pet.health.tp    = 1234;
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c PET_SYNC packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c PET_SYNC packet self-test failed: " << label << " got";
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
            std::cerr << "s2c PET_SYNC packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto expectPackedMessageSize(CBasicPacket& packet, std::uint16_t expected, const std::string& label) -> bool
{
    auto* data = packetData(packet);
    bool  ok   = true;
    ok         = expectEqualUInt(data[petSyncControlOffset] & 0x3F, 0x04, label + " low control bits") && ok;
    ok         = expectEqualUInt(unpackBitsBE(data + petSyncControlOffset, 0, 6, 10), expected, label + " message size") && ok;
    return ok;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(petSyncOwnerOnlyPacketSize, 28, "owner-only packet size") && ok;
    ok      = expectEqualUInt(petSyncWithPetPacketSize, 44, "with-pet packet size") && ok;
    ok      = expectEqualUInt(petSyncControlOffset, 4, "control offset") && ok;
    ok      = expectEqualUInt(petSyncOwnerTargIDOffset, 6, "owner targid offset") && ok;
    ok      = expectEqualUInt(petSyncOwnerIDOffset, 8, "owner id offset") && ok;
    ok      = expectEqualUInt(petSyncPetTargIDOffset, 12, "pet targid offset") && ok;
    ok      = expectEqualUInt(petSyncPetHPPOffset, 14, "pet HPP offset") && ok;
    ok      = expectEqualUInt(petSyncPetMPPOffset, 15, "pet MPP offset") && ok;
    ok      = expectEqualUInt(petSyncPetTPOffset, 16, "pet TP offset") && ok;
    ok      = expectEqualUInt(petSyncBattleTargetIDOffset, 20, "battle target offset") && ok;
    ok      = expectEqualUInt(petSyncNameOffset, 24, "name offset") && ok;
    return ok;
}

auto testOwnerOnlyConstructor() -> bool
{
    auto owner = CCharEntity{};
    populateOwner(owner, 0x11223344, 0x5566);

    auto packet = CPetSyncPacket(&owner);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x068, "owner-only type") && ok;
    ok      = expectEqualUInt(packet.getSize(), petSyncOwnerOnlyPacketSize, "owner-only size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x68, 0x0E, 0xEF, 0xBE }, "owner-only header") && ok;
    ok      = expectBytes(packet, petSyncControlOffset, std::array<uint8, 2>{ 0x04, 0x06 }, "owner-only packed control") && ok;
    ok      = expectPackedMessageSize(packet, 0x18, "owner-only") && ok;
    ok      = expectBytes(packet, petSyncOwnerTargIDOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "owner-only targid") && ok;
    ok      = expectBytes(packet, petSyncOwnerIDOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "owner-only id") && ok;
    ok      = expectZeroRange(packet, petSyncPetTargIDOffset, petSyncOwnerOnlyPacketSize, "owner-only payload tail") && ok;
    ok      = expectZeroRange(packet, petSyncOwnerOnlyPacketSize, PACKET_SIZE, "owner-only packet tail") && ok;
    return ok;
}

auto testWithPetConstructor() -> bool
{
    auto owner = CCharEntity{};
    auto pet   = CCharEntity{};
    populateOwner(owner, 0x01020304, 0x7788);
    populatePet(pet, 0x3344, "Carbuncle");
    owner.PPet = &pet;

    auto packet = CPetSyncPacket(&owner);
    packet.setSequence(0xCAFE);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x068, "with-pet type") && ok;
    ok      = expectEqualUInt(packet.getSize(), petSyncWithPetPacketSize, "with-pet size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x68, 0x16, 0xFE, 0xCA }, "with-pet header") && ok;
    ok      = expectBytes(packet, petSyncControlOffset, std::array<uint8, 2>{ 0x44, 0x08 }, "with-pet packed control") && ok;
    ok      = expectPackedMessageSize(packet, 0x18 + 9, "with-pet") && ok;
    ok      = expectBytes(packet, petSyncOwnerTargIDOffset, std::array<uint8, 2>{ 0x88, 0x77 }, "with-pet owner targid") && ok;
    ok      = expectBytes(packet, petSyncOwnerIDOffset, std::array<uint8, 4>{ 0x04, 0x03, 0x02, 0x01 }, "with-pet owner id") && ok;
    ok      = expectBytes(packet, petSyncPetTargIDOffset, std::array<uint8, 2>{ 0x44, 0x33 }, "with-pet pet targid") && ok;
    ok      = expectEqualUInt(packetData(packet)[petSyncPetHPPOffset], 75, "with-pet HPP") && ok;
    ok      = expectEqualUInt(packetData(packet)[petSyncPetMPPOffset], 60, "with-pet MPP") && ok;
    ok      = expectBytes(packet, petSyncPetTPOffset, std::array<uint8, 2>{ 0xD2, 0x04 }, "with-pet TP") && ok;
    ok      = expectBytes(packet, petSyncBattleTargetIDOffset, std::array<uint8, 4>{ 0x00, 0x00, 0x00, 0x00 }, "with-pet battle target") && ok;
    ok      = expectBytes(packet, petSyncNameOffset, std::array<uint8, 9>{ 'C', 'a', 'r', 'b', 'u', 'n', 'c', 'l', 'e' }, "with-pet name") && ok;
    ok      = expectZeroRange(packet, petSyncNameOffset + 9, petSyncWithPetPacketSize, "with-pet declared tail") && ok;
    ok      = expectZeroRange(packet, petSyncWithPetPacketSize, PACKET_SIZE, "with-pet packet tail") && ok;
    return ok;
}

} // namespace

auto runS2CPetSyncPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testOwnerOnlyConstructor() && ok;
    ok      = testWithPetConstructor() && ok;
    return ok;
}
