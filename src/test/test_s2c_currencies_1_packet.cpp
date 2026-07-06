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

#include "test_s2c_currencies_1_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x113_currencies_1.h"

namespace
{

using Currencies1Packet = GP_SERV_COMMAND_CURRENCIES_1;

constexpr auto currencies1PacketDataSize = sizeof(Currencies1Packet::PacketData);
constexpr auto currencies1PacketSize     = sizeof(GP_SERV_HEADER) + currencies1PacketDataSize;

constexpr auto currencies1ConquestSandoriaOffset = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, conquest_points_sandoria);
constexpr auto currencies1BeastmanSealOffset     = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, beastmans_seals_stored);
constexpr auto currencies1GuildFishingOffset     = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, guild_points_fishing);
constexpr auto currencies1FewellFireOffset       = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, synergy_fewell_fire);
constexpr auto currencies1DailyTallyOffset       = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, daily_tally);
constexpr auto currencies1Unknown67Offset        = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, unknown67);
constexpr auto currencies1LegionPointsOffset     = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, legion_points);
constexpr auto currencies1Trophy5Offset          = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, echelon_battle_trophies_5th);
constexpr auto currencies1TraverserOffset        = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, traverser_stones);
constexpr auto currencies1Rems1Offset            = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, rems_tale_chapters_1_stored);
constexpr auto currencies1ReclamationOffset      = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, reclamation_marks);
constexpr auto currencies1PlansOffset            = currencies1ReclamationOffset - sizeof(std::uint64_t);
constexpr auto currencies1Padding00Offset        = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, padding00);
constexpr auto currencies1UnityOffset            = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, unity_accolades);
constexpr auto currencies1CrystalsOffset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, fire_crystals_stored);
constexpr auto currencies1DeedsOffset            = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, deeds);
constexpr auto currencies1Padding01Offset        = sizeof(GP_SERV_HEADER) + offsetof(Currencies1Packet::PacketData, padding01);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CURRENCIES_1 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CURRENCIES_1 packet self-test failed: " << label << " got";
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
void putLE16(std::array<uint8, Size>& buffer, std::size_t offset, std::uint16_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
}

template <std::size_t Size>
void putLE32(std::array<uint8, Size>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset]     = static_cast<uint8>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    buffer[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    buffer[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

template <std::size_t Size>
void putLE64(std::array<uint8, Size>& buffer, std::size_t offset, std::uint64_t value)
{
    for (std::size_t i = 0; i < 8; ++i)
    {
        buffer[offset + i] = static_cast<uint8>((value >> (i * 8)) & 0xFF);
    }
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_CURRENCIES_1), 0x113, "CURRENCIES_1 packet id") && ok;
    ok      = expectEqualUInt(currencies1PacketDataSize, 248, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(currencies1PacketSize, 252, "packet size") && ok;
    ok      = expectEqualUInt(currencies1ConquestSandoriaOffset, 4, "conquest_points_sandoria offset") && ok;
    ok      = expectEqualUInt(currencies1BeastmanSealOffset, 16, "beastmans_seals_stored offset") && ok;
    ok      = expectEqualUInt(currencies1GuildFishingOffset, 32, "guild_points_fishing offset") && ok;
    ok      = expectEqualUInt(currencies1FewellFireOffset, 72, "synergy_fewell_fire offset") && ok;
    ok      = expectEqualUInt(currencies1DailyTallyOffset, 94, "daily_tally offset") && ok;
    ok      = expectEqualUInt(currencies1Unknown67Offset, 103, "unknown67 offset") && ok;
    ok      = expectEqualUInt(currencies1LegionPointsOffset, 112, "legion_points offset") && ok;
    ok      = expectEqualUInt(currencies1Trophy5Offset, 184, "echelon_battle_trophies_5th offset") && ok;
    ok      = expectEqualUInt(currencies1TraverserOffset, 192, "traverser_stones offset") && ok;
    ok      = expectEqualUInt(currencies1Rems1Offset, 206, "rems_tale_chapters_1_stored offset") && ok;
    ok      = expectEqualUInt(currencies1PlansOffset, 216, "bloodshed_plans_stored offset") && ok;
    ok      = expectEqualUInt(currencies1ReclamationOffset, 224, "reclamation_marks offset") && ok;
    ok      = expectEqualUInt(currencies1Padding00Offset, 226, "padding00 offset") && ok;
    ok      = expectEqualUInt(currencies1UnityOffset, 228, "unity_accolades offset") && ok;
    ok      = expectEqualUInt(currencies1CrystalsOffset, 232, "fire_crystals_stored offset") && ok;
    ok      = expectEqualUInt(currencies1DeedsOffset, 248, "deeds offset") && ok;
    ok      = expectEqualUInt(currencies1Padding01Offset, 250, "padding01 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = Currencies1Packet::PacketData{};

    data.conquest_points_sandoria = 0x01020304;
    data.conquest_points_bastok   = 0x05060708;
    data.conquest_points_windurst = 0x11121314;

    data.beastmans_seals_stored        = 0x1516;
    data.kindreds_seals_stored         = 0x1718;
    data.kindreds_crests_stored        = 0x191A;
    data.high_kindreds_crests_stored   = 0x1B1C;
    data.sacred_kindreds_crests_stored = 0x1D1E;
    data.ancient_beastcoins_stored     = 0x1F20;
    data.valor_points                  = 0x2122;
    data.scylds                        = 0x2324;

    data.guild_points_fishing      = 0x25262728;
    data.guild_points_woodworking  = 0x292A2B2C;
    data.guild_points_smithing     = 0x2D2E2F30;
    data.guild_points_goldsmithing = 0x31323334;
    data.guild_points_weaving      = 0x35363738;
    data.guild_points_leathercraft = 0x393A3B3C;
    data.guild_points_bonecraft    = 0x3D3E3F40;
    data.guild_points_alchemy      = 0x41424344;
    data.guild_points_cooking      = 0x45464748;
    data.cinders                   = 0x494A4B4C;

    data.synergy_fewell_fire      = 0x51;
    data.synergy_fewell_ice       = 0x52;
    data.synergy_fewell_wind      = 0x53;
    data.synergy_fewell_earth     = 0x54;
    data.synergy_fewell_lightning = 0x55;
    data.synergy_fewell_water     = 0x56;
    data.synergy_fewell_light     = 0x57;
    data.synergy_fewell_dark      = 0x58;

    data.ballista_points          = 0x12131415;
    data.fellow_points            = 0x16171819;
    data.chocobucks_sandoria_team = 0x1A1B;
    data.chocobucks_bastok_team   = 0x1C1D;
    data.chocobucks_windurst_team = 0x1E1F;
    data.daily_tally              = 0x2021;
    data.research_marks           = 0x22232425;
    data.wizened_tunnel_worms     = 0x26;
    data.wizened_morion_worms     = 0x27;
    data.wizened_phantom_worms    = 0x28;
    data.unknown67                = 0x29;
    data.moblin_marbles           = 0x2A2B2C2D;
    data.infamy                   = 0x2E2F;
    data.prestige                 = 0x3031;

    data.legion_points              = 0x32333435;
    data.sparks_of_eminence         = 0x36373839;
    data.shining_stars              = 0x3A3B3C3D;
    data.imperial_standing          = 0x3E3F4041;
    data.assault_points_l_sanctum   = 0x42434445;
    data.assault_points_mjtg        = 0x46474849;
    data.assault_points_l_cavern    = 0x4A4B4C4D;
    data.assault_points_periqia     = 0x4E4F5051;
    data.assault_points_ilrusi_atoll = 0x52535455;
    data.tokens                     = 0x56575859;
    data.zeni                       = 0x5A5B5C5D;
    data.jettons                    = 0x5E5F6061;
    data.therion_ichor              = 0x62636465;
    data.allied_notes               = 0x66676869;

    data.copper_aman_vouchers_stored = 0x6A6B;
    data.login_points                = 0x6C6D;
    data.cruor                       = 0x10111213;
    data.resistance_credits          = 0x14151617;
    data.dominion_notes              = 0x18191A1B;

    data.echelon_battle_trophies_5th = 0x71;
    data.echelon_battle_trophies_4th = 0x72;
    data.echelon_battle_trophies_3rd = 0x73;
    data.echelon_battle_trophies_2nd = 0x74;
    data.echelon_battle_trophies_1st = 0x75;
    data.cave_conservation_points    = 0x76;
    data.imperial_army_id_tags       = 0x77;
    data.op_credits                  = 0x78;

    data.traverser_stones       = 0x1C1D1E1F;
    data.voidstones             = 0x20212223;
    data.kupofrieds_corundums   = 0x24252627;
    data.moblin_pheromone_sacks = 0x79;
    data.unknownCD              = 0x7A;
    data.rems_tale_chapters_1_stored  = 0x31;
    data.rems_tale_chapters_2_stored  = 0x32;
    data.rems_tale_chapters_3_stored  = 0x33;
    data.rems_tale_chapters_4_stored  = 0x34;
    data.rems_tale_chapters_5_stored  = 0x35;
    data.rems_tale_chapters_6_stored  = 0x36;
    data.rems_tale_chapters_7_stored  = 0x37;
    data.rems_tale_chapters_8_stored  = 0x38;
    data.rems_tale_chapters_9_stored  = 0x39;
    data.rems_tale_chapters_10_stored = 0x3A;

    data.bloodshed_plans_stored   = 0x12A;
    data.umbrage_plans_stored     = 0x055;
    data.ritualistic_plans_stored = 0x1AB;
    data.tutelary_plans_stored    = 0x101;
    data.primacy_plans_stored     = 0x1FF;
    data.unused                   = 0;

    data.reclamation_marks = 0x3B3C;
    data.padding00         = 0x3D3E;
    data.unity_accolades   = 0x3F404142;

    data.fire_crystals_stored      = 0x4344;
    data.ice_crystals_stored       = 0x4546;
    data.wind_crystals_stored      = 0x4748;
    data.earth_crystals_stored     = 0x494A;
    data.lightning_crystals_stored = 0x4B4C;
    data.water_crystals_stored     = 0x4D4E;
    data.light_crystals_stored     = 0x4F50;
    data.dark_crystals_stored      = 0x5152;
    data.deeds                     = 0x5354;
    data.padding01                 = 0x5556;

    auto expected = std::array<uint8, currencies1PacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE32(expected, 4, 0x05060708);
    putLE32(expected, 8, 0x11121314);
    putLE16(expected, 12, 0x1516);
    putLE16(expected, 14, 0x1718);
    putLE16(expected, 16, 0x191A);
    putLE16(expected, 18, 0x1B1C);
    putLE16(expected, 20, 0x1D1E);
    putLE16(expected, 22, 0x1F20);
    putLE16(expected, 24, 0x2122);
    putLE16(expected, 26, 0x2324);
    putLE32(expected, 28, 0x25262728);
    putLE32(expected, 32, 0x292A2B2C);
    putLE32(expected, 36, 0x2D2E2F30);
    putLE32(expected, 40, 0x31323334);
    putLE32(expected, 44, 0x35363738);
    putLE32(expected, 48, 0x393A3B3C);
    putLE32(expected, 52, 0x3D3E3F40);
    putLE32(expected, 56, 0x41424344);
    putLE32(expected, 60, 0x45464748);
    putLE32(expected, 64, 0x494A4B4C);
    for (std::size_t i = 68; i <= 75; ++i)
    {
        expected[i] = static_cast<uint8>(0x51 + i - 68);
    }
    putLE32(expected, 76, 0x12131415);
    putLE32(expected, 80, 0x16171819);
    putLE16(expected, 84, 0x1A1B);
    putLE16(expected, 86, 0x1C1D);
    putLE16(expected, 88, 0x1E1F);
    putLE16(expected, 90, 0x2021);
    putLE32(expected, 92, 0x22232425);
    expected[96] = 0x26;
    expected[97] = 0x27;
    expected[98] = 0x28;
    expected[99] = 0x29;
    putLE32(expected, 100, 0x2A2B2C2D);
    putLE16(expected, 104, 0x2E2F);
    putLE16(expected, 106, 0x3031);
    putLE32(expected, 108, 0x32333435);
    putLE32(expected, 112, 0x36373839);
    putLE32(expected, 116, 0x3A3B3C3D);
    putLE32(expected, 120, 0x3E3F4041);
    putLE32(expected, 124, 0x42434445);
    putLE32(expected, 128, 0x46474849);
    putLE32(expected, 132, 0x4A4B4C4D);
    putLE32(expected, 136, 0x4E4F5051);
    putLE32(expected, 140, 0x52535455);
    putLE32(expected, 144, 0x56575859);
    putLE32(expected, 148, 0x5A5B5C5D);
    putLE32(expected, 152, 0x5E5F6061);
    putLE32(expected, 156, 0x62636465);
    putLE32(expected, 160, 0x66676869);
    putLE16(expected, 164, 0x6A6B);
    putLE16(expected, 166, 0x6C6D);
    putLE32(expected, 168, 0x10111213);
    putLE32(expected, 172, 0x14151617);
    putLE32(expected, 176, 0x18191A1B);
    for (std::size_t i = 180; i <= 187; ++i)
    {
        expected[i] = static_cast<uint8>(0x71 + i - 180);
    }
    putLE32(expected, 188, 0x1C1D1E1F);
    putLE32(expected, 192, 0x20212223);
    putLE32(expected, 196, 0x24252627);
    expected[200] = 0x79;
    expected[201] = 0x7A;
    for (std::size_t i = 202; i <= 211; ++i)
    {
        expected[i] = static_cast<uint8>(0x31 + i - 202);
    }

    const auto plans = (std::uint64_t{0x12A}) |
                       (std::uint64_t{0x055} << 9) |
                       (std::uint64_t{0x1AB} << 18) |
                       (std::uint64_t{0x101} << 27) |
                       (std::uint64_t{0x1FF} << 36);
    putLE64(expected, 212, plans);
    putLE16(expected, 220, 0x3B3C);
    putLE16(expected, 222, 0x3D3E);
    putLE32(expected, 224, 0x3F404142);
    putLE16(expected, 228, 0x4344);
    putLE16(expected, 230, 0x4546);
    putLE16(expected, 232, 0x4748);
    putLE16(expected, 234, 0x494A);
    putLE16(expected, 236, 0x4B4C);
    putLE16(expected, 238, 0x4D4E);
    putLE16(expected, 240, 0x4F50);
    putLE16(expected, 242, 0x5152);
    putLE16(expected, 244, 0x5354);
    putLE16(expected, 246, 0x5556);

    return expectStructBytes(data, expected, "CURRENCIES_1 PacketData bytes");
}

} // namespace

auto runS2CCurrencies1PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
