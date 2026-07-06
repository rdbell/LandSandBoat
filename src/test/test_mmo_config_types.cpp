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

#include "test_mmo_config_types.h"

#include "common/mmo.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo config types self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo config types self-test failed: " << label << " got";
        for (const auto value : actual)
        {
            std::cerr << ' ' << static_cast<unsigned>(value);
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

template <typename T>
auto bytesOf(const T& value) -> std::array<std::uint8_t, sizeof(T)>
{
    std::array<std::uint8_t, sizeof(T)> bytes{};
    std::memcpy(bytes.data(), &value, bytes.size());
    return bytes;
}

auto testSizesAndOffsets() -> bool
{
    bool ok = true;

    ok = expectEqualInt(sizeof(filters1_t), 4, "filters1_t sizeof") && ok;
    ok = expectEqualInt(sizeof(filters2_t), 4, "filters2_t sizeof") && ok;
    ok = expectEqualInt(sizeof(languages_t), 1, "languages_t sizeof") && ok;
    ok = expectEqualInt(sizeof(nameflags_t), 4, "nameflags_t sizeof") && ok;
    ok = expectEqualInt(sizeof(SAVE_CONF), 15, "SAVE_CONF sizeof") && ok;

    SAVE_CONF conf{};
    ok = expectEqualInt(static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&conf.MessageFilter) - reinterpret_cast<std::uintptr_t>(&conf)), 4, "SAVE_CONF MessageFilter offset") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&conf.MessageFilter2) - reinterpret_cast<std::uintptr_t>(&conf)), 8, "SAVE_CONF MessageFilter2 offset") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&conf.PvpFlg) - reinterpret_cast<std::uintptr_t>(&conf)), 12, "SAVE_CONF PvpFlg offset") && ok;
    ok = expectEqualInt(static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&conf.AreaCode) - reinterpret_cast<std::uintptr_t>(&conf)), 14, "SAVE_CONF AreaCode offset") && ok;
    return ok;
}

// Setting a single 1-bit filter must flip exactly one bit in the 32-bit little-endian word.
// bit index 0 -> byte0 0x01, index 7 -> byte0 0x80, index 8 -> byte1 0x01, etc.
template <typename Filter>
auto singleBitBytes(Filter& filter, auto setter) -> std::array<std::uint8_t, 4>
{
    std::memset(&filter, 0, sizeof(filter));
    setter(filter);
    return bytesOf(filter);
}

auto testFilters1BitPositions() -> bool
{
    bool ok = true;

    filters1_t f{};
    auto check = [&](int bit, auto setter, const std::string& label)
    {
        const auto bytes = singleBitBytes(f, setter);
        std::array<std::uint8_t, 4> expected{};
        expected[bit / 8] = static_cast<std::uint8_t>(1u << (bit % 8));
        ok = expectBytes(bytes, expected, label) && ok;
    };

    check(0, [](filters1_t& v) { v.say = 1; }, "filters1.say");
    check(1, [](filters1_t& v) { v.shout = 1; }, "filters1.shout");
    check(2, [](filters1_t& v) { v.unused02 = 1; }, "filters1.unused02");
    check(3, [](filters1_t& v) { v.emotes = 1; }, "filters1.emotes");
    check(4, [](filters1_t& v) { v.special_actions_started_on_by_you = 1; }, "filters1.special_actions_started_on_by_you");
    check(5, [](filters1_t& v) { v.special_action_effects_on_by_you = 1; }, "filters1.special_action_effects_on_by_you");
    check(6, [](filters1_t& v) { v.attacks_by_you = 1; }, "filters1.attacks_by_you");
    check(7, [](filters1_t& v) { v.missed_attacks_by_you = 1; }, "filters1.missed_attacks_by_you");
    check(8, [](filters1_t& v) { v.attacks_you_evade = 1; }, "filters1.attacks_you_evade");
    check(9, [](filters1_t& v) { v.damage_you_take = 1; }, "filters1.damage_you_take");
    check(10, [](filters1_t& v) { v.special_action_effects_on_by_npcs = 1; }, "filters1.special_action_effects_on_by_npcs");
    check(11, [](filters1_t& v) { v.attacks_by_npcs = 1; }, "filters1.attacks_by_npcs");
    check(12, [](filters1_t& v) { v.missed_attacks_by_npcs = 1; }, "filters1.missed_attacks_by_npcs");
    check(13, [](filters1_t& v) { v.special_action_effects_on_by_party = 1; }, "filters1.special_action_effects_on_by_party");
    check(14, [](filters1_t& v) { v.attacks_by_party = 1; }, "filters1.attacks_by_party");
    check(15, [](filters1_t& v) { v.missed_attacks_by_party = 1; }, "filters1.missed_attacks_by_party");
    check(16, [](filters1_t& v) { v.attacks_evaded_by_party = 1; }, "filters1.attacks_evaded_by_party");
    check(17, [](filters1_t& v) { v.damage_taken_by_party = 1; }, "filters1.damage_taken_by_party");
    check(18, [](filters1_t& v) { v.special_action_effects_on_by_allies = 1; }, "filters1.special_action_effects_on_by_allies");
    check(19, [](filters1_t& v) { v.attacks_by_allies = 1; }, "filters1.attacks_by_allies");
    check(20, [](filters1_t& v) { v.missed_attacks_by_allies = 1; }, "filters1.missed_attacks_by_allies");
    check(21, [](filters1_t& v) { v.attacks_evaded_by_allies = 1; }, "filters1.attacks_evaded_by_allies");
    check(22, [](filters1_t& v) { v.damage_taken_by_allies = 1; }, "filters1.damage_taken_by_allies");
    check(23, [](filters1_t& v) { v.special_actions_started_on_by_party = 1; }, "filters1.special_actions_started_on_by_party");
    check(24, [](filters1_t& v) { v.special_actions_started_on_by_allies = 1; }, "filters1.special_actions_started_on_by_allies");
    check(25, [](filters1_t& v) { v.special_actions_started_on_by_npcs = 1; }, "filters1.special_actions_started_on_by_npcs");
    check(26, [](filters1_t& v) { v.others_synthesis_and_fishing_results = 1; }, "filters1.others_synthesis_and_fishing_results");
    check(27, [](filters1_t& v) { v.lot_results = 1; }, "filters1.lot_results");
    check(28, [](filters1_t& v) { v.attacks_by_others = 1; }, "filters1.attacks_by_others");
    check(29, [](filters1_t& v) { v.missed_attacks_by_others = 1; }, "filters1.missed_attacks_by_others");
    check(30, [](filters1_t& v) { v.unused30 = 1; }, "filters1.unused30");
    check(31, [](filters1_t& v) { v.unused31 = 1; }, "filters1.unused31");
    return ok;
}

auto testFilters2BitPositions() -> bool
{
    bool ok = true;

    filters2_t f{};
    auto check = [&](int bit, auto setter, const std::string& label)
    {
        const auto bytes = singleBitBytes(f, setter);
        std::array<std::uint8_t, 4> expected{};
        expected[bit / 8] = static_cast<std::uint8_t>(1u << (bit % 8));
        ok = expectBytes(bytes, expected, label) && ok;
    };

    check(0, [](filters2_t& v) { v.attacks_evaded_by_others = 1; }, "filters2.attacks_evaded_by_others");
    check(1, [](filters2_t& v) { v.damage_taken_by_others = 1; }, "filters2.damage_taken_by_others");
    check(2, [](filters2_t& v) { v.special_action_effects_on_by_others = 1; }, "filters2.special_action_effects_on_by_others");
    check(3, [](filters2_t& v) { v.special_actions_started_on_by_others = 1; }, "filters2.special_actions_started_on_by_others");
    check(4, [](filters2_t& v) { v.attacks_by_foes = 1; }, "filters2.attacks_by_foes");
    check(5, [](filters2_t& v) { v.missed_attacks_by_foes = 1; }, "filters2.missed_attacks_by_foes");
    check(6, [](filters2_t& v) { v.attacks_evaded_by_foes = 1; }, "filters2.attacks_evaded_by_foes");
    check(7, [](filters2_t& v) { v.damage_taken_by_foes = 1; }, "filters2.damage_taken_by_foes");
    check(8, [](filters2_t& v) { v.special_action_effects_on_by_foes = 1; }, "filters2.special_action_effects_on_by_foes");
    check(9, [](filters2_t& v) { v.special_actions_started_on_by_foes = 1; }, "filters2.special_actions_started_on_by_foes");
    check(10, [](filters2_t& v) { v.campaign_related_data = 1; }, "filters2.campaign_related_data");
    check(11, [](filters2_t& v) { v.tell_messages_deemed_spam = 1; }, "filters2.tell_messages_deemed_spam");
    check(12, [](filters2_t& v) { v.shout_yell_messages_deemed_spam = 1; }, "filters2.shout_yell_messages_deemed_spam");
    check(13, [](filters2_t& v) { v.unused13 = 1; }, "filters2.unused13");
    check(14, [](filters2_t& v) { v.unused14 = 1; }, "filters2.unused14");
    check(15, [](filters2_t& v) { v.job_specific_emote = 1; }, "filters2.job_specific_emote");
    check(16, [](filters2_t& v) { v.yell = 1; }, "filters2.yell");
    check(17, [](filters2_t& v) { v.messages_from_alter_egos = 1; }, "filters2.messages_from_alter_egos");
    check(18, [](filters2_t& v) { v.unused18 = 1; }, "filters2.unused18");
    check(19, [](filters2_t& v) { v.assist_j = 1; }, "filters2.assist_j");
    check(20, [](filters2_t& v) { v.assist_e = 1; }, "filters2.assist_e");
    check(21, [](filters2_t& v) { v.unused21 = 1; }, "filters2.unused21");
    check(28, [](filters2_t& v) { v.unused28 = 1; }, "filters2.unused28");
    check(31, [](filters2_t& v) { v.unused31 = 1; }, "filters2.unused31");
    return ok;
}

auto testSaveConfGoldenBytes() -> bool
{
    bool ok = true;

    SAVE_CONF conf{};
    std::memset(&conf, 0, sizeof(conf));
    conf.InviteFlg           = 1;
    conf.AwayFlg             = 0;
    conf.AnonymityFlg        = 1;
    conf.Language            = 2;
    conf.unknown05           = 5;
    conf.unknown08           = 1;
    conf.unknown09           = 0;
    conf.unknown10           = 1;
    conf.SysMesFilterLevel   = 2;
    conf.unknown13           = 1;
    conf.AutoTargetOffFlg    = 1;
    conf.AutoPartyFlg        = 0;
    conf.unknown16           = 0xCC;
    conf.MentorUnlockedFlg   = 1;
    conf.MentorFlg           = 1;
    conf.NewAdventurerOffFlg = 0;
    conf.DisplayHeadOffFlg   = 1;
    conf.unknown28           = 0;
    conf.RecruitFlg          = 1;
    conf.unused              = 2;
    conf.MessageFilter.say      = 1;
    conf.MessageFilter.emotes   = 1;
    conf.MessageFilter2.yell    = 1;
    conf.MessageFilter2.assist_j = 1;
    conf.PvpFlg              = 0x1234;
    conf.AreaCode            = 0xAB;

    ok = expectBytes(bytesOf(conf),
                     std::array<std::uint8_t, 15>{ 0xB5, 0x75, 0xCC, 0xAB, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x34, 0x12, 0xAB },
                     "SAVE_CONF golden bytes") &&
         ok;

    // Round-trip: a zeroed SAVE_CONF is all zero bytes.
    SAVE_CONF zero{};
    std::memset(&zero, 0, sizeof(zero));
    ok = expectBytes(bytesOf(zero),
                     std::array<std::uint8_t, 15>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                     "SAVE_CONF zero bytes") &&
         ok;
    return ok;
}

auto testLanguagesAndNameFlags() -> bool
{
    bool ok = true;

    languages_t langs{};
    std::memset(&langs, 0, sizeof(langs));
    langs.Japanese = 1;
    langs.English  = 1;
    langs.German   = 0;
    langs.French   = 1;
    langs.Other    = 0;
    ok = expectBytes(bytesOf(langs), std::array<std::uint8_t, 1>{ 0x0B }, "languages_t golden bytes") && ok;

    nameflags_t flags{};
    flags.flags = 0xDEADBEEF;
    ok = expectBytes(bytesOf(flags), std::array<std::uint8_t, 4>{ 0xEF, 0xBE, 0xAD, 0xDE }, "nameflags_t little-endian uint32") && ok;

    // The byte1/byte2/byte3/byte4 union members map to ascending storage bytes in
    // declaration order, so byte1 is the lowest address (storage byte 0).
    nameflags_t byByte{};
    std::memset(&byByte, 0, sizeof(byByte));
    byByte.byte1 = 0x42;
    ok = expectBytes(bytesOf(byByte), std::array<std::uint8_t, 4>{ 0x42, 0x00, 0x00, 0x00 }, "nameflags_t byte1 union") && ok;
    return ok;
}

} // namespace

auto runMMOConfigTypesSelfTests() -> bool
{
    return testSizesAndOffsets() &&
           testFilters1BitPositions() &&
           testFilters2BitPositions() &&
           testSaveConfGoldenBytes() &&
           testLanguagesAndNameFlags();
}
