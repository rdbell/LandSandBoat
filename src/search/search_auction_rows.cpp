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
