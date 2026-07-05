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

#include "test_s2c_influence_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x071_influence.h"
#include "map/packets/s2c/0x071_influence_campaign.h"
#include "map/packets/s2c/0x071_influence_colonization.h"

namespace
{

using CampaignPacket     = GP_SERV_COMMAND_INFLUENCE::CAMPAIGN;
using ColonizationPacket = GP_SERV_COMMAND_INFLUENCE::COLONIZATION;

constexpr auto campaignModeOffset            = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, Mode);
constexpr auto campaignPadding05Offset       = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, padding05);
constexpr auto campaignLengthOffset          = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, Length);
constexpr auto campaignPadding07Offset       = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, padding07);
constexpr auto campaignZoneOffsetOffset      = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, ZoneOffset);
constexpr auto campaignPadding0AOffset       = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, padding0A);
constexpr auto campaignAlliedNotesOffset     = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, AlliedNotes);
constexpr auto campaignControlledAreasOffset = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, ControlledAreas);
constexpr auto campaignNationsOffset         = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, Nations);
constexpr auto campaignZonesOffset           = sizeof(GP_SERV_HEADER) + offsetof(CampaignPacket::PacketData, Zones);
constexpr auto campaignPacketDataSize        = sizeof(CampaignPacket::PacketData);
constexpr auto campaignPacketSize            = sizeof(GP_SERV_HEADER) + campaignPacketDataSize;

constexpr auto colonizationModeOffset       = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, Mode);
constexpr auto colonizationPadding05Offset  = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, padding05);
constexpr auto colonizationLengthOffset     = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, Length);
constexpr auto colonizationUnknown00Offset  = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, unknown00);
constexpr auto colonizationUnknown01Offset  = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, unknown01);
constexpr auto colonizationRanksOffset      = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, Ranks);
constexpr auto colonizationZonesOffset      = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, Zones);
constexpr auto colonizationBayldOffset      = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, Bayld);
constexpr auto colonizationPadding00Offset  = sizeof(GP_SERV_HEADER) + offsetof(ColonizationPacket::PacketData, padding00);
constexpr auto colonizationPacketDataSize   = sizeof(ColonizationPacket::PacketData);
constexpr auto colonizationPacketSize       = sizeof(GP_SERV_HEADER) + colonizationPacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c influence packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c influence packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(data[i]);
        }
        std::cerr << " expected";
        for (const auto byte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(byte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_INFLUENCE_MODE::Campaign), 2, "INFLUENCE_MODE::Campaign") && ok;
    ok      = expectEqualUInt(static_cast<uint8>(GP_SERV_COMMAND_INFLUENCE_MODE::Colonization), 3, "INFLUENCE_MODE::Colonization") && ok;

    ok = expectEqualUInt(campaignPacketDataSize, 200, "sizeof(CAMPAIGN::PacketData)") && ok;
    ok = expectEqualUInt(campaignPacketSize, 204, "CAMPAIGN packet size") && ok;
    ok = expectEqualUInt(campaignModeOffset, 4, "CAMPAIGN Mode offset") && ok;
    ok = expectEqualUInt(campaignPadding05Offset, 5, "CAMPAIGN padding05 offset") && ok;
    ok = expectEqualUInt(campaignLengthOffset, 6, "CAMPAIGN Length offset") && ok;
    ok = expectEqualUInt(campaignPadding07Offset, 8, "CAMPAIGN padding07 offset") && ok;
    ok = expectEqualUInt(campaignZoneOffsetOffset, 9, "CAMPAIGN ZoneOffset offset") && ok;
    ok = expectEqualUInt(campaignPadding0AOffset, 10, "CAMPAIGN padding0A offset") && ok;
    ok = expectEqualUInt(campaignAlliedNotesOffset, 12, "CAMPAIGN AlliedNotes offset") && ok;
    ok = expectEqualUInt(campaignControlledAreasOffset, 16, "CAMPAIGN ControlledAreas offset") && ok;
    ok = expectEqualUInt(campaignNationsOffset, 20, "CAMPAIGN Nations offset") && ok;
    ok = expectEqualUInt(campaignZonesOffset, 48, "CAMPAIGN Zones offset") && ok;

    ok = expectEqualUInt(colonizationPacketDataSize, 200, "sizeof(COLONIZATION::PacketData)") && ok;
    ok = expectEqualUInt(colonizationPacketSize, 204, "COLONIZATION packet size") && ok;
    ok = expectEqualUInt(colonizationModeOffset, 4, "COLONIZATION Mode offset") && ok;
    ok = expectEqualUInt(colonizationPadding05Offset, 5, "COLONIZATION padding05 offset") && ok;
    ok = expectEqualUInt(colonizationLengthOffset, 6, "COLONIZATION Length offset") && ok;
    ok = expectEqualUInt(colonizationUnknown00Offset, 8, "COLONIZATION unknown00 offset") && ok;
    ok = expectEqualUInt(colonizationUnknown01Offset, 12, "COLONIZATION unknown01 offset") && ok;
    ok = expectEqualUInt(colonizationRanksOffset, 16, "COLONIZATION Ranks offset") && ok;
    ok = expectEqualUInt(colonizationZonesOffset, 20, "COLONIZATION Zones offset") && ok;
    ok = expectEqualUInt(colonizationBayldOffset, 56, "COLONIZATION Bayld offset") && ok;
    ok = expectEqualUInt(colonizationPadding00Offset, 60, "COLONIZATION padding00 offset") && ok;
    return ok;
}

auto testCampaignBitPacking() -> bool
{
    auto controlled     = campaigncontrolledareas_t{};
    controlled.Sandoria = 1;
    controlled.Bastok   = 2;
    controlled.Windurst = 3;
    controlled.Beastman = 4;

    auto nation            = campaignnation_t{};
    nation.Reconnaissance  = 5;
    nation.Morale          = 66;
    nation.Prosperity      = 77;

    auto zone                    = campaignzone_t{};
    zone.Owner                   = 5;
    zone.CurrentFortifications   = 0x155;
    zone.CurrentResources        = 0x2AA;
    zone.Heroism                 = 0xBC;
    zone.InfluenceSandoria       = 0x11;
    zone.InfluenceBastok         = 0x22;
    zone.InfluenceWindurst       = 0x33;
    zone.InfluenceBeastman       = 0x44;
    zone.MaxFortifications       = 0x2AB;
    zone.MaxResources            = 0x155;

    bool ok = true;
    ok      = expectEqualUInt(sizeof(controlled), 4, "sizeof(campaigncontrolledareas_t)") && ok;
    ok      = expectEqualUInt(sizeof(nation), 4, "sizeof(campaignnation_t)") && ok;
    ok      = expectEqualUInt(sizeof(campaignnations_t), 28, "sizeof(campaignnations_t)") && ok;
    ok      = expectEqualUInt(sizeof(zone), 12, "sizeof(campaignzone_t)") && ok;
    ok      = expectStructBytes(controlled, std::array<uint8, 4>{ 0x41, 0x0C, 0x02, 0x00 }, "campaign controlled areas bytes") && ok;
    ok      = expectStructBytes(nation, std::array<uint8, 4>{ 0x05, 0x00, 0x08, 0x9B }, "campaign nation bytes") && ok;
    ok      = expectStructBytes(zone, std::array<uint8, 12>{ 0x5A, 0x95, 0xAA, 0xBC, 0x11, 0x22, 0x33, 0x44, 0xAB, 0x56, 0x05, 0x00 }, "campaign zone bytes") && ok;
    return ok;
}

auto testColonizationBitPacking() -> bool
{
    auto ranks          = coalitionranks_t{};
    ranks.Pioneers     = 1;
    ranks.Peacekeepers = 2;
    ranks.Couriers     = 3;
    ranks.Scouts       = 4;
    ranks.Inventors    = 5;
    ranks.Mummers      = 6;

    auto zone              = colonizationzone_t{};
    zone.ColonizationRate  = 85;
    zone.CurrentBivouacs   = 6;
    zone.MaxBivouacs       = 5;

    bool ok = true;
    ok      = expectEqualUInt(sizeof(ranks), 4, "sizeof(coalitionranks_t)") && ok;
    ok      = expectEqualUInt(sizeof(zone), 4, "sizeof(colonizationzone_t)") && ok;
    ok      = expectEqualUInt(sizeof(colonizationzones_t), 36, "sizeof(colonizationzones_t)") && ok;
    ok      = expectStructBytes(ranks, std::array<uint8, 4>{ 0x21, 0x43, 0x65, 0x00 }, "coalition ranks bytes") && ok;
    ok      = expectStructBytes(zone, std::array<uint8, 4>{ 0x55, 0x17, 0x00, 0x00 }, "colonization zone bytes") && ok;
    return ok;
}

} // namespace

auto runS2CInfluencePacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testCampaignBitPacking() && ok;
    ok      = testColonizationBitPacking() && ok;
    return ok;
}
