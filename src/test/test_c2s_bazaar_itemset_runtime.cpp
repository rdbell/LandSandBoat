/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_bazaar_itemset_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10a_bazaar_itemset.h"

namespace
{

using bazaaritemsethelpers::Action;
using bazaaritemsethelpers::SelectDecision;
using bazaaritemsethelpers::SubType;

auto expectNoAction(const bazaaritemsethelpers::Decision& decision, const char* label) -> bool
{
    if (decision.action != Action::None || decision.subType != SubType::Unchanged || decision.persistPrice || decision.sendItemAttr || decision.sendItemSame)
    {
        std::cerr << "c2s BAZAAR_ITEMSET runtime self-test failed: " << label << " should not update\n";
        return false;
    }
    return true;
}

auto expectApply(const bazaaritemsethelpers::Decision& decision, const uint32 price, const SubType subtype, const char* label) -> bool
{
    if (decision.action != Action::ApplyPrice || decision.price != price || decision.subType != subtype ||
        !decision.persistPrice || !decision.sendItemAttr || !decision.sendItemSame)
    {
        std::cerr << "c2s BAZAAR_ITEMSET runtime self-test failed: " << label << " effects differ\n";
        return false;
    }
    return true;
}

} // namespace

auto runC2SBazaarItemSetRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok = expectNoAction(SelectDecision(false, true, false, false, false, false, 0, 100), "missing storage") && ok;
    ok = expectNoAction(SelectDecision(true, false, false, false, false, false, 0, 100), "missing item") && ok;
    ok = expectNoAction(SelectDecision(true, true, true, false, false, false, 0, 100), "reserved item") && ok;
    ok = expectNoAction(SelectDecision(true, true, false, true, false, false, 0, 100), "busy item") && ok;
    ok = expectNoAction(SelectDecision(true, true, false, false, true, false, 0, 100), "exclusive item") && ok;
    ok = expectNoAction(SelectDecision(true, true, false, false, false, true, 0, 100), "unpriced locked item") && ok;
    ok = expectApply(SelectDecision(true, true, false, false, false, true, 1, 100), 100, SubType::Locked, "priced locked item") && ok;
    ok = expectApply(SelectDecision(true, true, false, false, false, false, 0, 100), 100, SubType::Locked, "set price") && ok;
    ok = expectApply(SelectDecision(true, true, false, false, false, false, 100, 0), 0, SubType::Unlocked, "clear price") && ok;
    return ok;
}
