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

#include "test_s2c_conquest_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/conquest_system.h"
#include "map/packets/s2c/0x05e_conquest.h"

namespace
{

using ConquestPacket = GP_SERV_COMMAND_CONQUEST;

constexpr auto conquestDataSize   = sizeof(conquestdata_t);
constexpr auto conquestPacketSize = sizeof(GP_SERV_HEADER) + sizeof(ConquestPacket::PacketData);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CONQUEST packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CONQUEST packet self-test failed: " << label << " got";
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

template <std::size_t Size>
void putLE32(std::array<uint8, Size>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_CONQUEST), 0x05E, "CONQUEST packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(conquestregion_t), 4, "sizeof(conquestregion_t)") && ok;
    ok      = expectEqualUInt(sizeof(conquestdata_t), 156, "sizeof(conquestdata_t)") && ok;
    ok      = expectEqualUInt(sizeof(besiegedoverview_t), 4, "sizeof(besiegedoverview_t)") && ok;
    ok      = expectEqualUInt(sizeof(besiegedstronghold_t), 4, "sizeof(besiegedstronghold_t)") && ok;
    ok      = expectEqualUInt(sizeof(besiegeddata_t), 20, "sizeof(besiegeddata_t)") && ok;
    ok      = expectEqualUInt(sizeof(ConquestPacket::PacketData), 176, "PacketData size") && ok;
    ok      = expectEqualUInt(conquestPacketSize, 180, "packet size") && ok;

    ok = expectEqualUInt(offsetof(conquestdata_t, Balance), 0, "Conquest Balance offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, Alliance), 1, "Conquest Alliance offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, Regions), 22, "Conquest Regions offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, CurrentRegionSandoria), 130, "Conquest CurrentRegionSandoria offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, CurrentRegionWindurstPct), 135, "Conquest CurrentRegionWindurstPct offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, NextTally), 136, "Conquest NextTally offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, ConquestPoints), 140, "Conquest ConquestPoints offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, CurrentRegionBeastmen), 144, "Conquest CurrentRegionBeastmen offset") && ok;
    ok = expectEqualUInt(offsetof(conquestdata_t, Unknown9C), 152, "Conquest Unknown9C offset") && ok;
    ok = expectEqualUInt(offsetof(besiegeddata_t, Overview), 0, "Besieged Overview offset") && ok;
    ok = expectEqualUInt(offsetof(besiegeddata_t, MamookStronghold), 4, "Besieged MamookStronghold offset") && ok;
    ok = expectEqualUInt(offsetof(besiegeddata_t, HalvungStronghold), 8, "Besieged HalvungStronghold offset") && ok;
    ok = expectEqualUInt(offsetof(besiegeddata_t, ArrapagoStronghold), 12, "Besieged ArrapagoStronghold offset") && ok;
    ok = expectEqualUInt(offsetof(besiegeddata_t, ImperialStanding), 16, "Besieged ImperialStanding offset") && ok;
    return ok;
}

auto testBitfieldBytes() -> bool
{
    bool ok = true;

    besiegedoverview_t overview{};
    overview.AstralCandescence = 2;
    overview.AlZahbiOrders     = 1;
    overview.MamookLevel       = 9;
    overview.HalvungLevel      = 8;
    overview.ArrapagoLevel     = 7;
    overview.MamookOrders      = 6;
    overview.HalvungOrders     = 5;
    overview.ArrapagoOrders    = 4;
    overview.Unknown           = 1;

    auto overviewExpected = std::array<uint8, sizeof(besiegedoverview_t)>{};
    putLE32(overviewExpected, 0, 0x032E7896);
    ok = expectStructBytes(overview, overviewExpected, "besieged overview bitfields") && ok;

    besiegedstronghold_t stronghold{};
    stronghold.Orders          = 5;
    stronghold.Forces          = 0xA5;
    stronghold.Level           = 9;
    stronghold.MirrorDestroyed = 1;
    stronghold.Mirrors         = 0xC;
    stronghold.Prisoners       = 0xD;

    auto strongholdExpected = std::array<uint8, sizeof(besiegedstronghold_t)>{};
    putLE32(strongholdExpected, 0, 0x00DCCD2D);
    ok = expectStructBytes(stronghold, strongholdExpected, "besieged stronghold bitfields") && ok;

    return ok;
}

auto testConquestHelpers() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(conquest::GetInfluenceGraphics(0, 0, 0, 0), 0, "influence graphics all zero") && ok;
    ok      = expectEqualUInt(conquest::GetInfluenceGraphics(100, 100, 100, 100), 0, "influence graphics all equal") && ok;
    ok      = expectEqualUInt(conquest::GetInfluenceGraphics(10, 20, 30, 40), 64, "influence graphics beastmen lead") && ok;
    ok      = expectEqualUInt(conquest::GetInfluenceGraphics(35, 30, 35, 1), 21, "influence graphics nation thresholds") && ok;
    ok      = expectEqualUInt(conquest::GetInfluenceRanking(100, 50, 25, 10), 57, "influence ranking S/B/W") && ok;
    ok      = expectEqualUInt(conquest::GetInfluenceRanking(50, 100, 25), 54, "influence ranking no beastmen") && ok;
    ok      = expectEqualUInt(conquest::GetBalance(12, 3, 2, 12, 3, 2), 57, "balance no previous alliance") && ok;
    ok      = expectEqualUInt(conquest::GetBalance(4, 7, 12, 12, 3, 2), 23, "balance previous Sandoria alliance") && ok;
    ok      = expectEqualUInt(conquest::GetAlliance(12, 3, 2), 1, "alliance simple") && ok;
    ok      = expectEqualUInt(conquest::GetAlliance(8, 5, 3), 0, "alliance simple none") && ok;
    ok      = expectEqualUInt(conquest::GetAlliance(12, 3, 2, 12, 3, 2), 1, "alliance ranked") && ok;
    ok      = expectEqualUInt(conquest::GetAlliance(4, 7, 11, 12, 3, 2), 0, "alliance ranked none after prior alliance") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data                               = ConquestPacket::PacketData{};
    data.Conquest.Balance                  = 0x3A;
    data.Conquest.Alliance                 = 0x01;
    data.Conquest.Regions[0]               = { 0xAA, 0xBB, 0xCC, 0xDD };
    data.Conquest.Regions[18]              = { 0x11, 0x22, 0x33, 0x44 };
    data.Conquest.CurrentRegionSandoria    = 10;
    data.Conquest.CurrentRegionBastok      = 20;
    data.Conquest.CurrentRegionWindurst    = 30;
    data.Conquest.CurrentRegionSandoriaPct = 40;
    data.Conquest.CurrentRegionBastokPct   = 50;
    data.Conquest.CurrentRegionWindurstPct = 60;
    data.Conquest.NextTally                = 6;
    data.Conquest.ConquestPoints           = 0x01020304;
    data.Conquest.CurrentRegionBeastmen    = 70;
    data.Conquest.Unknown9C                = 0x01;
    data.Besieged.Overview.AstralCandescence       = 1;
    data.Besieged.Overview.AlZahbiOrders           = 2;
    data.Besieged.Overview.MamookLevel             = 3;
    data.Besieged.Overview.Unknown                 = 1;
    data.Besieged.MamookStronghold.Orders          = 4;
    data.Besieged.MamookStronghold.Forces          = 5;
    data.Besieged.HalvungStronghold.Level          = 6;
    data.Besieged.ArrapagoStronghold.Prisoners     = 7;
    data.Besieged.ImperialStanding                 = 0xA1A2A3A4;

    auto expected = std::array<uint8, sizeof(ConquestPacket::PacketData)>{};
    expected[0]   = 0x3A;
    expected[1]   = 0x01;
    expected[22]  = 0xAA;
    expected[23]  = 0xBB;
    expected[24]  = 0xCC;
    expected[25]  = 0xDD;
    expected[94]  = 0x11;
    expected[95]  = 0x22;
    expected[96]  = 0x33;
    expected[97]  = 0x44;
    expected[130] = 10;
    expected[131] = 20;
    expected[132] = 30;
    expected[133] = 40;
    expected[134] = 50;
    expected[135] = 60;
    expected[136] = 6;
    putLE32(expected, 140, 0x01020304);
    expected[144] = 70;
    expected[152] = 0x01;
    putLE32(expected, conquestDataSize+0, 0x02000039);
    putLE32(expected, conquestDataSize+4, 0x0000002C);
    putLE32(expected, conquestDataSize+8, 0x00003000);
    putLE32(expected, conquestDataSize+12, 0x00700000);
    putLE32(expected, conquestDataSize+16, 0xA1A2A3A4);

    return expectStructBytes(data, expected, "CONQUEST PacketData bytes");
}

} // namespace

auto runS2CConquestPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testBitfieldBytes() && ok;
    ok      = testConquestHelpers() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
