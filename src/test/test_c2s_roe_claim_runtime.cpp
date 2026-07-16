/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_roe_claim_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10e_roe_claim.h"

auto runC2SROEClaimRuntimeSelfTests() -> bool
{
    bool ok = true;
    const auto noRecord = roeclaimhelpers::SelectResult(false);
    if (noRecord.action != roeclaimhelpers::ClaimAction::NoRecord || !noRecord.sendCurrencies1)
    {
        std::cerr << "c2s ROE_CLAIM runtime self-test failed: no record still refreshes currencies\n";
        ok = false;
    }

    const auto claimed = roeclaimhelpers::SelectResult(true);
    if (claimed.action != roeclaimhelpers::ClaimAction::TriggerClaim || !claimed.sendCurrencies1)
    {
        std::cerr << "c2s ROE_CLAIM runtime self-test failed: claimed record triggers then refreshes currencies\n";
        ok = false;
    }
    return ok;
}
