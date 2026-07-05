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

#include "test_s2c_group_effects_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x076_group_effects.h"

namespace
{

constexpr auto groupEffectsMemberCount      = 5U;
constexpr auto groupEffectsMemberSize       = sizeof(partymemberbuffs_t);
constexpr auto groupEffectsUniqueNoOffset   = sizeof(GP_SERV_HEADER) + offsetof(partymemberbuffs_t, UniqueNo);
constexpr auto groupEffectsActIndexOffset   = sizeof(GP_SERV_HEADER) + offsetof(partymemberbuffs_t, ActIndex);
constexpr auto groupEffectsPadding06Offset  = sizeof(GP_SERV_HEADER) + offsetof(partymemberbuffs_t, padding06);
constexpr auto groupEffectsBitsOffset       = sizeof(GP_SERV_HEADER) + offsetof(partymemberbuffs_t, Bits);
constexpr auto groupEffectsBuffsOffset      = sizeof(GP_SERV_HEADER) + offsetof(partymemberbuffs_t, Buffs);
constexpr auto groupEffectsPacketDataSize   = sizeof(GP_SERV_COMMAND_GROUP_EFFECTS::PacketData);
constexpr auto groupEffectsPacketSize       = sizeof(GP_SERV_HEADER) + groupEffectsPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c GROUP_EFFECTS packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c GROUP_EFFECTS packet self-test failed: " << label << " got";
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

template <typename T, std::size_t Size>
auto expectStructBytes(const T& value, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    const auto* data = reinterpret_cast<const uint8*>(&value);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c GROUP_EFFECTS packet self-test failed: " << label << " got";
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
            std::cerr << "s2c GROUP_EFFECTS packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

void makeChar(CCharEntity& character, std::uint32_t id, std::uint16_t targid)
{
    character.id     = id;
    character.targid = targid;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(groupEffectsMemberCount, 5, "std::size(Members)") && ok;
    ok      = expectEqualUInt(groupEffectsMemberSize, 48, "sizeof(partymemberbuffs_t)") && ok;
    ok      = expectEqualUInt(groupEffectsPacketDataSize, 240, "sizeof(GROUP_EFFECTS::PacketData)") && ok;
    ok      = expectEqualUInt(groupEffectsPacketSize, 244, "GROUP_EFFECTS packet size") && ok;
    ok      = expectEqualUInt(groupEffectsUniqueNoOffset, 4, "UniqueNo offset") && ok;
    ok      = expectEqualUInt(groupEffectsActIndexOffset, 8, "ActIndex offset") && ok;
    ok      = expectEqualUInt(groupEffectsPadding06Offset, 10, "padding06 offset") && ok;
    ok      = expectEqualUInt(groupEffectsBitsOffset, 12, "Bits offset") && ok;
    ok      = expectEqualUInt(groupEffectsBuffsOffset, 20, "Buffs offset") && ok;
    return ok;
}

auto testConstructorCopiesMembersAndCapsAtFive() -> bool
{
    auto member0 = CCharEntity{};
    auto member1 = CCharEntity{};
    auto member2 = CCharEntity{};
    auto member3 = CCharEntity{};
    auto member4 = CCharEntity{};
    auto member5 = CCharEntity{};
    makeChar(member0, 0x11223344, 0x5566);
    makeChar(member1, 0xAABBCCDD, 0x7788);
    makeChar(member2, 0x01020304, 0x090A);
    makeChar(member3, 0x05060708, 0x0B0C);
    makeChar(member4, 0x0D0E0F10, 0x0D0E);
    makeChar(member5, 0xDEADBEEF, 0xF00D);

    auto members = std::vector<CCharEntity*>{ &member0, &member1, &member2, &member3, &member4, &member5 };
    auto packet  = GP_SERV_COMMAND_GROUP_EFFECTS(members);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x076, "GROUP_EFFECTS type") && ok;
    ok      = expectEqualUInt(packet.getSize(), groupEffectsPacketSize, "GROUP_EFFECTS size") && ok;
    ok      = expectBytes(packet, 0, std::array<uint8, 20>{ 0x76, 0x7A, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11, 0x66, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "member 0 prefix") && ok;
    ok      = expectBytes(packet, sizeof(GP_SERV_HEADER) + groupEffectsMemberSize, std::array<uint8, 8>{ 0xDD, 0xCC, 0xBB, 0xAA, 0x88, 0x77, 0x00, 0x00 }, "member 1 identity") && ok;
    ok      = expectBytes(packet, sizeof(GP_SERV_HEADER) + groupEffectsMemberSize*4, std::array<uint8, 8>{ 0x10, 0x0F, 0x0E, 0x0D, 0x0E, 0x0D, 0x00, 0x00 }, "member 4 identity") && ok;
    ok      = expectZeroTail(packet, groupEffectsPacketSize, "packet tail") && ok;
    return ok;
}

auto testEmptyConstructorLeavesMembersZero() -> bool
{
    auto members = std::vector<CCharEntity*>{};
    auto packet  = GP_SERV_COMMAND_GROUP_EFFECTS(members);
    packet.setSequence(0xBEEF);

    bool ok = true;
    ok      = expectBytes(packet, 0, std::array<uint8, 4>{ 0x76, 0x7A, 0xEF, 0xBE }, "empty constructor header") && ok;
    ok      = expectZeroTail(packet, sizeof(GP_SERV_HEADER), "empty constructor members") && ok;
    return ok;
}

auto testMemberStructBytes() -> bool
{
    auto member      = partymemberbuffs_t{};
    member.UniqueNo = 0x11223344;
    member.ActIndex = 0x5566;
    member.Bits     = 0x0102030405060708;
    for (std::size_t i = 0; i < std::size(member.Buffs); ++i)
    {
        member.Buffs[i] = static_cast<uint8>(i + 1);
    }

    return expectStructBytes(member, std::array<uint8, 48>{
                                         0x44, 0x33, 0x22, 0x11,
                                         0x66, 0x55, 0x00, 0x00,
                                         0x08, 0x07, 0x06, 0x05,
                                         0x04, 0x03, 0x02, 0x01,
                                         0x01, 0x02, 0x03, 0x04,
                                         0x05, 0x06, 0x07, 0x08,
                                         0x09, 0x0A, 0x0B, 0x0C,
                                         0x0D, 0x0E, 0x0F, 0x10,
                                         0x11, 0x12, 0x13, 0x14,
                                         0x15, 0x16, 0x17, 0x18,
                                         0x19, 0x1A, 0x1B, 0x1C,
                                         0x1D, 0x1E, 0x1F, 0x20,
                                     },
                             "member struct bytes");
}

} // namespace

auto runS2CGroupEffectsPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testConstructorCopiesMembersAndCapsAtFive() && ok;
    ok      = testEmptyConstructorLeavesMembersZero() && ok;
    ok      = testMemberStructBytes() && ok;
    return ok;
}
