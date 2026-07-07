#include "search_auction_rows.h"

#include <algorithm>

auto BuildAuctionCategoryItem(const uint16 itemID, const uint32 singleAmount, const uint32 stackAmount, const uint32 stackSize, const uint16 category) -> ahItem
{
    auto item          = ahItem{};
    item.ItemID       = itemID;
    item.SingleAmount = singleAmount;
    item.StackAmount  = stackAmount;
    item.Category     = category;

    if (stackSize == 1)
    {
        item.StackAmount = static_cast<uint32>(-1);
    }

    return item;
}

auto BuildAuctionCategoryListQuery(const bool omitNoHistory, const std::string& orderByString) -> std::string
{
    const auto fromTable = omitNoHistory ? "(SELECT item_basic.* "
                                           "FROM item_basic "
                                           "INNER JOIN auction_house_items ON item_basic.itemid = auction_house_items.itemid"
                                           ") AS item_basic "
                                         : "item_basic";

    return "SELECT item_basic.itemid, item_basic.stackSize, COUNT(*)-SUM(stack), SUM(stack) "
           "FROM " +
           std::string(fromTable) +
           " "
           "LEFT JOIN auction_house ON item_basic.itemId = auction_house.itemid AND auction_house.buyer_name IS NULL "
           "LEFT JOIN item_equipment ON item_basic.itemid = item_equipment.itemid "
           "LEFT JOIN item_weapon ON item_basic.itemid = item_weapon.itemid "
           "WHERE aH = ? "
           "GROUP BY item_basic.itemid "
           + orderByString;
}

auto BuildAuctionItemFromIDQuery(const uint16 itemID) -> AuctionItemFromIDQuery
{
    return AuctionItemFromIDQuery{
        "SELECT aH, COUNT(*)-SUM(stack), SUM(stack) "
        "FROM item_basic "
        "LEFT JOIN auction_house ON item_basic.itemId = auction_house.itemid AND auction_house.buyer_name IS NULL "
        "LEFT JOIN item_equipment ON item_basic.itemid = item_equipment.itemid "
        "LEFT JOIN item_weapon ON item_basic.itemid = item_weapon.itemid "
        "WHERE item_basic.itemid = ?",
        itemID,
    };
}

auto BuildAuctionHistoryQuery(const uint16 itemID, const bool stack) -> AuctionHistoryQuery
{
    return AuctionHistoryQuery{
        "SELECT sale, sell_date, seller_name, buyer_name "
        "FROM auction_house "
        "WHERE itemid = ? AND stack = ? AND buyer_name IS NOT NULL "
        "ORDER BY sell_date DESC "
        "LIMIT 10",
        itemID,
        stack,
    };
}

auto BuildExpiredAuctionListingsQuery(const uint16 expireAgeInDays, const uint32 currentTimestamp) -> ExpiredAuctionListingsQuery
{
    return ExpiredAuctionListingsQuery{
        "SELECT T0.id,T0.itemid,T1.stacksize, T0.stack, T0.seller FROM auction_house T0 INNER JOIN item_basic T1 ON "
        "T0.itemid = T1.itemid WHERE T0.buyer_name IS NULL AND T0.date <= ?",
        currentTimestamp - static_cast<uint32>(expireAgeInDays) * 86400u,
    };
}

auto BuildAuctionItemFromIDRow(const uint16 itemID, const uint16 category, const uint32 singleAmount, const uint32 stackAmount) -> ahItem
{
    auto item          = ahItem{};
    item.ItemID       = itemID;
    item.Category     = category;
    item.SingleAmount = singleAmount;
    item.StackAmount  = stackAmount;
    return item;
}

void OrderAuctionHistoryForPacket(std::vector<ahHistory*>& history)
{
    std::reverse(history.begin(), history.end());
}

auto BuildListingToExpire(const uint32 saleID, const uint32 itemID, const uint8 itemStack, const uint8 ahStack, const uint32 sellerID) -> ListingToExpire
{
    return ListingToExpire{ saleID, itemID, itemStack, ahStack, sellerID, "?" };
}

auto AuctionExpiredDeliveryQuantity(const ListingToExpire& listing) -> uint8
{
    return listing.ahStack == 1 ? listing.itemStack : 1;
}
