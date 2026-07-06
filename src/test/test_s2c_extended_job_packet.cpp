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

#include "test_s2c_extended_job_packet.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x044_extended_job_blu.h"
#include "map/packets/s2c/0x044_extended_job_mon.h"
#include "map/packets/s2c/0x044_extended_job_pup.h"

namespace
{

using BluPacket = GP_SERV_COMMAND_EXTENDED_JOB::BLU;
using PupPacket = GP_SERV_COMMAND_EXTENDED_JOB::PUP;
using MonPacket = GP_SERV_COMMAND_EXTENDED_JOB::MON;

constexpr auto extendedJobPacketID = static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_EXTENDED_JOB);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c EXTENDED_JOB packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c EXTENDED_JOB packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto valueByte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(valueByte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto put16(std::array<uint8, 156>& bytes, const std::size_t offset, const uint16_t value) -> void
{
    bytes[offset]     = static_cast<uint8>(value & 0xFF);
    bytes[offset + 1] = static_cast<uint8>(value >> 8);
}

auto put32(std::array<uint8, 156>& bytes, const std::size_t offset, const uint32_t value) -> void
{
    bytes[offset]     = static_cast<uint8>(value & 0xFF);
    bytes[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    bytes[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    bytes[offset + 3] = static_cast<uint8>(value >> 24);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(extendedJobPacketID, 0x044, "EXTENDED_JOB packet id") && ok;

    ok = expectEqualUInt(sizeof(BluPacket::PacketData), 156, "sizeof(BLU PacketData)") && ok;
    ok = expectEqualUInt(sizeof(PupPacket::PacketData), 156, "sizeof(PUP PacketData)") && ok;
    ok = expectEqualUInt(sizeof(MonPacket::PacketData), 156, "sizeof(MON PacketData)") && ok;

    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + sizeof(BluPacket::PacketData), 160, "BLU packet size") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + sizeof(PupPacket::PacketData), 160, "PUP packet size") && ok;
    ok = expectEqualUInt(sizeof(GP_SERV_HEADER) + sizeof(MonPacket::PacketData), 160, "MON packet size") && ok;

    ok = expectEqualUInt(offsetof(BluPacket::PacketData, Job), 0, "BLU Job offset") && ok;
    ok = expectEqualUInt(offsetof(BluPacket::PacketData, IsSubJob), 1, "BLU IsSubJob offset") && ok;
    ok = expectEqualUInt(offsetof(BluPacket::PacketData, SetSpells), 4, "BLU SetSpells offset") && ok;
    ok = expectEqualUInt(offsetof(BluPacket::PacketData, unknown01), 24, "BLU unknown01 offset") && ok;

    ok = expectEqualUInt(offsetof(PupPacket::PacketData, Head), 4, "PUP Head offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, Frame), 5, "PUP Frame offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, Attachments), 6, "PUP Attachments offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, UnlockedHeads), 20, "PUP UnlockedHeads offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, UnlockedAttachments), 52, "PUP UnlockedAttachments offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, Name), 84, "PUP Name offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, HP), 100, "PUP HP offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, STR), 124, "PUP STR offset") && ok;
    ok = expectEqualUInt(offsetof(PupPacket::PacketData, BonusElementalCapacity), 152, "PUP BonusElementalCapacity offset") && ok;

    ok = expectEqualUInt(offsetof(MonPacket::PacketData, Species), 4, "MON Species offset") && ok;
    ok = expectEqualUInt(offsetof(MonPacket::PacketData, EquippedInstincts), 8, "MON EquippedInstincts offset") && ok;
    ok = expectEqualUInt(offsetof(MonPacket::PacketData, padding03), 32, "MON padding03 offset") && ok;
    return ok;
}

auto testBluPacketDataBytes() -> bool
{
    auto data = BluPacket::PacketData{};
    data.Job  = 16;
    data.IsSubJob = 1;
    data.SetSpells[0]  = 0x11;
    data.SetSpells[19] = 0x22;
    data.unknown01[0]  = 0x33;
    data.unknown01[131] = 0x44;

    auto expected = std::array<uint8, 156>{};
    expected[0]   = 16;
    expected[1]   = 1;
    expected[4]   = 0x11;
    expected[23]  = 0x22;
    expected[24]  = 0x33;
    expected[155] = 0x44;
    return expectStructBytes(data, expected, "BLU PacketData bytes");
}

auto testPupPacketDataBytes() -> bool
{
    auto data = PupPacket::PacketData{};
    data.Job      = 18;
    data.IsSubJob = 1;
    data.Head     = AutomatonHead::Soulsoother;
    data.Frame    = AutomatonFrame::Stormwaker;
    data.Attachments[0]  = 0x10;
    data.Attachments[11] = 0x1B;
    data.unknown00[0]    = 0x2A;
    data.unknown00[1]    = 0x2B;
    data.UnlockedHeads   = 0x01020304;
    data.UnlockedFrames  = 0x05060708;
    data.UnlockedAttachments[0] = 0x11223344;
    data.UnlockedAttachments[7] = 0x55667788;
    std::memcpy(data.Name, "PuppetName123456", sizeof(data.Name));
    data.HP              = 0x1112;
    data.MaxHP           = 0x1314;
    data.MP              = 0x1516;
    data.MaxMP           = 0x1718;
    data.MeleeSkill      = 0x191A;
    data.MeleeSkillCap   = 0x1B1C;
    data.RangedSkill     = 0x1D1E;
    data.RangedSkillCap  = 0x1F20;
    data.MagicSkill      = 0x2122;
    data.MagicSkillCap   = 0x2324;
    data.STR             = 0x2526;
    data.BonusSTR        = 0x2728;
    data.DEX             = 0x292A;
    data.BonusDEX        = 0x2B2C;
    data.VIT             = 0x2D2E;
    data.BonusVIT        = 0x2F30;
    data.AGI             = 0x3132;
    data.BonusAGI        = 0x3334;
    data.INT             = 0x3536;
    data.BonusINT        = 0x3738;
    data.MND             = 0x393A;
    data.BonusMND        = 0x3B3C;
    data.CHR             = 0x3D3E;
    data.BonusCHR        = 0x3F40;
    data.BonusElementalCapacity = 0x41;

    auto expected = std::array<uint8, 156>{};
    expected[0] = 18;
    expected[1] = 1;
    expected[4] = 0x05;
    expected[5] = 0x23;
    expected[6] = 0x10;
    expected[17] = 0x1B;
    expected[18] = 0x2A;
    expected[19] = 0x2B;
    put32(expected, 20, 0x01020304);
    put32(expected, 24, 0x05060708);
    put32(expected, 52, 0x11223344);
    put32(expected, 80, 0x55667788);
    std::copy_n(reinterpret_cast<const uint8*>("PuppetName123456"), 16, expected.begin() + 84);
    put16(expected, 100, 0x1112);
    put16(expected, 102, 0x1314);
    put16(expected, 104, 0x1516);
    put16(expected, 106, 0x1718);
    put16(expected, 108, 0x191A);
    put16(expected, 110, 0x1B1C);
    put16(expected, 112, 0x1D1E);
    put16(expected, 114, 0x1F20);
    put16(expected, 116, 0x2122);
    put16(expected, 118, 0x2324);
    put16(expected, 124, 0x2526);
    put16(expected, 126, 0x2728);
    put16(expected, 128, 0x292A);
    put16(expected, 130, 0x2B2C);
    put16(expected, 132, 0x2D2E);
    put16(expected, 134, 0x2F30);
    put16(expected, 136, 0x3132);
    put16(expected, 138, 0x3334);
    put16(expected, 140, 0x3536);
    put16(expected, 142, 0x3738);
    put16(expected, 144, 0x393A);
    put16(expected, 146, 0x3B3C);
    put16(expected, 148, 0x3D3E);
    put16(expected, 150, 0x3F40);
    expected[152] = 0x41;
    return expectStructBytes(data, expected, "PUP PacketData bytes");
}

auto testMonPacketDataBytes() -> bool
{
    auto data = MonPacket::PacketData{};
    data.Job      = 23;
    data.IsSubJob = 1;
    data.Species  = 0x1234;
    data.EquippedInstincts[0]  = 0x1111;
    data.EquippedInstincts[11] = 0x2222;

    auto expected = std::array<uint8, 156>{};
    expected[0] = 23;
    expected[1] = 1;
    put16(expected, 4, 0x1234);
    put16(expected, 8, 0x1111);
    put16(expected, 30, 0x2222);
    return expectStructBytes(data, expected, "MON PacketData bytes");
}

} // namespace

auto runS2CExtendedJobPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testBluPacketDataBytes() && ok;
    ok      = testPupPacketDataBytes() && ok;
    ok      = testMonPacketDataBytes() && ok;
    return ok;
}
