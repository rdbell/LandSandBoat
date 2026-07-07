#pragma once

#include "data_loader.h"

#include <vector>

auto BuildAuctionCategoryItem(uint16 itemID, uint32 singleAmount, uint32 stackAmount, uint32 stackSize, uint16 category) -> ahItem;
auto BuildAuctionItemFromIDRow(uint16 itemID, uint16 category, uint32 singleAmount, uint32 stackAmount) -> ahItem;
void OrderAuctionHistoryForPacket(std::vector<ahHistory*>& history);
