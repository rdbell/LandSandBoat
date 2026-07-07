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
