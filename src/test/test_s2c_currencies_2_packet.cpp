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

#include "test_s2c_currencies_2_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/packets/s2c/0x118_currencies_2.h"

namespace
{

using Currencies2Packet = GP_SERV_COMMAND_CURRENCIES_2;

constexpr auto currencies2PacketDataSize = sizeof(Currencies2Packet::PacketData);
constexpr auto currencies2PacketSize     = sizeof(GP_SERV_HEADER) + currencies2PacketDataSize;

constexpr auto currencies2BayldOffset             = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, bayld);
constexpr auto currencies2KineticUnitsOffset      = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, kinetic_units);
constexpr auto currencies2CoalitionOffset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, coalition_imprimaturs);
constexpr auto currencies2ObsidianOffset          = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, obsidian_fragments);
constexpr auto currencies2MweyaOffset             = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, mweya_plasm_corpuscles);
constexpr auto currencies2GhastlyOffset           = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, ghastly_stones_stored);
constexpr auto currencies2TaupeOffset             = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, taupe_stones_stored);
constexpr auto currencies2MellidoptOffset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, mellidopt_wings_stored);
constexpr auto currencies2EschaSiltOffset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, escha_silt);
constexpr auto currencies2FireCrystalsOffset      = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, fire_crystals_set);
constexpr auto currencies2DarknessSpheresOffset   = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, darkness_spheres_set);
constexpr auto currencies2Padding00Offset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, padding00);
constexpr auto currencies2SilverAMANOffset        = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, silver_aman_vouchers_stored);
constexpr auto currencies2IsAccoladesOffset       = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, is_accolades);
constexpr auto currencies2Padding02Offset         = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, padding02);
constexpr auto currencies2TemenosOffset           = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, temenos_units);
constexpr auto currencies2ApollyonOffset          = sizeof(GP_SERV_HEADER) + offsetof(Currencies2Packet::PacketData, apollyon_units);

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CURRENCIES_2 packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
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
        std::cerr << "s2c CURRENCIES_2 packet self-test failed: " << label << " got";
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

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_CURRENCIES_2), 0x118, "CURRENCIES_2 packet id") && ok;
    ok      = expectEqualUInt(currencies2PacketDataSize, 156, "sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(currencies2PacketSize, 160, "packet size") && ok;
    ok      = expectEqualUInt(currencies2BayldOffset, 4, "bayld offset") && ok;
    ok      = expectEqualUInt(currencies2KineticUnitsOffset, 8, "kinetic_units offset") && ok;
    ok      = expectEqualUInt(currencies2CoalitionOffset, 10, "coalition_imprimaturs offset") && ok;
    ok      = expectEqualUInt(currencies2ObsidianOffset, 12, "obsidian_fragments offset") && ok;
    ok      = expectEqualUInt(currencies2MweyaOffset, 20, "mweya_plasm_corpuscles offset") && ok;
    ok      = expectEqualUInt(currencies2GhastlyOffset, 24, "ghastly_stones_stored offset") && ok;
    ok      = expectEqualUInt(currencies2TaupeOffset, 71, "taupe_stones_stored offset") && ok;
    ok      = expectEqualUInt(currencies2MellidoptOffset, 72, "mellidopt_wings_stored offset") && ok;
    ok      = expectEqualUInt(currencies2EschaSiltOffset, 76, "escha_silt offset") && ok;
    ok      = expectEqualUInt(currencies2FireCrystalsOffset, 100, "fire_crystals_set offset") && ok;
    ok      = expectEqualUInt(currencies2DarknessSpheresOffset, 124, "darkness_spheres_set offset") && ok;
    ok      = expectEqualUInt(currencies2Padding00Offset, 125, "padding00 offset") && ok;
    ok      = expectEqualUInt(currencies2SilverAMANOffset, 128, "silver_aman_vouchers_stored offset") && ok;
    ok      = expectEqualUInt(currencies2IsAccoladesOffset, 148, "is_accolades offset") && ok;
    ok      = expectEqualUInt(currencies2Padding02Offset, 150, "padding02 offset") && ok;
    ok      = expectEqualUInt(currencies2TemenosOffset, 152, "temenos_units offset") && ok;
    ok      = expectEqualUInt(currencies2ApollyonOffset, 156, "apollyon_units offset") && ok;
    ok      = expectEqualUInt(sizeof(Currencies2Packet::PacketData::padding00), 3, "padding00 size") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data = Currencies2Packet::PacketData{};

    data.bayld                         = 0x01020304;
    data.kinetic_units                 = 0x0506;
    data.coalition_imprimaturs         = 0x07;
    data.mystical_canteens             = 0x08;
    data.obsidian_fragments            = 0x11121314;
    data.lebondopt_wings_stored        = 0x1516;
    data.pulchridopt_wings_stored      = 0x1718;
    data.mweya_plasm_corpuscles        = 0x21222324;
    data.ghastly_stones_stored         = 0x25;
    data.ghastly_stones_plus1_stored   = 0x26;
    data.ghastly_stones_plus2_stored   = 0x27;
    data.verdigris_stones_stored       = 0x28;
    data.verdigris_stones_plus1_stored = 0x29;
    data.verdigris_stones_plus2_stored = 0x2A;
    data.wailing_stones_stored         = 0x2B;
    data.wailing_stones_plus1_stored   = 0x2C;
    data.wailing_stones_plus2_stored   = 0x2D;
    data.snowslit_stones_stored        = 0x2E;
    data.snowslit_stones_plus1_stored  = 0x2F;
    data.snowslit_stones_plus2_stored  = 0x30;
    data.snowtip_stones_stored         = 0x31;
    data.snowtip_stones_plus1_stored   = 0x32;
    data.snowtip_stones_plus2_stored   = 0x33;
    data.snowdim_stones_stored         = 0x34;
    data.snowdim_stones_plus1_stored   = 0x35;
    data.snowdim_stones_plus2_stored   = 0x36;
    data.snoworb_stones_stored         = 0x37;
    data.snoworb_stones_plus1_stored   = 0x38;
    data.snoworb_stones_plus2_stored   = 0x39;
    data.leafslit_stones_stored        = 0x3A;
    data.leafslit_stones_plus1_stored  = 0x3B;
    data.leafslit_stones_plus2_stored  = 0x3C;
    data.leaftip_stones_stored         = 0x3D;
    data.leaftip_stones_plus1_stored   = 0x3E;
    data.leaftip_stones_plus2_stored   = 0x3F;
    data.leafdim_stones_stored         = 0x40;
    data.leafdim_stones_plus1_stored   = 0x41;
    data.leafdim_stones_plus2_stored   = 0x42;
    data.leaforb_stones_stored         = 0x43;
    data.leaforb_stones_plus1_stored   = 0x44;
    data.leaforb_stones_plus2_stored   = 0x45;
    data.duskslit_stones_stored        = 0x46;
    data.duskslit_stones_plus1_stored  = 0x47;
    data.duskslit_stones_plus2_stored  = 0x48;
    data.dusktip_stones_stored         = 0x49;
    data.dusktip_stones_plus1_stored   = 0x4A;
    data.dusktip_stones_plus2_stored   = 0x4B;
    data.duskdim_stones_stored         = 0x4C;
    data.duskdim_stones_plus1_stored   = 0x4D;
    data.duskdim_stones_plus2_stored   = 0x4E;
    data.duskorb_stones_stored         = 0x4F;
    data.duskorb_stones_plus1_stored   = 0x50;
    data.duskorb_stones_plus2_stored   = 0x51;
    data.pellucid_stones_stored        = 0x52;
    data.fern_stones_stored            = 0x53;
    data.taupe_stones_stored           = 0x54;
    data.mellidopt_wings_stored        = 0x2728;
    data.escha_beads                   = 0x292A;
    data.escha_silt                    = 0x31323334;
    data.potpourri                     = 0x35363738;
    data.hallmarks                     = 0x41424344;
    data.total_hallmarks               = 0x45464748;
    data.badges_of_gallantry           = 0x51525354;
    data.crafter_points                = 0x55565758;
    data.fire_crystals_set             = 0x61;
    data.ice_crystals_set              = 0x62;
    data.wind_crystals_set             = 0x63;
    data.earth_crystals_set            = 0x64;
    data.lightning_crystals_set        = 0x65;
    data.water_crystals_set            = 0x66;
    data.light_crystals_set            = 0x67;
    data.dark_crystals_set             = 0x68;
    data.mc_i_sr01s_set                = 0x69;
    data.mc_i_sr02s_set                = 0x6A;
    data.mc_i_sr03s_set                = 0x6B;
    data.liquefactions_spheres_set     = 0x6C;
    data.induration_spheres_set        = 0x6D;
    data.dentonation_spheres_set       = 0x6E;
    data.scission_spheres_set          = 0x6F;
    data.impaction_spheres_set         = 0x70;
    data.reverberation_spheres_set     = 0x71;
    data.transfixion_spheres_set       = 0x72;
    data.compression_spheres_set       = 0x73;
    data.fusion_spheres_set            = 0x74;
    data.distortion_spheres_set        = 0x75;
    data.fragmentation_spheres_set     = 0x76;
    data.gravitation_spheres_set       = 0x77;
    data.light_spheres_set             = 0x78;
    data.darkness_spheres_set          = 0x79;
    data.padding00[2]                  = 0x63;
    data.silver_aman_vouchers_stored   = 0x64656667;
    data.domain_points                 = 0x68696A6B;
    data.domain_points_earned_today    = 0x6C6D6E6F;
    data.mog_segments                  = 0x70717273;
    data.gallimaufry                   = 0x74757677;
    data.is_accolades                  = 0x7879;
    data.padding02                     = 0x7A7B;
    data.temenos_units                 = 0x7C7D7E7F;
    data.apollyon_units                = 0x01030507;

    auto expected = std::array<uint8, currencies2PacketDataSize>{};
    putLE32(expected, 0, 0x01020304);
    putLE16(expected, 4, 0x0506);
    expected[6] = 0x07;
    expected[7] = 0x08;
    putLE32(expected, 8, 0x11121314);
    putLE16(expected, 12, 0x1516);
    putLE16(expected, 14, 0x1718);
    putLE32(expected, 16, 0x21222324);
    for (std::size_t i = 20; i <= 67; ++i)
    {
        expected[i] = static_cast<uint8>(0x25 + i - 20);
    }
    putLE16(expected, 68, 0x2728);
    putLE16(expected, 70, 0x292A);
    putLE32(expected, 72, 0x31323334);
    putLE32(expected, 76, 0x35363738);
    putLE32(expected, 80, 0x41424344);
    putLE32(expected, 84, 0x45464748);
    putLE32(expected, 88, 0x51525354);
    putLE32(expected, 92, 0x55565758);
    for (std::size_t i = 96; i <= 120; ++i)
    {
        expected[i] = static_cast<uint8>(0x61 + i - 96);
    }
    expected[123] = 0x63;
    putLE32(expected, 124, 0x64656667);
    putLE32(expected, 128, 0x68696A6B);
    putLE32(expected, 132, 0x6C6D6E6F);
    putLE32(expected, 136, 0x70717273);
    putLE32(expected, 140, 0x74757677);
    putLE16(expected, 144, 0x7879);
    putLE16(expected, 146, 0x7A7B);
    putLE32(expected, 148, 0x7C7D7E7F);
    putLE32(expected, 152, 0x01030507);

    return expectStructBytes(data, expected, "CURRENCIES_2 PacketData bytes");
}

} // namespace

auto runS2CCurrencies2PacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    return ok;
}
