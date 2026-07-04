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

#include "test_c2s_shop_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/0x083_shop_buy.h"
#include "map/packets/c2s/0x084_shop_sell_req.h"
#include "map/packets/c2s/0x085_shop_sell_set.h"

namespace
{

using ShopBuyBytes     = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SHOP_BUY)>;
using ShopSellReqBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SHOP_SELL_REQ)>;
using ShopSellSetBytes = std::array<std::uint8_t, sizeof(GP_CLI_COMMAND_SHOP_SELL_SET)>;

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s shop packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "c2s shop packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "c2s shop packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s shop packet self-test failed: " << label << " got \"" << actual << "\" expected \"" << expected << "\"\n";
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<std::uint8_t, N>& actual, const std::array<std::uint8_t, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s shop packet self-test failed: " << label << " got";
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

auto makeShopBuyPacket() -> GP_CLI_COMMAND_SHOP_BUY
{
    auto packet               = GP_CLI_COMMAND_SHOP_BUY{};
    packet.header.id         = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SHOP_BUY);
    packet.header.size       = sizeof(GP_CLI_COMMAND_SHOP_BUY) / 4U;
    packet.header.sync       = 0xBEEF;
    packet.ItemNum           = 0x11223344;
    packet.ShopNo            = 0x5566;
    packet.ShopItemIndex     = 0x7788;
    packet.PropertyItemIndex = 0;
    packet.padding00[0]      = 0xAA;
    packet.padding00[1]      = 0xBB;
    packet.padding00[2]      = 0xCC;
    return packet;
}

auto makeShopSellReqPacket() -> GP_CLI_COMMAND_SHOP_SELL_REQ
{
    auto packet         = GP_CLI_COMMAND_SHOP_SELL_REQ{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SHOP_SELL_REQ);
    packet.header.size = sizeof(GP_CLI_COMMAND_SHOP_SELL_REQ) / 4U;
    packet.header.sync = 0xBEEF;
    packet.ItemNum     = 0x11223344;
    packet.ItemNo      = 0x5566;
    packet.ItemIndex   = 0x77;
    packet.padding00   = 0x88;
    return packet;
}

auto makeShopSellSetPacket() -> GP_CLI_COMMAND_SHOP_SELL_SET
{
    auto packet         = GP_CLI_COMMAND_SHOP_SELL_SET{};
    packet.header.id   = static_cast<std::uint16_t>(PacketC2S::GP_CLI_COMMAND_SHOP_SELL_SET);
    packet.header.size = sizeof(GP_CLI_COMMAND_SHOP_SELL_SET) / 4U;
    packet.header.sync = 0xBEEF;
    packet.SellFlag    = 1;
    packet.padding00   = 0x3344;
    return packet;
}

auto validateShopBuyPure(std::uint8_t propertyItemIndex) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(propertyItemIndex, 0, "PropertyItemIndex not 0");
    return validator;
}

auto validateShopSellReqPure() -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    return validator;
}

auto validateShopSellSetPure(std::uint16_t sellFlag) -> PacketValidationResult
{
    auto validator = PacketValidator(nullptr);
    validator.mustEqual(sellFlag, 1, "SellFlag not 1");
    return validator;
}

auto testShopBuyLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeShopBuyPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_SHOP_BUY::name, "GP_CLI_COMMAND_SHOP_BUY", "SHOP_BUY name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SHOP_BUY::packetId), 0x083, "SHOP_BUY packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SHOP_BUY), 16, "SHOP_BUY sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_BUY, ItemNum), 4, "SHOP_BUY ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_BUY, ShopNo), 8, "SHOP_BUY ShopNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_BUY, ShopItemIndex), 10, "SHOP_BUY ShopItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_BUY, PropertyItemIndex), 12, "SHOP_BUY PropertyItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_BUY, padding00), 13, "SHOP_BUY padding00 offset") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SHOP_BUY::padding00), 3, "SHOP_BUY padding00 bytes") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SHOP_BUY, sizeof(GP_CLI_COMMAND_SHOP_BUY)>(packet),
                     ShopBuyBytes{ 0x83, 0x08, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11,
                                   0x66, 0x55, 0x88, 0x77, 0x00, 0xAA, 0xBB, 0xCC },
                     "SHOP_BUY encoded packet") &&
         ok;
    ok = expectEqualInt(packet.ItemNum, 0x11223344, "SHOP_BUY ItemNum") && ok;
    ok = expectEqualInt(packet.ShopNo, 0x5566, "SHOP_BUY ShopNo") && ok;
    ok = expectEqualInt(packet.ShopItemIndex, 0x7788, "SHOP_BUY ShopItemIndex") && ok;
    ok = expectEqualInt(packet.PropertyItemIndex, 0, "SHOP_BUY PropertyItemIndex") && ok;
    return ok;
}

auto testShopSellReqLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeShopSellReqPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_SHOP_SELL_REQ::name, "GP_CLI_COMMAND_SHOP_SELL_REQ", "SHOP_SELL_REQ name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SHOP_SELL_REQ::packetId), 0x084, "SHOP_SELL_REQ packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SHOP_SELL_REQ), 12, "SHOP_SELL_REQ sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_REQ, ItemNum), 4, "SHOP_SELL_REQ ItemNum offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_REQ, ItemNo), 8, "SHOP_SELL_REQ ItemNo offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_REQ, ItemIndex), 10, "SHOP_SELL_REQ ItemIndex offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_REQ, padding00), 11, "SHOP_SELL_REQ padding00 offset") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SHOP_SELL_REQ, sizeof(GP_CLI_COMMAND_SHOP_SELL_REQ)>(packet),
                     ShopSellReqBytes{ 0x84, 0x06, 0xEF, 0xBE, 0x44, 0x33, 0x22, 0x11,
                                       0x66, 0x55, 0x77, 0x88 },
                     "SHOP_SELL_REQ encoded packet") &&
         ok;
    ok = expectEqualInt(packet.ItemNum, 0x11223344, "SHOP_SELL_REQ ItemNum") && ok;
    ok = expectEqualInt(packet.ItemNo, 0x5566, "SHOP_SELL_REQ ItemNo") && ok;
    ok = expectEqualInt(packet.ItemIndex, 0x77, "SHOP_SELL_REQ ItemIndex") && ok;
    ok = expectEqualInt(packet.padding00, 0x88, "SHOP_SELL_REQ padding00") && ok;
    return ok;
}

auto testShopSellSetLayoutMetadataAndPayload() -> bool
{
    const auto packet = makeShopSellSetPacket();
    bool       ok     = true;

    ok = expectEqualString(GP_CLI_COMMAND_SHOP_SELL_SET::name, "GP_CLI_COMMAND_SHOP_SELL_SET", "SHOP_SELL_SET name") && ok;
    ok = expectEqualInt(static_cast<std::uint16_t>(GP_CLI_COMMAND_SHOP_SELL_SET::packetId), 0x085, "SHOP_SELL_SET packet id") && ok;
    ok = expectEqualInt(sizeof(GP_CLI_COMMAND_SHOP_SELL_SET), 8, "SHOP_SELL_SET sizeof") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_SET, SellFlag), 4, "SHOP_SELL_SET SellFlag offset") && ok;
    ok = expectEqualInt(offsetof(GP_CLI_COMMAND_SHOP_SELL_SET, padding00), 6, "SHOP_SELL_SET padding00 offset") && ok;
    ok = expectBytes(encodedPacketBytes<GP_CLI_COMMAND_SHOP_SELL_SET, sizeof(GP_CLI_COMMAND_SHOP_SELL_SET)>(packet),
                     ShopSellSetBytes{ 0x85, 0x04, 0xEF, 0xBE, 0x01, 0x00, 0x44, 0x33 },
                     "SHOP_SELL_SET encoded packet") &&
         ok;
    ok = expectEqualInt(packet.SellFlag, 1, "SHOP_SELL_SET SellFlag") && ok;
    ok = expectEqualInt(packet.padding00, 0x3344, "SHOP_SELL_SET padding00") && ok;
    return ok;
}

auto testShopPureValidationFacts() -> bool
{
    bool ok = true;
    ok      = expectValid(validateShopBuyPure(0), "SHOP_BUY PropertyItemIndex zero validation") && ok;
    ok      = expectInvalidError(validateShopBuyPure(1), "PropertyItemIndex not 0", "SHOP_BUY PropertyItemIndex one validation") && ok;
    ok      = expectValid(validateShopSellReqPure(), "SHOP_SELL_REQ pure validation") && ok;
    ok      = expectValid(validateShopSellSetPure(1), "SHOP_SELL_SET SellFlag one validation") && ok;
    ok      = expectInvalidError(validateShopSellSetPure(0), "SellFlag not 1", "SHOP_SELL_SET SellFlag zero validation") && ok;
    ok      = expectInvalidError(validateShopSellSetPure(2), "SellFlag not 1", "SHOP_SELL_SET SellFlag two validation") && ok;
    return ok;
}

} // namespace

auto runC2SShopPacketSelfTests() -> bool
{
    return testShopBuyLayoutMetadataAndPayload() &&
           testShopSellReqLayoutMetadataAndPayload() &&
           testShopSellSetLayoutMetadataAndPayload() &&
           testShopPureValidationFacts();
}
