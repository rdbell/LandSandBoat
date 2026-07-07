#pragma once

#include "data_loader.h"

#include <string>
#include <vector>

struct ListingToExpire
{
    uint32      saleID     = 0;
    uint32      itemID     = 0;
    uint8       itemStack  = 0;
    uint8       ahStack    = 0;
    uint32      sellerID   = 0;
    std::string sellerName = "?";
};

auto BuildAuctionCategoryItem(uint16 itemID, uint32 singleAmount, uint32 stackAmount, uint32 stackSize, uint16 category) -> ahItem;
auto BuildAuctionCategoryListQuery(bool omitNoHistory, const std::string& orderByString) -> std::string;
auto BuildAuctionItemFromIDRow(uint16 itemID, uint16 category, uint32 singleAmount, uint32 stackAmount) -> ahItem;
void OrderAuctionHistoryForPacket(std::vector<ahHistory*>& history);
auto BuildListingToExpire(uint32 saleID, uint32 itemID, uint8 itemStack, uint8 ahStack, uint32 sellerID) -> ListingToExpire;
auto AuctionExpiredDeliveryQuantity(const ListingToExpire& listing) -> uint8;
