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

#include "test_s2c_bazaar_packets.h"
#include "omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "entities/char_entity.h"
#include "map/packets/s2c/0x105_bazaar_list.h"
#include "map/packets/s2c/0x106_bazaar_buy.h"
#include "map/packets/s2c/0x107_bazaar_close.h"
#include "map/packets/s2c/0x108_bazaar_shopping.h"
#include "map/packets/s2c/0x109_bazaar_sell.h"
#include "map/packets/s2c/0x10a_bazaar_sale.h"

namespace
{

using BazaarList     = GP_SERV_COMMAND_BAZAAR_LIST;
using BazaarBuy      = GP_SERV_COMMAND_BAZAAR_BUY;
using BazaarClose    = GP_SERV_COMMAND_BAZAAR_CLOSE;
using BazaarShopping = GP_SERV_COMMAND_BAZAAR_SHOPPING;
using BazaarSell     = GP_SERV_COMMAND_BAZAAR_SELL;
using BazaarSale     = GP_SERV_COMMAND_BAZAAR_SALE;

constexpr auto bazaarListPriceOffset     = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, Price);
constexpr auto bazaarListItemNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, ItemNum);
constexpr auto bazaarListTaxRateOffset   = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, TaxRate);
constexpr auto bazaarListItemNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, ItemNo);
constexpr auto bazaarListItemIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, ItemIndex);
constexpr auto bazaarListAttrOffset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, Attr);
constexpr auto bazaarListPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(BazaarList::PacketData, padding00);
constexpr auto bazaarListPacketDataSize  = sizeof(BazaarList::PacketData);
constexpr auto bazaarListPacketSize      = sizeof(GP_SERV_HEADER) + bazaarListPacketDataSize;

constexpr auto bazaarBuyStateOffset    = sizeof(GP_SERV_HEADER) + offsetof(BazaarBuy::PacketData, State);
constexpr auto bazaarBuyNameOffset     = sizeof(GP_SERV_HEADER) + offsetof(BazaarBuy::PacketData, sName);
constexpr auto bazaarBuyPacketDataSize = sizeof(BazaarBuy::PacketData);
constexpr auto bazaarBuyPacketSize     = sizeof(GP_SERV_HEADER) + bazaarBuyPacketDataSize;

constexpr auto bazaarCloseNameOffset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarClose::PacketData, sName);
constexpr auto bazaarClosePadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(BazaarClose::PacketData, padding00);
constexpr auto bazaarClosePacketDataSize  = sizeof(BazaarClose::PacketData);
constexpr auto bazaarClosePacketSize      = sizeof(GP_SERV_HEADER) + bazaarClosePacketDataSize;

constexpr auto bazaarShoppingUniqueNoOffset  = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, UniqueNo);
constexpr auto bazaarShoppingStateOffset     = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, State);
constexpr auto bazaarShoppingHideLevelOffset = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, HideLevel);
constexpr auto bazaarShoppingPadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, padding00);
constexpr auto bazaarShoppingActIndexOffset  = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, ActIndex);
constexpr auto bazaarShoppingNameOffset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarShopping::PacketData, sName);
constexpr auto bazaarShoppingPacketDataSize  = sizeof(BazaarShopping::PacketData);
constexpr auto bazaarShoppingPacketSize      = sizeof(GP_SERV_HEADER) + bazaarShoppingPacketDataSize;

constexpr auto bazaarSellUniqueNoOffset       = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, UniqueNo);
constexpr auto bazaarSellItemNumOffset        = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, ItemNum);
constexpr auto bazaarSellActIndexOffset       = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, ActIndex);
constexpr auto bazaarSellBazaarActIndexOffset = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, BazaarActIndex);
constexpr auto bazaarSellNameOffset           = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, sName);
constexpr auto bazaarSellItemIndexOffset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, ItemIndex);
constexpr auto bazaarSellPadding00Offset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarSell::PacketData, padding00);
constexpr auto bazaarSellPacketDataSize       = sizeof(BazaarSell::PacketData);
constexpr auto bazaarSellPacketSize           = sizeof(GP_SERV_HEADER) + bazaarSellPacketDataSize;

constexpr auto bazaarSaleItemNumOffset   = sizeof(GP_SERV_HEADER) + offsetof(BazaarSale::PacketData, ItemNum);
constexpr auto bazaarSaleItemNoOffset    = sizeof(GP_SERV_HEADER) + offsetof(BazaarSale::PacketData, ItemNo);
constexpr auto bazaarSaleNameOffset      = sizeof(GP_SERV_HEADER) + offsetof(BazaarSale::PacketData, sName);
constexpr auto bazaarSalePadding00Offset = sizeof(GP_SERV_HEADER) + offsetof(BazaarSale::PacketData, padding00);
constexpr auto bazaarSalePacketDataSize  = sizeof(BazaarSale::PacketData);
constexpr auto bazaarSalePacketSize      = sizeof(GP_SERV_HEADER) + bazaarSalePacketDataSize;

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c bazaar packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <std::size_t Size>
auto expectBytes(const void* data, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    if (std::memcmp(data, expected.data(), expected.size()) != 0)
    {
        const auto* bytes = static_cast<const uint8*>(data);
        std::cerr << "s2c bazaar packet self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(bytes[i]);
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

auto testLayoutAndEnums() -> bool
{
    bool ok = true;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_LIST), 0x105, "BAZAAR_LIST packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_BUY), 0x106, "BAZAAR_BUY packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_CLOSE), 0x107, "BAZAAR_CLOSE packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_SHOPPING), 0x108, "BAZAAR_SHOPPING packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_SELL), 0x109, "BAZAAR_SELL packet id") && ok;
    ok      = expectEqualUInt(static_cast<std::uint16_t>(PacketS2C::GP_SERV_COMMAND_BAZAAR_SALE), 0x10A, "BAZAAR_SALE packet id") && ok;
    ok      = expectEqualUInt(sizeof(GP_SERV_HEADER), 4, "sizeof(GP_SERV_HEADER)") && ok;

    ok = expectEqualUInt(bazaarListPacketDataSize, 40, "BAZAAR_LIST PacketData size") && ok;
    ok = expectEqualUInt(bazaarListPacketSize, 44, "BAZAAR_LIST packet size") && ok;
    ok = expectEqualUInt(bazaarListPriceOffset, 4, "BAZAAR_LIST Price offset") && ok;
    ok = expectEqualUInt(bazaarListItemNumOffset, 8, "BAZAAR_LIST ItemNum offset") && ok;
    ok = expectEqualUInt(bazaarListTaxRateOffset, 12, "BAZAAR_LIST TaxRate offset") && ok;
    ok = expectEqualUInt(bazaarListItemNoOffset, 14, "BAZAAR_LIST ItemNo offset") && ok;
    ok = expectEqualUInt(bazaarListItemIndexOffset, 16, "BAZAAR_LIST ItemIndex offset") && ok;
    ok = expectEqualUInt(bazaarListAttrOffset, 17, "BAZAAR_LIST Attr offset") && ok;
    ok = expectEqualUInt(bazaarListPadding00Offset, 41, "BAZAAR_LIST padding00 offset") && ok;

    ok = expectEqualUInt(bazaarBuyPacketDataSize, 20, "BAZAAR_BUY PacketData size") && ok;
    ok = expectEqualUInt(bazaarBuyPacketSize, 24, "BAZAAR_BUY packet size") && ok;
    ok = expectEqualUInt(bazaarBuyStateOffset, 4, "BAZAAR_BUY State offset") && ok;
    ok = expectEqualUInt(bazaarBuyNameOffset, 8, "BAZAAR_BUY sName offset") && ok;

    ok = expectEqualUInt(bazaarClosePacketDataSize, 20, "BAZAAR_CLOSE PacketData size") && ok;
    ok = expectEqualUInt(bazaarClosePacketSize, 24, "BAZAAR_CLOSE packet size") && ok;
    ok = expectEqualUInt(bazaarCloseNameOffset, 4, "BAZAAR_CLOSE sName offset") && ok;
    ok = expectEqualUInt(bazaarClosePadding00Offset, 20, "BAZAAR_CLOSE padding00 offset") && ok;

    ok = expectEqualUInt(bazaarShoppingPacketDataSize, 28, "BAZAAR_SHOPPING PacketData size") && ok;
    ok = expectEqualUInt(bazaarShoppingPacketSize, 32, "BAZAAR_SHOPPING packet size") && ok;
    ok = expectEqualUInt(bazaarShoppingUniqueNoOffset, 4, "BAZAAR_SHOPPING UniqueNo offset") && ok;
    ok = expectEqualUInt(bazaarShoppingStateOffset, 8, "BAZAAR_SHOPPING State offset") && ok;
    ok = expectEqualUInt(bazaarShoppingHideLevelOffset, 12, "BAZAAR_SHOPPING HideLevel offset") && ok;
    ok = expectEqualUInt(bazaarShoppingPadding00Offset, 13, "BAZAAR_SHOPPING padding00 offset") && ok;
    ok = expectEqualUInt(bazaarShoppingActIndexOffset, 14, "BAZAAR_SHOPPING ActIndex offset") && ok;
    ok = expectEqualUInt(bazaarShoppingNameOffset, 16, "BAZAAR_SHOPPING sName offset") && ok;

    ok = expectEqualUInt(bazaarSellPacketDataSize, 32, "BAZAAR_SELL PacketData size") && ok;
    ok = expectEqualUInt(bazaarSellPacketSize, 36, "BAZAAR_SELL packet size") && ok;
    ok = expectEqualUInt(bazaarSellUniqueNoOffset, 4, "BAZAAR_SELL UniqueNo offset") && ok;
    ok = expectEqualUInt(bazaarSellItemNumOffset, 8, "BAZAAR_SELL ItemNum offset") && ok;
    ok = expectEqualUInt(bazaarSellActIndexOffset, 12, "BAZAAR_SELL ActIndex offset") && ok;
    ok = expectEqualUInt(bazaarSellBazaarActIndexOffset, 14, "BAZAAR_SELL BazaarActIndex offset") && ok;
    ok = expectEqualUInt(bazaarSellNameOffset, 16, "BAZAAR_SELL sName offset") && ok;
    ok = expectEqualUInt(bazaarSellItemIndexOffset, 32, "BAZAAR_SELL ItemIndex offset") && ok;
    ok = expectEqualUInt(bazaarSellPadding00Offset, 33, "BAZAAR_SELL padding00 offset") && ok;

    ok = expectEqualUInt(bazaarSalePacketDataSize, 24, "BAZAAR_SALE PacketData size") && ok;
    ok = expectEqualUInt(bazaarSalePacketSize, 28, "BAZAAR_SALE packet size") && ok;
    ok = expectEqualUInt(bazaarSaleItemNumOffset, 4, "BAZAAR_SALE ItemNum offset") && ok;
    ok = expectEqualUInt(bazaarSaleItemNoOffset, 8, "BAZAAR_SALE ItemNo offset") && ok;
    ok = expectEqualUInt(bazaarSaleNameOffset, 10, "BAZAAR_SALE sName offset") && ok;
    ok = expectEqualUInt(bazaarSalePadding00Offset, 26, "BAZAAR_SALE padding00 offset") && ok;

    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_BAZAAR_BUY_STATE::OK), 0, "BAZAAR_BUY OK state") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_BAZAAR_BUY_STATE::ERR), 1, "BAZAAR_BUY ERR state") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_BAZAAR_BUY_STATE::END), 2, "BAZAAR_BUY END state") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_BAZAAR_SHOPPING_STATE::Enter), 0, "BAZAAR_SHOPPING Enter state") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_BAZAAR_SHOPPING_STATE::Exit), 1, "BAZAAR_SHOPPING Exit state") && ok;
    ok = expectEqualUInt(static_cast<std::uint32_t>(GP_SERV_COMMAND_BAZAAR_SHOPPING_STATE::End), 2, "BAZAAR_SHOPPING End state") && ok;
    return ok;
}

auto testPacketDataBytes() -> bool
{
    auto list       = BazaarList::PacketData{};
    list.Price     = 0x11223344;
    list.ItemNum   = 0x55667788;
    list.TaxRate   = 0x99AA;
    list.ItemNo    = 0xBBCC;
    list.ItemIndex = 0xDD;
    for (std::size_t i = 0; i < sizeof(list.Attr); ++i)
    {
        list.Attr[i] = static_cast<uint8>(0x10 + i);
    }
    list.padding00[0] = 0xEE;
    list.padding00[1] = 0xF0;
    list.padding00[2] = 0xF1;

    auto buy  = BazaarBuy::PacketData{};
    buy.State = GP_BAZAAR_BUY_STATE::ERR;
    std::memcpy(buy.sName, "SellerName123456", sizeof(buy.sName));

    auto close = BazaarClose::PacketData{};
    std::memcpy(close.sName, "ClosedBazaarName", sizeof(close.sName));
    close.padding00[0] = 0xA1;
    close.padding00[1] = 0xA2;
    close.padding00[2] = 0xA3;
    close.padding00[3] = 0xA4;

    auto shopping        = BazaarShopping::PacketData{};
    shopping.UniqueNo   = 0x12345678;
    shopping.State      = GP_SERV_COMMAND_BAZAAR_SHOPPING_STATE::Exit;
    shopping.HideLevel  = 0x42;
    shopping.padding00  = 0x24;
    shopping.ActIndex   = 0xBEEF;
    std::memcpy(shopping.sName, "ShopperName12345", sizeof(shopping.sName));

    auto sell            = BazaarSell::PacketData{};
    sell.UniqueNo       = 0x01020304;
    sell.ItemNum        = 0x05060708;
    sell.ActIndex       = 0x0A0B;
    sell.BazaarActIndex = 0x0C0D;
    std::memcpy(sell.sName, "BuyerName1234567", sizeof(sell.sName));
    sell.ItemIndex    = 0xEF;
    sell.padding00[0] = 0xFA;
    sell.padding00[1] = 0xFB;
    sell.padding00[2] = 0xFC;

    auto sale    = BazaarSale::PacketData{};
    sale.ItemNum = 0xAABBCCDD;
    sale.ItemNo  = 0x1122;
    std::memcpy(sale.sName, "SaleName12345678", sizeof(sale.sName));
    sale.padding00[0] = 0x33;
    sale.padding00[1] = 0x44;

    bool ok = true;
    ok      = expectBytes(&list, std::array<uint8, bazaarListPacketDataSize>{
        0x44, 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55, 0xAA, 0x99, 0xCC, 0xBB, 0xDD, 0x10, 0x11, 0x12,
        0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
        0x23, 0x24, 0x25, 0x26, 0x27, 0xEE, 0xF0, 0xF1,
    }, "BAZAAR_LIST PacketData bytes") && ok;
    ok = expectBytes(&buy, std::array<uint8, bazaarBuyPacketDataSize>{
        0x01, 0x00, 0x00, 0x00,
        'S', 'e', 'l', 'l', 'e', 'r', 'N', 'a', 'm', 'e', '1', '2', '3', '4', '5', '6',
    }, "BAZAAR_BUY PacketData bytes") && ok;
    ok = expectBytes(&close, std::array<uint8, bazaarClosePacketDataSize>{
        'C', 'l', 'o', 's', 'e', 'd', 'B', 'a', 'z', 'a', 'a', 'r', 'N', 'a', 'm', 'e',
        0xA1, 0xA2, 0xA3, 0xA4,
    }, "BAZAAR_CLOSE PacketData bytes") && ok;
    ok = expectBytes(&shopping, std::array<uint8, bazaarShoppingPacketDataSize>{
        0x78, 0x56, 0x34, 0x12,
        0x01, 0x00, 0x00, 0x00,
        0x42, 0x24, 0xEF, 0xBE,
        'S', 'h', 'o', 'p', 'p', 'e', 'r', 'N', 'a', 'm', 'e', '1', '2', '3', '4', '5',
    }, "BAZAAR_SHOPPING PacketData bytes") && ok;
    ok = expectBytes(&sell, std::array<uint8, bazaarSellPacketDataSize>{
        0x04, 0x03, 0x02, 0x01,
        0x08, 0x07, 0x06, 0x05,
        0x0B, 0x0A, 0x0D, 0x0C,
        'B', 'u', 'y', 'e', 'r', 'N', 'a', 'm', 'e', '1', '2', '3', '4', '5', '6', '7',
        0xEF, 0xFA, 0xFB, 0xFC,
    }, "BAZAAR_SELL PacketData bytes") && ok;
    ok = expectBytes(&sale, std::array<uint8, bazaarSalePacketDataSize>{
        0xDD, 0xCC, 0xBB, 0xAA,
        0x22, 0x11,
        'S', 'a', 'l', 'e', 'N', 'a', 'm', 'e', '1', '2', '3', '4', '5', '6', '7', '8',
        0x33, 0x44,
    }, "BAZAAR_SALE PacketData bytes") && ok;
    return ok;
}

auto testBazaarCloseCharacterConstructor() -> bool
{
    CCharEntity character{};
    character.name = "BazaarOwnerNameThatIsTooLong";

    auto packet = BazaarClose(&character);
    const auto* data = static_cast<const uint8*>(packet);

    const auto expectedName = std::array<uint8, 16>{
        'B', 'a', 'z', 'a', 'a', 'r', 'O', 'w', 'n', 'e', 'r', 'N', 'a', 'm', 'e', 'T',
    };

    bool ok = true;
    ok      = expectBytes(data + bazaarCloseNameOffset, expectedName, "BAZAAR_CLOSE character name truncation") && ok;
    for (std::size_t i = bazaarClosePadding00Offset; i < bazaarClosePacketSize; ++i)
    {
        if (data[i] != 0)
        {
            std::cerr << "s2c bazaar packet self-test failed: BAZAAR_CLOSE constructor padding byte " << i << " got "
                      << static_cast<unsigned>(data[i]) << " expected 0\n";
            ok = false;
        }
    }
    return ok;
}

auto testBazaarShoppingCharacterConstructor() -> bool
{
    CCharEntity character{};
    character.id    = 0x11223344;
    character.targid = 0x5566;
    character.name  = "ShopperNameThatIsTooLong";

    auto packet = BazaarShopping(&character, GP_SERV_COMMAND_BAZAAR_SHOPPING_STATE::Exit);
    const auto* data = static_cast<const uint8*>(packet);
    const auto expectedName = std::array<uint8, 16>{
        'S', 'h', 'o', 'p', 'p', 'e', 'r', 'N', 'a', 'm', 'e', 'T', 'h', 'a', 't', 'I',
    };

    bool ok = true;
    ok      = expectBytes(data + bazaarShoppingUniqueNoOffset, std::array<uint8, 4>{ 0x44, 0x33, 0x22, 0x11 }, "BAZAAR_SHOPPING character UniqueNo") && ok;
    ok      = expectBytes(data + bazaarShoppingStateOffset, std::array<uint8, 4>{ 0x01, 0x00, 0x00, 0x00 }, "BAZAAR_SHOPPING character State") && ok;
    ok      = expectEqualUInt(data[bazaarShoppingHideLevelOffset], 0, "BAZAAR_SHOPPING character HideLevel default") && ok;
    ok      = expectEqualUInt(data[bazaarShoppingPadding00Offset], 0, "BAZAAR_SHOPPING character padding default") && ok;
    ok      = expectBytes(data + bazaarShoppingActIndexOffset, std::array<uint8, 2>{ 0x66, 0x55 }, "BAZAAR_SHOPPING character ActIndex") && ok;
    ok      = expectBytes(data + bazaarShoppingNameOffset, expectedName, "BAZAAR_SHOPPING character name truncation") && ok;
    return ok;
}

} // namespace

auto runS2CBazaarPacketSelfTests() -> bool
{
    bool ok = true;
    ok      = testLayoutAndEnums() && ok;
    ok      = testPacketDataBytes() && ok;
    ok      = testBazaarCloseCharacterConstructor() && ok;
    ok      = testBazaarShoppingCharacterConstructor() && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("s2c-bazaar-packets", runS2CBazaarPacketSelfTests);
