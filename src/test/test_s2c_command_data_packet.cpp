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

#include "test_s2c_command_data_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x0ac_command_data.h"

namespace
{

using CommandDataPacket = GP_SERV_COMMAND_COMMAND_DATA;

constexpr auto commandDataTblOffset        = sizeof(GP_SERV_HEADER) + offsetof(CommandDataPacket::PacketData, CommandDataTbl);
constexpr auto commandWeaponSkillsOffset   = commandDataTblOffset + offsetof(CommandDataTbl_t, WeaponSkills);
constexpr auto commandJobAbilitiesOffset   = commandDataTblOffset + offsetof(CommandDataTbl_t, JobAbilities);
constexpr auto commandPetAbilitiesOffset   = commandDataTblOffset + offsetof(CommandDataTbl_t, PetAbilities);
constexpr auto commandTraitsOffset         = commandDataTblOffset + offsetof(CommandDataTbl_t, Traits);
constexpr auto commandDataTblSize          = sizeof(CommandDataTbl_t);
constexpr auto commandPacketDataSize       = sizeof(CommandDataPacket::PacketData);
constexpr auto commandPacketSize           = sizeof(GP_SERV_HEADER) + commandPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c COMMAND_DATA packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c COMMAND_DATA packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_COMMAND_DATA), 0x0AC, "COMMAND_DATA packet id") && ok;
    ok      = expectEqualUInt(commandDataTblSize, 224, "sizeof(CommandDataTbl_t)") && ok;
    ok      = expectEqualUInt(commandPacketDataSize, 224, "sizeof(COMMAND_DATA::PacketData)") && ok;
    ok      = expectEqualUInt(commandPacketSize, 228, "COMMAND_DATA packet size") && ok;
    ok      = expectEqualUInt(commandDataTblOffset, 4, "CommandDataTbl offset") && ok;
    ok      = expectEqualUInt(commandWeaponSkillsOffset, 4, "WeaponSkills offset") && ok;
    ok      = expectEqualUInt(commandJobAbilitiesOffset, 68, "JobAbilities offset") && ok;
    ok      = expectEqualUInt(commandPetAbilitiesOffset, 132, "PetAbilities offset") && ok;
    ok      = expectEqualUInt(commandTraitsOffset, 196, "Traits offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = CommandDataPacket::PacketData{};

    data.CommandDataTbl.WeaponSkills[0]  = 0x11;
    data.CommandDataTbl.WeaponSkills[63] = 0x22;
    data.CommandDataTbl.JobAbilities[0]  = 0x33;
    data.CommandDataTbl.JobAbilities[63] = 0x44;
    data.CommandDataTbl.PetAbilities[0]  = 0x55;
    data.CommandDataTbl.PetAbilities[63] = 0x66;
    data.CommandDataTbl.Traits[0]        = 0x77;
    data.CommandDataTbl.Traits[31]       = 0x88;

    auto expected = std::array<uint8, 224>{};
    expected[0]   = 0x11;
    expected[63]  = 0x22;
    expected[64]  = 0x33;
    expected[127] = 0x44;
    expected[128] = 0x55;
    expected[191] = 0x66;
    expected[192] = 0x77;
    expected[223] = 0x88;

    return expectStructBytes(data, expected, "COMMAND_DATA PacketData bytes");
}

} // namespace

auto runS2CCommandDataPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
