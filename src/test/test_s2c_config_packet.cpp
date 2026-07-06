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

#include "test_s2c_config_packet.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x0b4_config.h"

namespace
{

using ConfigPacket = GP_SERV_COMMAND_CONFIG;

constexpr auto configConfDataOffset       = sizeof(GP_SERV_HEADER) + offsetof(ConfigPacket::PacketData, ConfData);
constexpr auto configUnknown00Offset      = sizeof(GP_SERV_HEADER) + offsetof(ConfigPacket::PacketData, unknown00);
constexpr auto configPartyLanguagesOffset = sizeof(GP_SERV_HEADER) + offsetof(ConfigPacket::PacketData, PartyLanguages);
constexpr auto configUnknown01Offset      = sizeof(GP_SERV_HEADER) + offsetof(ConfigPacket::PacketData, unknown01);
constexpr auto configUnknown01Len         = sizeof(ConfigPacket::PacketData{}.unknown01);
constexpr auto configPacketDataSize       = sizeof(ConfigPacket::PacketData);
constexpr auto configPacketSize           = sizeof(GP_SERV_HEADER) + configPacketDataSize;

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c CONFIG packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(const uint8* data, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c CONFIG packet self-test failed: " << label << " got";
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
            std::cerr << "s2c CONFIG packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto sampleSaveConf() -> SAVE_CONF
{
    auto conf                    = SAVE_CONF{};
    conf.InviteFlg              = 1;
    conf.AwayFlg                = 0;
    conf.AnonymityFlg           = 1;
    conf.Language               = 2;
    conf.unknown05              = 5;
    conf.unknown08              = 1;
    conf.unknown09              = 0;
    conf.unknown10              = 1;
    conf.SysMesFilterLevel      = 2;
    conf.unknown13              = 1;
    conf.AutoTargetOffFlg       = 1;
    conf.AutoPartyFlg           = 0;
    conf.unknown16              = 0xCC;
    conf.MentorUnlockedFlg      = 1;
    conf.MentorFlg              = 1;
    conf.NewAdventurerOffFlg    = 0;
    conf.DisplayHeadOffFlg      = 1;
    conf.unknown28              = 0;
    conf.RecruitFlg             = 1;
    conf.unused                 = 2;
    conf.MessageFilter.say      = 1;
    conf.MessageFilter.emotes   = 1;
    conf.MessageFilter2.yell    = 1;
    conf.MessageFilter2.assist_j = 1;
    conf.PvpFlg                 = 0x1234;
    conf.AreaCode               = 0xAB;
    return conf;
}

auto sampleSaveConfBytes() -> std::array<uint8, sizeof(SAVE_CONF)>
{
    return {
        0xB5, 0x75, 0xCC, 0xAB,
        0x09, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x09, 0x00,
        0x34, 0x12, 0xAB,
    };
}

auto sampleLanguagesByte() -> uint8
{
    auto languages      = languages_t{};
    languages.Japanese = 1;
    languages.English  = 1;
    languages.French   = 1;

    uint8 storage = 0;
    std::memcpy(&storage, &languages, sizeof(storage));
    return storage;
}

auto testLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_CONFIG), 0x0B4, "CONFIG packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(SAVE_CONF), 15, "sizeof(SAVE_CONF)") && ok;
    ok      = expectEqualUInt(sizeof(languages_t), 1, "sizeof(languages_t)") && ok;
    ok      = expectEqualUInt(configUnknown01Len, 3, "unknown01 length") && ok;
    ok      = expectEqualUInt(configPacketDataSize, 20, "PacketData size") && ok;
    ok      = expectEqualUInt(configPacketSize, 24, "packet size") && ok;
    ok      = expectEqualUInt(configConfDataOffset, 4, "ConfData offset") && ok;
    ok      = expectEqualUInt(configUnknown00Offset, 19, "unknown00 offset") && ok;
    ok      = expectEqualUInt(configPartyLanguagesOffset, 20, "PartyLanguages offset") && ok;
    ok      = expectEqualUInt(configUnknown01Offset, 21, "unknown01 offset") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto data             = ConfigPacket::PacketData{};
    data.ConfData         = sampleSaveConf();
    data.unknown00        = 0x66;
    data.PartyLanguages.Japanese = 1;
    data.PartyLanguages.English  = 1;
    data.PartyLanguages.French   = 1;
    data.unknown01[0]     = 0xAA;
    data.unknown01[1]     = 0xBB;
    data.unknown01[2]     = 0xCC;

    auto expected = std::array<uint8, configPacketDataSize>{};
    const auto confBytes = sampleSaveConfBytes();
    std::memcpy(expected.data(), confBytes.data(), confBytes.size());
    expected[15] = 0x66;
    expected[16] = sampleLanguagesByte();
    expected[17] = 0xAA;
    expected[18] = 0xBB;
    expected[19] = 0xCC;

    return expectBytes(reinterpret_cast<const uint8*>(&data), expected, "PacketData bytes");
}

auto testConstructorCopiesCharacterConfig() -> bool
{
    auto character              = CCharEntity{};
    character.playerConfig     = sampleSaveConf();
    character.search.language  = sampleLanguagesByte();

    auto packet = ConfigPacket(&character);
    packet.setSequence(0xBEEF);

    auto expected = std::array<uint8, configPacketSize>{
        0xB4, 0x0C, 0xEF, 0xBE,
    };
    const auto confBytes = sampleSaveConfBytes();
    std::memcpy(expected.data() + sizeof(GP_SERV_HEADER), confBytes.data(), confBytes.size());
    expected[configPartyLanguagesOffset] = sampleLanguagesByte();

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x0B4, "CONFIG type") && ok;
    ok      = expectEqualUInt(packet.getSize(), configPacketSize, "CONFIG size") && ok;
    ok      = expectBytes(packetData(packet), expected, "encoded CONFIG packet") && ok;
    ok      = expectZeroTail(packet, expected.size(), "CONFIG tail") && ok;
    return ok;
}

} // namespace

auto runS2CConfigPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayout() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testConstructorCopiesCharacterConfig() && ok;
    return ok;
}
