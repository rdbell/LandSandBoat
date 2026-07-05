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

#include "test_s2c_shop_packets.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "map/entities/char_entity.h"
#include "map/packets/s2c/0x03d_shop_sell.h"
#include "map/packets/s2c/0x03e_shop_open.h"
#include "map/packets/s2c/0x03f_shop_buy.h"
#include "map/trade_container.h"

namespace
{

constexpr auto shopSellPriceOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_SELL::PacketData, Price);
constexpr auto shopSellPropertyItemIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_SELL::PacketData, PropertyItemIndex);
constexpr auto shopSellTypeOffset              = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_SELL::PacketData, Type);
constexpr auto shopSellPadding00Offset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_SELL::PacketData, padding00);
constexpr auto shopSellCountOffset             = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_SELL::PacketData, Count);
constexpr auto shopSellPacketDefaultSize       = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SHOP_SELL::PacketData);

constexpr auto shopBuyShopItemIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_BUY::PacketData, ShopItemIndex);
constexpr auto shopBuyBuyStateOffset      = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_BUY::PacketData, BuyState);
constexpr auto shopBuyPadding00Offset     = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_BUY::PacketData, padding00);
constexpr auto shopBuyCountOffset         = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_BUY::PacketData, Count);
constexpr auto shopBuyPacketDefaultSize   = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SHOP_BUY::PacketData);

constexpr auto shopOpenShopListNumOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_OPEN::PacketData, ShopListNum);
constexpr auto shopOpenPadding00Offset   = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_SHOP_OPEN::PacketData, padding00);
constexpr auto shopOpenPacketDefaultSize = sizeof(GP_SERV_HEADER) + sizeof(GP_SERV_COMMAND_SHOP_OPEN::PacketData);

auto packetData(CBasicPacket& packet) -> uint8*
{
    return static_cast<uint8*>(packet);
}

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c shop packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t N>
auto expectBytes(CBasicPacket& packet, const std::array<uint8, N>& expected, const std::string& label) -> bool
{
    const auto* data = packetData(packet);
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        std::cerr << "s2c shop packet self-test failed: " << label << " got";
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
            std::cerr << "s2c shop packet self-test failed: " << label << " byte " << i << " got " << static_cast<unsigned>(data[i]) << " expected 0\n";
            return false;
        }
    }
    return true;
}

auto testShopSellLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_SHOP_SELL::PacketData), 12, "SHOP_SELL sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(shopSellPacketDefaultSize, 16, "SHOP_SELL default size") && ok;
    ok      = expectEqualUInt(shopSellPriceOffset, 4, "SHOP_SELL Price offset") && ok;
    ok      = expectEqualUInt(shopSellPropertyItemIndexOffset, 8, "SHOP_SELL PropertyItemIndex offset") && ok;
    ok      = expectEqualUInt(shopSellTypeOffset, 9, "SHOP_SELL Type offset") && ok;
    ok      = expectEqualUInt(shopSellPadding00Offset, 10, "SHOP_SELL padding00 offset") && ok;
    ok      = expectEqualUInt(shopSellCountOffset, 12, "SHOP_SELL Count offset") && ok;
    return ok;
}

auto testShopBuyLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_SHOP_BUY::PacketData), 8, "SHOP_BUY sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(shopBuyPacketDefaultSize, 12, "SHOP_BUY default size") && ok;
    ok      = expectEqualUInt(shopBuyShopItemIndexOffset, 4, "SHOP_BUY ShopItemIndex offset") && ok;
    ok      = expectEqualUInt(shopBuyBuyStateOffset, 6, "SHOP_BUY BuyState offset") && ok;
    ok      = expectEqualUInt(shopBuyPadding00Offset, 7, "SHOP_BUY padding00 offset") && ok;
    ok      = expectEqualUInt(shopBuyCountOffset, 8, "SHOP_BUY Count offset") && ok;
    return ok;
}

auto testShopOpenLayout() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(sizeof(GP_SERV_COMMAND_SHOP_OPEN::PacketData), 4, "SHOP_OPEN sizeof(PacketData)") && ok;
    ok      = expectEqualUInt(shopOpenPacketDefaultSize, 8, "SHOP_OPEN default size") && ok;
    ok      = expectEqualUInt(shopOpenShopListNumOffset, 4, "SHOP_OPEN ShopListNum offset") && ok;
    ok      = expectEqualUInt(shopOpenPadding00Offset, 6, "SHOP_OPEN padding00 offset") && ok;
    return ok;
}

auto testShopSellConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_SHOP_SELL(0x7F, 0x11223344);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 16>{
        0x3D, 0x08, 0xEF, 0xBE,
        0x44, 0x33, 0x22, 0x11,
        0x7F, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03D, "SHOP_SELL type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "SHOP_SELL size") && ok;
    ok      = expectBytes(packet, expected, "encoded SHOP_SELL prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "SHOP_SELL tail") && ok;
    return ok;
}

auto testShopBuyConstructor() -> bool
{
    auto packet = GP_SERV_COMMAND_SHOP_BUY(0x7F, 0x11223344);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 12>{
        0x3F, 0x06, 0xEF, 0xBE,
        0x7F, 0x00, 0x00, 0x00,
        0x44, 0x33, 0x22, 0x11,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03F, "SHOP_BUY type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "SHOP_BUY size") && ok;
    ok      = expectBytes(packet, expected, "encoded SHOP_BUY prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "SHOP_BUY tail") && ok;
    return ok;
}

auto testShopOpenConstructor() -> bool
{
    auto character = CCharEntity{};
    character.Container->setItemsCount(0x34);

    auto packet = GP_SERV_COMMAND_SHOP_OPEN(&character);
    packet.setSequence(0xBEEF);

    const auto expected = std::array<uint8, 8>{
        0x3E, 0x04, 0xEF, 0xBE,
        0x34, 0x00, 0x00, 0x00,
    };

    bool ok = true;
    ok      = expectEqualUInt(packet.getType(), 0x03E, "SHOP_OPEN type") && ok;
    ok      = expectEqualUInt(packet.getSize(), expected.size(), "SHOP_OPEN size") && ok;
    ok      = expectBytes(packet, expected, "encoded SHOP_OPEN prefix") && ok;
    ok      = expectZeroTail(packet, expected.size(), "SHOP_OPEN tail") && ok;
    return ok;
}

} // namespace

auto runS2CShopPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testShopSellLayout() && ok;
    ok      = testShopBuyLayout() && ok;
    ok      = testShopOpenLayout() && ok;
    ok      = testShopSellConstructor() && ok;
    ok      = testShopBuyConstructor() && ok;
    ok      = testShopOpenConstructor() && ok;
    return ok;
}
