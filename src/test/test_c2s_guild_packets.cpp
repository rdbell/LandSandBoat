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

#include "test_c2s_guild_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x0aa_guild_buy.h"
#include "map/packets/c2s/0x0ab_guild_buylist.h"
#include "map/packets/c2s/0x0ac_guild_sell.h"
#include "map/packets/c2s/0x0ad_guild_selllist.h"

namespace
{

using GuildBuyBytes      = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GUILD_BUY)>;
using GuildBuyListBytes  = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GUILD_BUYLIST)>;
using GuildSellBytes     = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GUILD_SELL)>;
using GuildSellListBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_GUILD_SELLLIST)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s guild packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s guild packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s guild packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s guild packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s guild packet self-test failed: " << label << " got";
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

template <typename Packet, std::size_t N>
auto encodedPacketBytes(const Packet& packet) -> std::array<std::uint8_t, N>
{
    auto bytes = std::array<std::uint8_t, N>{};
    std::memcpy(bytes.data(), &packet, sizeof(packet));
    return bytes;
}

auto makeGuildBuyPacket() -> GP_CLI_COMMAND_GUILD_BUY
{
    auto packet               = GP_CLI_COMMAND_GUILD_BUY{};
    packet.header.id         = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GUILD_BUY);
    packet.header.size       = sizeof(GP_CLI_COMMAND_GUILD_BUY) / 4U;
    packet.header.sync       = 0xBEEF;
    packet.ItemNo            = 0x1122;
    packet.PropertyItemIndex = 0;
    packet.ItemNum           = 0x33;
    return packet;
}

auto makeGuildBuyListPacket() -> GP_CLI_COMMAND_GUILD_BUYLIST
{
    auto packet         = GP_CLI_COMMAND_GUILD_BUYLIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GUILD_BUYLIST);
    packet.header.size = sizeof(GP_CLI_COMMAND_GUILD_BUYLIST) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto makeGuildSellPacket() -> GP_CLI_COMMAND_GUILD_SELL
{
    auto packet               = GP_CLI_COMMAND_GUILD_SELL{};
    packet.header.id         = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GUILD_SELL);
    packet.header.size       = sizeof(GP_CLI_COMMAND_GUILD_SELL) / 4U;
    packet.header.sync       = 0xBEEF;
    packet.ItemNo            = 0x4455;
    packet.PropertyItemIndex = 0x66;
    packet.ItemNum           = 0x77;
    return packet;
}

auto makeGuildSellListPacket() -> GP_CLI_COMMAND_GUILD_SELLLIST
{
    auto packet         = GP_CLI_COMMAND_GUILD_SELLLIST{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_GUILD_SELLLIST);
    packet.header.size = sizeof(GP_CLI_COMMAND_GUILD_SELLLIST) / 4U;
    packet.header.sync = 0xBEEF;
    return packet;
}

auto validateGuildBuyPure(std::uint8_t itemNum, std::uint8_t propertyItemIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("ItemNum", itemNum, 1, 99)
        .mustEqual(propertyItemIndex, 0, "PropertyItemIndex not 0");
    return validator;
}

auto validateGuildBuyListPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateGuildSellPure(std::uint8_t itemNum) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.range("ItemNum", itemNum, 1, 99);
    return validator;
}

auto validateGuildSellListPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto testGuildBuyLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeGuildBuyPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_GUILD_BUY::name, "GP_CLI_COMMAND_GUILD_BUY", "GUILD_BUY name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GUILD_BUY::packetId), 0x0AA, "GUILD_BUY packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GUILD_BUY), 8, "GUILD_BUY sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_BUY, ItemNo), 4, "GUILD_BUY ItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_BUY, PropertyItemIndex), 6, "GUILD_BUY PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_BUY, ItemNum), 7, "GUILD_BUY ItemNum offset") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GUILD_BUY, sizeof(GP_CLI_COMMAND_GUILD_BUY)>(packet),
                     GuildBuyBytes{ 0xAA, 0x04, 0xEF, 0xBE, 0x22, 0x11, 0x00, 0x33 },
                     "GUILD_BUY encoded packet") &&
         ok;
    ok = expectEqualInt(packet.ItemNo, 0x1122, "GUILD_BUY ItemNo") && ok;
    ok = expectEqualInt(packet.PropertyItemIndex, 0, "GUILD_BUY PropertyItemIndex") && ok;
    ok = expectEqualInt(packet.ItemNum, 0x33, "GUILD_BUY ItemNum") && ok;
    return ok;
}

auto testGuildListLayoutMetadataAndPayload() -> bool
{
    const auto buyList  = makeGuildBuyListPacket();
    const auto sellList = makeGuildSellListPacket();
    bool       ok       = true;

    ok = expectEqualString(GP_CLI_COMMAND_GUILD_BUYLIST::name, "GP_CLI_COMMAND_GUILD_BUYLIST", "GUILD_BUYLIST name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GUILD_BUYLIST::packetId), 0x0AB, "GUILD_BUYLIST packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GUILD_BUYLIST), 4, "GUILD_BUYLIST sizeof") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GUILD_BUYLIST, sizeof(GP_CLI_COMMAND_GUILD_BUYLIST)>(buyList),
                     GuildBuyListBytes{ 0xAB, 0x02, 0xEF, 0xBE },
                     "GUILD_BUYLIST encoded packet") &&
         ok;

    ok = expectEqualString(GP_CLI_COMMAND_GUILD_SELLLIST::name, "GP_CLI_COMMAND_GUILD_SELLLIST", "GUILD_SELLLIST name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GUILD_SELLLIST::packetId), 0x0AD, "GUILD_SELLLIST packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GUILD_SELLLIST), 4, "GUILD_SELLLIST sizeof") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GUILD_SELLLIST, sizeof(GP_CLI_COMMAND_GUILD_SELLLIST)>(sellList),
                     GuildSellListBytes{ 0xAD, 0x02, 0xEF, 0xBE },
                     "GUILD_SELLLIST encoded packet") &&
         ok;
    return ok;
}

auto testGuildSellLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeGuildSellPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_GUILD_SELL::name, "GP_CLI_COMMAND_GUILD_SELL", "GUILD_SELL name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_GUILD_SELL::packetId), 0x0AC, "GUILD_SELL packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_GUILD_SELL), 8, "GUILD_SELL sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_SELL, ItemNo), 4, "GUILD_SELL ItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_SELL, PropertyItemIndex), 6, "GUILD_SELL PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_GUILD_SELL, ItemNum), 7, "GUILD_SELL ItemNum offset") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_GUILD_SELL, sizeof(GP_CLI_COMMAND_GUILD_SELL)>(packet),
                     GuildSellBytes{ 0xAC, 0x04, 0xEF, 0xBE, 0x55, 0x44, 0x66, 0x77 },
                     "GUILD_SELL encoded packet") &&
         ok;
    ok = expectEqualInt(packet.ItemNo, 0x4455, "GUILD_SELL ItemNo") && ok;
    ok = expectEqualInt(packet.PropertyItemIndex, 0x66, "GUILD_SELL PropertyItemIndex") && ok;
    ok = expectEqualInt(packet.ItemNum, 0x77, "GUILD_SELL ItemNum") && ok;
    return ok;
}

auto testGuildPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(validateGuildBuyPure(1, 0), "GUILD_BUY lower item count validation") && ok;
    ok      = expectValid(validateGuildBuyPure(99, 0), "GUILD_BUY upper item count validation") && ok;
    ok      = expectInvalidError(validateGuildBuyPure(0, 0), "ItemNum out of range: 0 not in [1, 99]", "GUILD_BUY zero item count validation") && ok;
    ok      = expectInvalidError(validateGuildBuyPure(100, 0), "ItemNum out of range: 100 not in [1, 99]", "GUILD_BUY high item count validation") && ok;
    ok      = expectInvalidError(validateGuildBuyPure(1, 1), "PropertyItemIndex not 0", "GUILD_BUY property index validation") && ok;
    ok      = expectInvalidError(validateGuildBuyPure(0, 1), "ItemNum out of range: 0 not in [1, 99]", "GUILD_BUY validation order") && ok;
    ok      = expectValid(validateGuildBuyListPure(), "GUILD_BUYLIST pure validation") && ok;
    ok      = expectValid(validateGuildSellPure(1), "GUILD_SELL lower item count validation") && ok;
    ok      = expectValid(validateGuildSellPure(99), "GUILD_SELL upper item count validation") && ok;
    ok      = expectInvalidError(validateGuildSellPure(0), "ItemNum out of range: 0 not in [1, 99]", "GUILD_SELL zero item count validation") && ok;
    ok      = expectInvalidError(validateGuildSellPure(100), "ItemNum out of range: 100 not in [1, 99]", "GUILD_SELL high item count validation") && ok;
    ok      = expectValid(validateGuildSellListPure(), "GUILD_SELLLIST pure validation") && ok;
    return ok;
}

} // namespace

auto runC2SGuildPacketSelfTests() -> bool
{
    return testGuildBuyLayoutMetadataAndPayload() &&
           testGuildListLayoutMetadataAndPayload() &&
           testGuildSellLayoutMetadataAndPayload() &&
           testGuildPureValidationFacts();
}
