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

struct AuctionItemFromIDQuery
{
    std::string sql    = {};
    uint16      itemID = 0;
};

struct AuctionHistoryQuery
{
    std::string sql    = {};
    uint16      itemID = 0;
    bool        stack  = false;
};

struct ExpiredAuctionListingsQuery
{
    std::string sql    = {};
    uint32      cutoff = 0;
};

struct ExpiredAuctionSellerNameQuery
{
    std::string sql      = {};
    uint32      sellerID = 0;
};

struct ExpiredAuctionDeliveryBoxQuery
{
    std::string sql        = {};
    uint32      sellerID   = 0;
    std::string sellerName = {};
    uint32      itemID     = 0;
    uint8       quantity   = 0;
};

struct ExpiredAuctionDeleteListingQuery
{
    std::string sql    = {};
    uint32      saleID = 0;
};

auto BuildAuctionCategoryItem(uint16 itemID, uint32 singleAmount, uint32 stackAmount, uint32 stackSize, uint16 category) -> ahItem;
auto BuildAuctionCategoryListQuery(bool omitNoHistory, const std::string& orderByString) -> std::string;
auto BuildAuctionItemFromIDQuery(uint16 itemID) -> AuctionItemFromIDQuery;
auto BuildAuctionHistoryQuery(uint16 itemID, bool stack) -> AuctionHistoryQuery;
auto BuildExpiredAuctionListingsQuery(uint16 expireAgeInDays, uint32 currentTimestamp) -> ExpiredAuctionListingsQuery;
auto BuildExpiredAuctionSellerNameQuery(uint32 sellerID) -> ExpiredAuctionSellerNameQuery;
auto BuildExpiredAuctionDeliveryBoxQuery(const ListingToExpire& listing) -> ExpiredAuctionDeliveryBoxQuery;
auto BuildExpiredAuctionDeleteListingQuery(uint32 saleID) -> ExpiredAuctionDeleteListingQuery;
auto BuildAuctionItemFromIDRow(uint16 itemID, uint16 category, uint32 singleAmount, uint32 stackAmount) -> ahItem;
void OrderAuctionHistoryForPacket(std::vector<ahHistory*>& history);
auto BuildListingToExpire(uint32 saleID, uint32 itemID, uint8 itemStack, uint8 ahStack, uint32 sellerID) -> ListingToExpire;
auto AuctionExpiredDeliveryQuantity(const ListingToExpire& listing) -> uint8;
