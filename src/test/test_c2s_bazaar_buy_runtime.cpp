/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_bazaar_buy_runtime.h"

#include <iostream>
#include <limits>

#include "map/packets/c2s/0x106_bazaar_buy.h"

namespace
{
using bazaarbuyhelpers::Action;
using bazaarbuyhelpers::Error;
using bazaarbuyhelpers::Facts;
using bazaarbuyhelpers::SelectDecision;

auto expect(const bazaarbuyhelpers::Decision& decision, const Action action, const Error error, const uint32 sellerCredit, const uint32 buyerDebit, const char* label) -> bool
{
    if (decision.action != action || decision.error != error || decision.sellerCredit != sellerCredit || decision.buyerDebit != buyerDebit)
    {
        std::cerr << "c2s BAZAAR_BUY runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

constexpr auto purchasableFacts() -> Facts
{
    return { true, true, true, true, true, false, false, false, true, 100, 4, 1200, 500 };
}
} // namespace

auto runC2SBazaarBuyRuntimeSelfTests() -> bool
{
    bool ok = true;
    auto facts = purchasableFacts();
    ok = expect(SelectDecision({ false }, 1), Action::None, Error::None, 0, 0, "missing target") && ok;
    facts.targetMatchesBazaarID = false;
    ok = expect(SelectDecision(facts, 1), Action::None, Error::None, 0, 0, "mismatched target") && ok;
    facts = purchasableFacts(); facts.hasBazaarInventory = false;
    ok = expect(SelectDecision(facts, 1), Action::None, Error::None, 0, 0, "missing bazaar inventory") && ok;
    facts = purchasableFacts(); facts.hasBuyerInventory = false;
    ok = expect(SelectDecision(facts, 1), Action::None, Error::None, 0, 0, "missing buyer inventory") && ok;
    facts = purchasableFacts(); facts.hasBazaarItem = false;
    ok = expect(SelectDecision(facts, 1), Action::None, Error::None, 0, 0, "missing item") && ok;
    facts = purchasableFacts(); facts.bazaarItemReserved = true;
    ok = expect(SelectDecision(facts, 1), Action::None, Error::None, 0, 0, "reserved item") && ok;
    facts = purchasableFacts(); facts.buyerIsSeller = true;
    ok = expect(SelectDecision(facts, 1), Action::SendError, Error::SelfOrInventoryFull, 0, 0, "self purchase") && ok;
    facts = purchasableFacts(); facts.buyerInventoryFull = true;
    ok = expect(SelectDecision(facts, 1), Action::SendError, Error::SelfOrInventoryFull, 0, 0, "full inventory") && ok;
    facts = purchasableFacts(); facts.hasUsableGil = false;
    ok = expect(SelectDecision(facts, 1), Action::SendError, Error::NoUsableGil, 0, 0, "unusable gil") && ok;
    facts = purchasableFacts(); facts.itemPrice = 0;
    ok = expect(SelectDecision(facts, 1), Action::SendError, Error::UnavailableItem, 0, 0, "zero price") && ok;
    facts = purchasableFacts(); facts.itemQuantity = 1;
    ok = expect(SelectDecision(facts, 2), Action::SendError, Error::UnavailableItem, 0, 0, "insufficient quantity") && ok;
    ok = expect(SelectDecision(purchasableFacts(), 2), Action::Purchase, Error::None, 200, 210, "taxed purchase") && ok;
    facts = purchasableFacts(); facts.itemPrice = std::numeric_limits<uint32>::max(); facts.itemQuantity = 1; facts.tax = 0; facts.buyerGil = std::numeric_limits<uint32>::max();
    ok = expect(SelectDecision(facts, 1), Action::Purchase, Error::None, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max(), "maximum uint32 price") && ok;
    facts = purchasableFacts(); facts.itemPrice = std::numeric_limits<uint32>::max(); facts.itemQuantity = 1;
    ok = expect(SelectDecision(facts, 1), Action::SendError, Error::PriceOverflow, 0, 0, "tax overflow") && ok;
    facts = purchasableFacts(); facts.buyerGil = 209;
    ok = expect(SelectDecision(facts, 2), Action::SendError, Error::InsufficientGil, 0, 0, "insufficient gil") && ok;
    return ok;
}
