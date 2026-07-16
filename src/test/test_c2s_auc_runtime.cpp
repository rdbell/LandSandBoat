/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_auc_runtime.h"

#include <array>
#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x04e_auc.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s AUC runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

struct TestCase
{
    GP_CLI_COMMAND_AUC_COMMAND           command;
    std::array<auchelpers::Operation, 2> operations;
    uint8                                count;
};

auto testOperationPlans() -> bool
{
    constexpr auto cases = std::array{
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::Open, {}, 0 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::AskCommit, { auchelpers::Operation::SellingItems }, 1 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::Info, { auchelpers::Operation::OpenListOfSales, auchelpers::Operation::RetrieveListOfItemsSoldByPlayer }, 2 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::WorkCheck, { auchelpers::Operation::RetrieveListOfItemsSoldByPlayer }, 1 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::LotIn, { auchelpers::Operation::ProofOfPurchase }, 1 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::LotCancel, { auchelpers::Operation::CancelSale }, 1 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::LotCheck, { auchelpers::Operation::UpdateSaleListByPlayer }, 1 },
        TestCase{ GP_CLI_COMMAND_AUC_COMMAND::Bid, { auchelpers::Operation::PurchasingItems }, 1 },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        const auto plan = auchelpers::BuildOperationPlan(test.command);
        ok              = expect(plan.count == test.count, "operation count") && ok;
        ok              = expect(plan.operations == test.operations, "operation sequence") && ok;
    }
    return ok;
}

auto testUnknownCommandHasNoOperation() -> bool
{
    const auto plan = auchelpers::BuildOperationPlan(static_cast<GP_CLI_COMMAND_AUC_COMMAND>(0));
    return expect(plan.count == 0, "unknown command has no operations");
}

} // namespace

auto runC2SAUCRuntimeSelfTests() -> bool
{
    return testOperationPlans() && testUnknownCommandHasNoOperation();
}
