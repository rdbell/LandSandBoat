#include "test_trade_item_1517.h"

#include "map/trade_item_capacity.h"

#include <iostream>

namespace
{
using tradeitemhelpers::ShouldCloneSingleStackTrade;
using tradeitemhelpers::ShouldRejectAddItemEmptyOrZero;
using tradeitemhelpers::ShouldRejectAddItemMissingDB;
using tradeitemhelpers::ShouldRejectMonstrosityTrade;
using tradeitemhelpers::ShouldRejectRareAddItem;
using tradeitemhelpers::ShouldRejectRareDuplicate;
using tradeitemhelpers::ShouldRejectTradeForSpace;
using tradeitemhelpers::ShouldUpdateCurrencyInstead;
using tradeitemhelpers::TradeRemoveQuantity;
using tradeitemhelpers::IsTradeSlotInRange;

auto Check() -> bool
{
    if (!ShouldRejectMonstrosityTrade(true, false) || !ShouldRejectMonstrosityTrade(false, true) || ShouldRejectMonstrosityTrade(false, false))
    {
        return false;
    }
    if (!ShouldRejectTradeForSpace(2, 3) || ShouldRejectTradeForSpace(3, 3) || ShouldRejectTradeForSpace(5, 2))
    {
        return false;
    }
    if (!ShouldRejectRareDuplicate(true, true) || ShouldRejectRareDuplicate(true, false) || ShouldRejectRareDuplicate(false, true))
    {
        return false;
    }
    if (!IsTradeSlotInRange(0) || !IsTradeSlotInRange(8) || IsTradeSlotInRange(9))
    {
        return false;
    }
    if (!ShouldCloneSingleStackTrade(1, 1) || ShouldCloneSingleStackTrade(1, 2) || ShouldCloneSingleStackTrade(2, 1))
    {
        return false;
    }
    if (TradeRemoveQuantity(5) != -5 || TradeRemoveQuantity(0) != 0)
    {
        return false;
    }
    if (!ShouldRejectAddItemEmptyOrZero(0, 1) || !ShouldRejectAddItemEmptyOrZero(1, 0) || ShouldRejectAddItemEmptyOrZero(1, 1))
    {
        return false;
    }
    if (!ShouldRejectAddItemMissingDB(false) || ShouldRejectAddItemMissingDB(true))
    {
        return false;
    }
    if (!ShouldUpdateCurrencyInstead(true) || ShouldUpdateCurrencyInstead(false))
    {
        return false;
    }
    if (!ShouldRejectRareAddItem(true, true) || ShouldRejectRareAddItem(true, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runTradeItem1517SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "trade item 1517 self-test failed\n";
    }
    return ok;
}
