/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_auc_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x04c_auc.h"

namespace
{

auto expectPlan(const auchelpters::AskCommitPlan actual, const auchelpters::AskCommitPlan expected, const char* label) -> bool
{
    if (actual.command != expected.command || actual.aucWorkIndex != expected.aucWorkIndex || actual.result != expected.result || actual.resultStatus != expected.resultStatus || actual.commission != expected.commission || actual.itemId != expected.itemId || actual.itemWorkIndex != expected.itemWorkIndex || actual.itemStacks != expected.itemStacks || actual.marketNo != expected.marketNo)
    {
        std::cerr << "s2c AUC runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CAUCRuntimeSelfTests() -> bool
{
    using namespace auchelpters;

    const auto common = AskCommitFacts{ 4, 0x1234, 0x0056, 0, 125, 10, 2.5F, 20, 4.0F, 100 };
    bool ok = true;
    ok = expectPlan(AskCommitPlanFor(common), { 4, -1, 1, 2, 13, 0x1234, 0x0056, 0, 4 }, "stack branch truncates fractional fee and fills fixed fields") && ok;

    auto single       = common;
    single.itemStacks = 1;
    ok = expectPlan(AskCommitPlanFor(single), { 4, -1, 1, 2, 25, 0x1234, 0x0056, 1, 4 }, "single branch uses single settings") && ok;

    auto clamped   = common;
    clamped.price  = 5000;
    clamped.maxFee = 50;
    ok = expectPlan(AskCommitPlanFor(clamped), { 4, -1, 1, 2, 50, 0x1234, 0x0056, 0, 4 }, "fee clamps at configured maximum") && ok;
    return ok;
}
