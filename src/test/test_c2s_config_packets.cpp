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

#include "test_c2s_config_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0db_config_language.h"
#include "map/packets/c2s/0x0dc_config.h"
#include "map/packets/c2s/config_language_runtime.h"

namespace
{

using ConfigLanguageBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CONFIG_LANGUAGE)>;
using ConfigBytes         = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_CONFIG)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s config packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s config packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s config packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s config packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s config packet self-test failed: " << label << " got";
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

auto expectValid(const PacketValidationResult& result, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectTrue(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), "", label + " error string") && ok;
    return ok;
}

auto expectInvalidError(const PacketValidationResult& result, const std::string& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expectFalse(result.valid(), label + " valid") && ok;
    ok      = expectEqualString(result.errorString(), expected, label + " error string") && ok;
    return ok;
}

template <typename Packet>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, sizeof(Packet)>
{
    auto bytes = std::array<std::uint8_t, sizeof(Packet)>{};
    std::memcpy(bytes.data(), &packet, bytes.size());
    return bytes;
}

auto validateConfigLanguagePure(std::uint8_t kind, std::uint8_t unknown00, std::uint8_t unknown01) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND>(kind)
        .mustEqual(unknown00, 0, "unknown00 must be 0")
        .mustEqual(unknown01, 0, "unknown00 must be 0");
    return validator;
}

auto validateConfigPure(std::uint8_t setFlg) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.oneOf<GP_CLI_COMMAND_CONFIG_SETFLG>(setFlg);
    return validator;
}

auto makeConfigLanguagePacket() -> GP_CLI_COMMAND_CONFIG_LANGUAGE
{
    auto packet         = GP_CLI_COMMAND_CONFIG_LANGUAGE{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CONFIG_LANGUAGE);
    packet.header.size = sizeof(GP_CLI_COMMAND_CONFIG_LANGUAGE) / 4U;
    packet.header.sync = 0xBEEF;
    packet.unknown00   = 0;
    packet.unknown01   = 0;
    packet.Kind        = static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::PartyLanguages);
    packet.padding00   = 0x77;
    packet.ConfigSys[0] = 0x11223344;
    packet.ConfigSys[1] = 0x55667788;
    packet.ConfigSys[2] = 0x99AABBCC;
    packet.padding01[0] = 0x01020304;
    packet.padding01[1] = 0x05060708;
    packet.padding01[2] = 0x090A0B0C;
    packet.padding01[3] = 0x0D0E0F10;
    packet.Param        = static_cast<std::uint32_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_FLAGS::English) |
                   static_cast<std::uint32_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_FLAGS::French);
    return packet;
}

auto makeConfigPacket() -> GP_CLI_COMMAND_CONFIG
{
    auto packet                  = GP_CLI_COMMAND_CONFIG{};
    packet.header.id            = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_CONFIG);
    packet.header.size          = sizeof(GP_CLI_COMMAND_CONFIG) / 4U;
    packet.header.sync          = 0xBEEF;
    packet.InviteFlg            = 1;
    packet.AwayFlg              = 0;
    packet.AnonymityFlg         = 1;
    packet.Language             = 2;
    packet.unused05             = 5;
    packet.unused08             = 1;
    packet.unused09             = 0;
    packet.unused10             = 1;
    packet.unused11             = 2;
    packet.unused13             = 1;
    packet.AutoTargetOffFlg     = 1;
    packet.AutoPartyFlg         = 0;
    packet.unused16             = 0xCC;
    packet.unused24             = 1;
    packet.MentorFlg            = 1;
    packet.NewAdventurerOffFlg  = 0;
    packet.DisplayHeadOffFlg    = 1;
    packet.unused28             = 0;
    packet.RecruitFlg           = 1;
    packet.unused30             = 2;
    packet.unused00             = 0x11223344;
    packet.unused01             = 0x55667788;
    packet.SetFlg               = static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_SETFLG::On);
    packet.padding00[0]         = 0xAA;
    packet.padding00[1]         = 0xBB;
    packet.padding00[2]         = 0xCC;
    return packet;
}

auto testConfigLayoutsAndMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualString(GP_CLI_COMMAND_CONFIG_LANGUAGE::name, "GP_CLI_COMMAND_CONFIG_LANGUAGE", "CONFIG_LANGUAGE name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE::packetId), 0x0DB, "CONFIG_LANGUAGE packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CONFIG_LANGUAGE), 40, "CONFIG_LANGUAGE sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, unknown00), 4, "CONFIG_LANGUAGE unknown00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, unknown01), 5, "CONFIG_LANGUAGE unknown01 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, Kind), 6, "CONFIG_LANGUAGE Kind offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, padding00), 7, "CONFIG_LANGUAGE padding00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, ConfigSys), 8, "CONFIG_LANGUAGE ConfigSys offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CONFIG_LANGUAGE::ConfigSys), 12, "CONFIG_LANGUAGE ConfigSys bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, padding01), 20, "CONFIG_LANGUAGE padding01 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CONFIG_LANGUAGE::padding01), 16, "CONFIG_LANGUAGE padding01 bytes") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG_LANGUAGE, Param), 36, "CONFIG_LANGUAGE Param offset") && ok;

    ok = expectEqualString(GP_CLI_COMMAND_CONFIG::name, "GP_CLI_COMMAND_CONFIG", "CONFIG name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_CONFIG::packetId), 0x0DC, "CONFIG packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CONFIG), 20, "CONFIG sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG, unused00), 8, "CONFIG unused00 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG, unused01), 12, "CONFIG unused01 offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG, SetFlg), 16, "CONFIG SetFlg offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_CONFIG, padding00), 17, "CONFIG padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_CONFIG::padding00), 3, "CONFIG padding00 bytes") && ok;
    return ok;
}

auto testConfigEncodedBytesAndPayloads() -> bool
{
    bool ok = true;

    const auto language = makeConfigLanguagePacket();
    ok = expectBytes(encodedPacketBytes(language),
                     ConfigLanguageBytes{ 0xDB, 0x14, 0xEF, 0xBE, 0x00, 0x00, 0x01, 0x77,
                                          0x44, 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55,
                                          0xCC, 0xBB, 0xAA, 0x99, 0x04, 0x03, 0x02, 0x01,
                                          0x08, 0x07, 0x06, 0x05, 0x0C, 0x0B, 0x0A, 0x09,
                                          0x10, 0x0F, 0x0E, 0x0D, 0x0A, 0x00, 0x00, 0x00 },
                     "CONFIG_LANGUAGE encoded packet") &&
         ok;
    ok = expectEqualInt(language.ConfigSys[2], 0x99AABBCC, "CONFIG_LANGUAGE ConfigSys payload") && ok;
    ok = expectEqualInt(language.Param, 0x0A, "CONFIG_LANGUAGE Param payload") && ok;

    const auto config = makeConfigPacket();
    ok = expectBytes(encodedPacketBytes(config),
                     ConfigBytes{ 0xDC, 0x0A, 0xEF, 0xBE, 0xB5, 0x75, 0xCC, 0xAB,
                                  0x44, 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55,
                                  0x01, 0xAA, 0xBB, 0xCC },
                     "CONFIG encoded packet") &&
         ok;
    ok = expectEqualInt(config.InviteFlg, 1, "CONFIG InviteFlg payload") && ok;
    ok = expectEqualInt(config.Language, 2, "CONFIG Language payload") && ok;
    ok = expectEqualInt(config.unused05, 5, "CONFIG unused05 payload") && ok;
    ok = expectEqualInt(config.AutoTargetOffFlg, 1, "CONFIG AutoTargetOffFlg payload") && ok;
    ok = expectEqualInt(config.unused16, 0xCC, "CONFIG unused16 payload") && ok;
    ok = expectEqualInt(config.RecruitFlg, 1, "CONFIG RecruitFlg payload") && ok;
    ok = expectEqualInt(config.unused30, 2, "CONFIG unused30 payload") && ok;
    ok = expectEqualInt(config.SetFlg, 1, "CONFIG SetFlg payload") && ok;
    return ok;
}

auto testConfigConstantsAndValidation() -> bool
{
    bool ok = true;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::SearchLanguage), 0, "CONFIG_LANGUAGE_KIND::SearchLanguage") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::PartyLanguages), 1, "CONFIG_LANGUAGE_KIND::PartyLanguages") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_FLAGS::Japanese), 0x01, "CONFIG_LANGUAGE_FLAGS::Japanese") && ok;
    ok = expectEqualInt(static_cast<std::uint32_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_FLAGS::Other), 0x10, "CONFIG_LANGUAGE_FLAGS::Other") && ok;
    ok = expectValid(validateConfigLanguagePure(0, 0, 0), "CONFIG_LANGUAGE search validation") && ok;
    ok = expectValid(validateConfigLanguagePure(1, 0, 0), "CONFIG_LANGUAGE party validation") && ok;
    ok = expectInvalidError(validateConfigLanguagePure(2, 0, 0), "2 not a valid GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND value.", "CONFIG_LANGUAGE invalid kind validation") && ok;
    ok = expectInvalidError(validateConfigLanguagePure(0, 1, 0), "unknown00 must be 0", "CONFIG_LANGUAGE unknown00 validation") && ok;
    ok = expectInvalidError(validateConfigLanguagePure(0, 0, 1), "unknown00 must be 0", "CONFIG_LANGUAGE unknown01 validation") && ok;
    ok = expectInvalidError(validateConfigLanguagePure(2, 1, 1), "2 not a valid GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND value.", "CONFIG_LANGUAGE validation order") && ok;

    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_SETFLG::On), 1, "CONFIG_SETFLG::On") && ok;
    ok = expectEqualInt(static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_SETFLG::Off), 2, "CONFIG_SETFLG::Off") && ok;
    ok = expectValid(validateConfigPure(1), "CONFIG SetFlg on validation") && ok;
    ok = expectValid(validateConfigPure(2), "CONFIG SetFlg off validation") && ok;
    ok = expectInvalidError(validateConfigPure(0), "0 not a valid GP_CLI_COMMAND_CONFIG_SETFLG value.", "CONFIG invalid SetFlg zero validation") && ok;
    ok = expectInvalidError(validateConfigPure(3), "3 not a valid GP_CLI_COMMAND_CONFIG_SETFLG value.", "CONFIG invalid SetFlg three validation") && ok;
    return ok;
}

auto testConfigLanguageRuntimePlan() -> bool
{
    bool ok = true;

    auto state = configlanguage::RuntimeState{
        .playerConfig   = 1,
        .chatFilter1    = 2,
        .chatFilter2    = 3,
        .partyLanguages = 4,
    };

    auto search = GP_CLI_COMMAND_CONFIG_LANGUAGE{};
    search.Kind = static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::SearchLanguage);
    search.ConfigSys[0] = 1;
    search.ConfigSys[1] = 2;
    search.ConfigSys[2] = 3;
    auto plan = configlanguage::PlanFor(search.Kind, search.ConfigSys, search.Param, state);
    ok        = expectFalse(plan.updatePlayerConfig, "CONFIG_LANGUAGE unchanged search player config") && ok;
    ok        = expectFalse(plan.updateChatFilters, "CONFIG_LANGUAGE unchanged search chat filters") && ok;
    ok        = expectTrue(plan.sendConfig, "CONFIG_LANGUAGE unchanged search sends config") && ok;
    ok        = expectTrue(plan.sendCharStatus, "CONFIG_LANGUAGE unchanged search sends status") && ok;

    search.ConfigSys[0] = 9;
    plan                = configlanguage::PlanFor(search.Kind, search.ConfigSys, search.Param, state);
    ok                  = expectTrue(plan.updatePlayerConfig, "CONFIG_LANGUAGE changed search updates player config") && ok;
    ok                  = expectTrue(plan.savePlayerSettings, "CONFIG_LANGUAGE changed search saves player settings") && ok;
    ok                  = expectFalse(plan.updateChatFilters, "CONFIG_LANGUAGE player-only search does not update chat filters") && ok;
    ok                  = expectEqualInt(plan.playerConfig, 9, "CONFIG_LANGUAGE changed search player config value") && ok;

    search.ConfigSys[0] = 1;
    search.ConfigSys[1] = 8;
    plan                = configlanguage::PlanFor(search.Kind, search.ConfigSys, search.Param, state);
    ok                  = expectFalse(plan.updatePlayerConfig, "CONFIG_LANGUAGE filter-only search does not update player config") && ok;
    ok                  = expectTrue(plan.updateChatFilters, "CONFIG_LANGUAGE changed search updates both chat filters") && ok;
    ok                  = expectTrue(plan.saveChatFilterFlags, "CONFIG_LANGUAGE changed search saves chat filters") && ok;
    ok                  = expectEqualInt(plan.chatFilter1, 8, "CONFIG_LANGUAGE changed search chat filter one value") && ok;
    ok                  = expectEqualInt(plan.chatFilter2, 3, "CONFIG_LANGUAGE changed search chat filter two value") && ok;

    auto party  = GP_CLI_COMMAND_CONFIG_LANGUAGE{};
    party.Kind  = static_cast<std::uint8_t>(GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::PartyLanguages);
    party.Param = 4;
    plan        = configlanguage::PlanFor(party.Kind, party.ConfigSys, party.Param, state);
    ok          = expectFalse(plan.updatePartyLanguages, "CONFIG_LANGUAGE unchanged party languages") && ok;
    ok          = expectTrue(plan.sendConfig, "CONFIG_LANGUAGE unchanged party sends config") && ok;
    ok          = expectTrue(plan.sendCharStatus, "CONFIG_LANGUAGE unchanged party sends status") && ok;

    party.Param = 7;
    plan        = configlanguage::PlanFor(party.Kind, party.ConfigSys, party.Param, state);
    ok          = expectTrue(plan.updatePartyLanguages, "CONFIG_LANGUAGE changed party updates languages") && ok;
    ok          = expectTrue(plan.saveLanguages, "CONFIG_LANGUAGE changed party saves languages") && ok;
    ok          = expectEqualInt(plan.partyLanguages, 7, "CONFIG_LANGUAGE changed party language value") && ok;

    party.Param = 0x01000004;
    plan        = configlanguage::PlanFor(party.Kind, party.ConfigSys, party.Param, state);
    ok          = expectTrue(plan.updatePartyLanguages, "CONFIG_LANGUAGE party high bits trigger update") && ok;
    ok          = expectTrue(plan.saveLanguages, "CONFIG_LANGUAGE party high bits trigger save") && ok;
    ok          = expectEqualInt(plan.partyLanguages, 4, "CONFIG_LANGUAGE party high bits narrow to byte") && ok;
    return ok;
}

} // namespace

auto runC2SConfigPacketSelfTests() -> bool
{
    return testConfigLayoutsAndMetadata() &&
           testConfigEncodedBytesAndPayloads() &&
           testConfigConstantsAndValidation() &&
           testConfigLanguageRuntimePlan();
}
