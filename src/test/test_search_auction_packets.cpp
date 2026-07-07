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

#include "test_search_auction_packets.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "search/auction_request_order.h"
#include "search/data_loader.h"
#include "search/packets/auction_history.h"
#include "search/packets/auction_list.h"
#include "search/search_auction_rows.h"

namespace
{

template <typename T>
auto read(const std::uint8_t* data, const std::size_t offset) -> T
{
    T value{};
    std::memcpy(&value, data + offset, sizeof(value));
    return value;
}

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search auction packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectBytes(const std::uint8_t* actual, const std::string& expected, const std::string& label) -> bool
{
    if (std::memcmp(actual, expected.data(), expected.size()) != 0)
    {
        std::cerr << "search auction packet self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search auction packet self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testAuctionListConstructorAndCountHeaders() -> bool
{
    auto packet = CAHItemsListPacket(20);
    packet.SetItemCount(23);
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 52, "auction list empty size") && ok;
    ok      = expectEqualInt(data[0x0B], 0x95, "auction list packet type") && ok;
    ok      = expectEqualInt(data[0x0A], 0x80, "auction list final-page flag") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x08), 0x18 + 0x0A * 3, "auction list final-page length") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x0E), 23, "auction list total count") && ok;
    return ok;
}

auto testAuctionListCountCapsNonFinalPageLength() -> bool
{
    auto packet = CAHItemsListPacket(0);
    packet.SetItemCount(25);
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(data[0x0A], 0, "auction list non-final flag remains clear") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x08), 0x18 + 0x0A * 20, "auction list non-final capped length") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x0E), 25, "auction list non-final total count") && ok;
    return ok;
}

auto testAuctionListCountBeforeOffsetUsesPromotedArithmetic() -> bool
{
    auto packet = CAHItemsListPacket(20);
    packet.SetItemCount(10);
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(data[0x0A], 0x80, "auction list count-before-offset final flag") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x08), static_cast<std::uint16_t>(0x18 + 0x0A * -10), "auction list count-before-offset length") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x0E), 10, "auction list count-before-offset total count") && ok;
    return ok;
}

auto testAuctionListAddItemWritesRowsAndSize() -> bool
{
    auto packet = CAHItemsListPacket(0);
    packet.AddItem(new ahItem{ 0x1234, 1000, 12000, 7 });
    packet.AddItem(new ahItem{ 0x4567, 2000, 24000, 8 });
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 72, "auction list two-item size") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x18), 0x1234, "auction list first item id") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x1A), 1000, "auction list first single amount") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x1E), 12000, "auction list first stack amount") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x22), 0x4567, "auction list second item id") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x24), 2000, "auction list second single amount") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x28), 24000, "auction list second stack amount") && ok;
    return ok;
}

auto testAuctionHistoryConstructorUsesStackSelection() -> bool
{
    const auto item  = ahItem{ 0x2345, 3000, 36000, 12 };
    auto packet      = CAHHistoryPacket(item, 1);
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 60, "auction history empty size") && ok;
    ok      = expectEqualInt(data[0x0A], 0x80, "auction history flag") && ok;
    ok      = expectEqualInt(data[0x0B], 0x85, "auction history packet type") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x10), 0x2345, "auction history obsolete item id") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x18), 0x2345, "auction history item id") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x1A), 36000, "auction history stack amount") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x1E), 12, "auction history category") && ok;
    return ok;
}

auto testAuctionHistoryConstructorUsesSingleSelection() -> bool
{
    const auto item  = ahItem{ 0x2345, 3000, 36000, 12 };
    auto packet      = CAHHistoryPacket(item, 0);
    const auto* data = packet.GetData();

    return expectEqualInt(read<std::uint32_t>(data, 0x1A), 3000, "auction history single amount");
}

auto testAuctionHistoryAddItemWritesRecordAndLength() -> bool
{
    const auto item = ahItem{ 0x2345, 3000, 36000, 12 };
    auto packet     = CAHHistoryPacket(item, 0);
    packet.AddItem(new ahHistory{ 7777, 0xAABBCCDD, "BuyerName123456", "SellerName12345" });
    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 100, "auction history one-record size") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x08), 0x20 + 40, "auction history one-record length") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x20), 7777, "auction history record price") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x24), 0xAABBCCDD, "auction history record date") && ok;
    ok      = expectBytes(data + 0x28, "BuyerName123456", "auction history buyer name") && ok;
    ok      = expectBytes(data + 0x38, "SellerName12345", "auction history seller name") && ok;
    ok      = expectEqualInt(data[0x37], 0, "auction history byte after buyer copy remains zero") && ok;
    ok      = expectEqualInt(data[0x47], 0, "auction history byte after seller copy remains zero") && ok;
    return ok;
}

auto testAuctionHistoryCapsAtTenRecords() -> bool
{
    const auto item = ahItem{ 0x2345, 3000, 36000, 12 };
    auto packet     = CAHHistoryPacket(item, 0);

    for (std::uint32_t i = 0; i < 11; ++i)
    {
        packet.AddItem(new ahHistory{ 1000 + i, 2000 + i, "BuyerName123456", "SellerName12345" });
    }

    const auto* data = packet.GetData();

    bool ok = true;
    ok      = expectEqualInt(packet.GetSize(), 460, "auction history capped size") && ok;
    ok      = expectEqualInt(read<std::uint16_t>(data, 0x08), 0x20 + 40 * 10, "auction history capped length") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x20 + 40 * 9), 1009, "auction history tenth record price") && ok;
    ok      = expectEqualInt(read<std::uint32_t>(data, 0x20 + 40 * 10), 0, "auction history eleventh record ignored") && ok;
    return ok;
}

auto testAuctionOrderByDefaultsToItemID() -> bool
{
    std::uint8_t packet[64]{};
    const auto   orderBy = BuildAuctionHouseOrderByString(packet, 0);

    return expectEqualString(orderBy, "ORDER BY item_basic.itemid", "auction order default");
}

auto testAuctionOrderByMapsSupportedParamsInPacketOrder() -> bool
{
    std::uint8_t packet[96]{};
    packet[0x18] = 2;
    packet[0x20] = 5;
    packet[0x28] = 6;
    packet[0x30] = 9;

    const auto orderBy = BuildAuctionHouseOrderByString(packet, 4);

    return expectEqualString(orderBy,
                             "ORDER BY item_equipment.level DESC, item_weapon.dmg DESC, item_weapon.delay DESC, item_basic.sortname, item_basic.itemid",
                             "auction order supported params");
}

auto testAuctionOrderByIgnoresUnsupportedAndUsesLowByte() -> bool
{
    std::uint8_t packet[96]{};
    packet[0x18] = 3;
    packet[0x19] = 2;
    packet[0x20] = 9;
    packet[0x21] = 5;
    packet[0x28] = 8;
    packet[0x30] = 5;

    const auto orderBy = BuildAuctionHouseOrderByString(packet, 4);

    return expectEqualString(orderBy,
                             "ORDER BY item_basic.sortname, item_weapon.dmg DESC, item_basic.itemid",
                             "auction order low-byte params");
}

auto testAuctionCategoryItemMarksSingleOnlyStackAmount() -> bool
{
    const auto item = BuildAuctionCategoryItem(0x1234, 5, 0, 1, 7);

    bool ok = true;
    ok      = expectEqualInt(item.ItemID, 0x1234, "auction category row item id") && ok;
    ok      = expectEqualInt(item.SingleAmount, 5, "auction category row single amount") && ok;
    ok      = expectEqualInt(item.StackAmount, 0xFFFFFFFF, "auction category row single-only stack marker") && ok;
    ok      = expectEqualInt(item.Category, 7, "auction category row category") && ok;
    return ok;
}

auto testAuctionCategoryItemPreservesStackCounts() -> bool
{
    const auto item = BuildAuctionCategoryItem(0x4567, 3, 9, 12, 8);

    bool ok = true;
    ok      = expectEqualInt(item.ItemID, 0x4567, "auction category stack row item id") && ok;
    ok      = expectEqualInt(item.SingleAmount, 3, "auction category stack row single amount") && ok;
    ok      = expectEqualInt(item.StackAmount, 9, "auction category stack row stack amount") && ok;
    ok      = expectEqualInt(item.Category, 8, "auction category stack row category") && ok;
    return ok;
}

auto testAuctionCategoryListQueryUsesItemBasicByDefault() -> bool
{
    const auto query = BuildAuctionCategoryListQuery(false, "ORDER BY item_basic.itemid");

    return expectEqualString(query,
                             "SELECT item_basic.itemid, item_basic.stackSize, COUNT(*)-SUM(stack), SUM(stack) "
                             "FROM item_basic "
                             "LEFT JOIN auction_house ON item_basic.itemId = auction_house.itemid AND auction_house.buyer_name IS NULL "
                             "LEFT JOIN item_equipment ON item_basic.itemid = item_equipment.itemid "
                             "LEFT JOIN item_weapon ON item_basic.itemid = item_weapon.itemid "
                             "WHERE aH = ? "
                             "GROUP BY item_basic.itemid "
                             "ORDER BY item_basic.itemid",
                             "auction category default query");
}

auto testAuctionCategoryListQueryCanOmitNoHistoryRows() -> bool
{
    const auto query = BuildAuctionCategoryListQuery(true, "ORDER BY item_basic.sortname, item_basic.itemid");

    return expectEqualString(query,
                             "SELECT item_basic.itemid, item_basic.stackSize, COUNT(*)-SUM(stack), SUM(stack) "
                             "FROM (SELECT item_basic.* "
                             "FROM item_basic "
                             "INNER JOIN auction_house_items ON item_basic.itemid = auction_house_items.itemid"
                             ") AS item_basic  "
                             "LEFT JOIN auction_house ON item_basic.itemId = auction_house.itemid AND auction_house.buyer_name IS NULL "
                             "LEFT JOIN item_equipment ON item_basic.itemid = item_equipment.itemid "
                             "LEFT JOIN item_weapon ON item_basic.itemid = item_weapon.itemid "
                             "WHERE aH = ? "
                             "GROUP BY item_basic.itemid "
                             "ORDER BY item_basic.sortname, item_basic.itemid",
                             "auction category omit-no-history query");
}

auto testAuctionItemFromIDQueryBuildsSQLAndParam() -> bool
{
    const auto query = BuildAuctionItemFromIDQuery(0x2345);

    bool ok = true;
    ok      = expectEqualString(query.sql,
                                "SELECT aH, COUNT(*)-SUM(stack), SUM(stack) "
                                "FROM item_basic "
                                "LEFT JOIN auction_house ON item_basic.itemId = auction_house.itemid AND auction_house.buyer_name IS NULL "
                                "LEFT JOIN item_equipment ON item_basic.itemid = item_equipment.itemid "
                                "LEFT JOIN item_weapon ON item_basic.itemid = item_weapon.itemid "
                                "WHERE item_basic.itemid = ?",
                                "auction item-from-id query") &&
         ok;
    ok = expectEqualInt(query.itemID, 0x2345, "auction item-from-id item id") && ok;
    return ok;
}

auto testAuctionHistoryQueryBuildsSQLAndParams() -> bool
{
    const auto stackQuery  = BuildAuctionHistoryQuery(0x4567, true);
    const auto singleQuery = BuildAuctionHistoryQuery(0x4567, false);

    bool ok = true;
    ok      = expectEqualString(stackQuery.sql,
                                "SELECT sale, sell_date, seller_name, buyer_name "
                                "FROM auction_house "
                                "WHERE itemid = ? AND stack = ? AND buyer_name IS NOT NULL "
                                "ORDER BY sell_date DESC "
                                "LIMIT 10",
                                "auction history query") &&
         ok;
    ok = expectEqualInt(stackQuery.itemID, 0x4567, "auction history item id") && ok;
    ok = expectEqualInt(stackQuery.stack, true, "auction history stack param") && ok;
    ok = expectEqualInt(singleQuery.itemID, 0x4567, "auction history single item id") && ok;
    ok = expectEqualInt(singleQuery.stack, false, "auction history single stack param") && ok;
    return ok;
}

auto testExpiredAuctionListingsQueryBuildsSQLAndCutoff() -> bool
{
    const auto query = BuildExpiredAuctionListingsQuery(7, 1'700'000'000);

    bool ok = true;
    ok      = expectEqualString(query.sql,
                                "SELECT T0.id,T0.itemid,T1.stacksize, T0.stack, T0.seller FROM auction_house T0 INNER JOIN item_basic T1 ON "
                                "T0.itemid = T1.itemid WHERE T0.buyer_name IS NULL AND T0.date <= ?",
                                "expired auction listings query") &&
         ok;
    ok = expectEqualInt(query.cutoff, 1'700'000'000 - 7 * 86400, "expired auction listings cutoff") && ok;
    return ok;
}

auto testExpiredAuctionListingsQueryUsesUnsignedCutoffArithmetic() -> bool
{
    const auto query = BuildExpiredAuctionListingsQuery(2, 100);

    return expectEqualInt(query.cutoff, static_cast<uint32>(100 - 2 * 86400), "expired auction listings wrapped cutoff");
}

auto testExpiredAuctionSellerNameQueryBuildsSQLAndParam() -> bool
{
    const auto query = BuildExpiredAuctionSellerNameQuery(0x12345678);

    bool ok = true;
    ok      = expectEqualString(query.sql,
                                "SELECT charname FROM chars WHERE charid = ?",
                                "expired auction seller-name query") &&
         ok;
    ok = expectEqualInt(query.sellerID, 0x12345678, "expired auction seller-name seller id") && ok;
    return ok;
}

auto testAuctionItemFromIDRowBuildsDefaultAndLoadedRows() -> bool
{
    const auto empty  = BuildAuctionItemFromIDRow(0x2222, 0, 0, 0);
    const auto loaded = BuildAuctionItemFromIDRow(0x2222, 12, 4, 6);

    bool ok = true;
    ok      = expectEqualInt(empty.ItemID, 0x2222, "auction item default item id") && ok;
    ok      = expectEqualInt(empty.Category, 0, "auction item default category") && ok;
    ok      = expectEqualInt(empty.SingleAmount, 0, "auction item default single amount") && ok;
    ok      = expectEqualInt(empty.StackAmount, 0, "auction item default stack amount") && ok;
    ok      = expectEqualInt(loaded.ItemID, 0x2222, "auction item loaded item id") && ok;
    ok      = expectEqualInt(loaded.Category, 12, "auction item loaded category") && ok;
    ok      = expectEqualInt(loaded.SingleAmount, 4, "auction item loaded single amount") && ok;
    ok      = expectEqualInt(loaded.StackAmount, 6, "auction item loaded stack amount") && ok;
    return ok;
}

auto testAuctionHistoryRowsReverseForPacketOrder() -> bool
{
    auto first  = ahHistory{ 100, 1, "SellerA", "BuyerA" };
    auto second = ahHistory{ 200, 2, "SellerB", "BuyerB" };
    auto third  = ahHistory{ 300, 3, "SellerC", "BuyerC" };
    auto rows   = std::vector<ahHistory*>{ &first, &second, &third };

    OrderAuctionHistoryForPacket(rows);

    bool ok = true;
    ok      = expectEqualInt(rows[0]->Price, 300, "auction history first reversed price") && ok;
    ok      = expectEqualInt(rows[1]->Price, 200, "auction history middle reversed price") && ok;
    ok      = expectEqualInt(rows[2]->Price, 100, "auction history last reversed price") && ok;
    return ok;
}

auto testAuctionHistoryRowsReverseAllowsEmptyAndSingleRows() -> bool
{
    auto empty = std::vector<ahHistory*>{};
    OrderAuctionHistoryForPacket(empty);

    auto only = ahHistory{ 400, 4, "SellerD", "BuyerD" };
    auto rows = std::vector<ahHistory*>{ &only };
    OrderAuctionHistoryForPacket(rows);

    bool ok = true;
    ok      = expectEqualInt(empty.size(), 0, "auction history empty reverse size") && ok;
    ok      = expectEqualInt(rows.size(), 1, "auction history single reverse size") && ok;
    ok      = expectEqualInt(rows[0]->Price, 400, "auction history single reverse price") && ok;
    return ok;
}

auto testAuctionListingToExpireUsesPlaceholderSellerName() -> bool
{
    const auto listing = BuildListingToExpire(11, 0x1234, 12, 1, 0x456789);

    bool ok = true;
    ok      = expectEqualInt(listing.saleID, 11, "auction expire listing sale id") && ok;
    ok      = expectEqualInt(listing.itemID, 0x1234, "auction expire listing item id") && ok;
    ok      = expectEqualInt(listing.itemStack, 12, "auction expire listing item stack") && ok;
    ok      = expectEqualInt(listing.ahStack, 1, "auction expire listing ah stack") && ok;
    ok      = expectEqualInt(listing.sellerID, 0x456789, "auction expire listing seller id") && ok;
    ok      = expectEqualString(listing.sellerName, "?", "auction expire listing placeholder seller name") && ok;
    return ok;
}

auto testAuctionExpiredDeliveryQuantityUsesStackFlag() -> bool
{
    const auto stackListing  = BuildListingToExpire(11, 0x1234, 12, 1, 0x456789);
    const auto singleListing = BuildListingToExpire(12, 0x1234, 12, 0, 0x456789);
    const auto otherListing  = BuildListingToExpire(13, 0x1234, 12, 2, 0x456789);

    bool ok = true;
    ok      = expectEqualInt(AuctionExpiredDeliveryQuantity(stackListing), 12, "auction expire stack quantity") && ok;
    ok      = expectEqualInt(AuctionExpiredDeliveryQuantity(singleListing), 1, "auction expire single quantity") && ok;
    ok      = expectEqualInt(AuctionExpiredDeliveryQuantity(otherListing), 1, "auction expire non-stack quantity") && ok;
    return ok;
}

} // namespace

auto runSearchAuctionPacketSelfTests() -> bool
{
    return testAuctionListConstructorAndCountHeaders() &&
           testAuctionListCountCapsNonFinalPageLength() &&
           testAuctionListCountBeforeOffsetUsesPromotedArithmetic() &&
           testAuctionListAddItemWritesRowsAndSize() &&
           testAuctionHistoryConstructorUsesStackSelection() &&
           testAuctionHistoryConstructorUsesSingleSelection() &&
           testAuctionHistoryAddItemWritesRecordAndLength() &&
           testAuctionHistoryCapsAtTenRecords() &&
           testAuctionOrderByDefaultsToItemID() &&
           testAuctionOrderByMapsSupportedParamsInPacketOrder() &&
           testAuctionOrderByIgnoresUnsupportedAndUsesLowByte() &&
           testAuctionCategoryItemMarksSingleOnlyStackAmount() &&
           testAuctionCategoryItemPreservesStackCounts() &&
           testAuctionCategoryListQueryUsesItemBasicByDefault() &&
           testAuctionCategoryListQueryCanOmitNoHistoryRows() &&
           testAuctionItemFromIDQueryBuildsSQLAndParam() &&
           testAuctionHistoryQueryBuildsSQLAndParams() &&
           testExpiredAuctionListingsQueryBuildsSQLAndCutoff() &&
           testExpiredAuctionListingsQueryUsesUnsignedCutoffArithmetic() &&
           testExpiredAuctionSellerNameQueryBuildsSQLAndParam() &&
           testAuctionItemFromIDRowBuildsDefaultAndLoadedRows() &&
           testAuctionHistoryRowsReverseForPacketOrder() &&
           testAuctionHistoryRowsReverseAllowsEmptyAndSingleRows() &&
           testAuctionListingToExpireUsesPlaceholderSellerName() &&
           testAuctionExpiredDeliveryQuantityUsesStackFlag();
}
