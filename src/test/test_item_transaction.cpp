/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_item_transaction.h"

#include "map/items/item.h"
#include "map/items/transaction.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item transaction self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item transaction self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectGreater(std::uint64_t actual, std::uint64_t floor, const char* label) -> bool
{
    if (actual <= floor)
    {
        std::cerr << "item transaction self-test failed: " << label << " got "
                  << actual << " expected > " << floor << '\n';
        return false;
    }
    return true;
}

struct ProbeTransaction final : Transaction
{
    ~ProbeTransaction() override
    {
        rollbackIfOpen();
    }

    auto holds(const CItem* item) const -> bool override
    {
        return item == held;
    }

    bool         commitResult{ true };
    int          commitCalls{ 0 };
    int          rollbackCalls{ 0 };
    int*         externalRollbackCalls{ nullptr };
    const CItem* held{ nullptr };

protected:
    auto doCommit() -> bool override
    {
        ++commitCalls;
        return commitResult;
    }

    void doRollback() override
    {
        ++rollbackCalls;
        if (externalRollbackCalls != nullptr)
        {
            ++(*externalRollbackCalls);
        }
    }
};

auto testIDsAndInitialState() -> bool
{
    ProbeTransaction first;
    ProbeTransaction second;

    bool ok = true;
    ok      = expectGreater(first.id(), 0, "first id") && ok;
    ok      = expectGreater(second.id(), first.id(), "second id") && ok;
    ok      = expectBool(first.isOpen(), true, "first initially open") && ok;
    ok      = expectBool(second.isOpen(), true, "second initially open") && ok;
    return ok;
}

auto testCommitStateTransitions() -> bool
{
    ProbeTransaction tx;

    bool ok = true;
    ok      = expectBool(tx.commit(), true, "commit succeeds") && ok;
    ok      = expectBool(tx.isOpen(), false, "committed tx closed") && ok;
    ok      = expectUInt(tx.commitCalls, 1, "commit calls after commit") && ok;
    ok      = expectBool(tx.commit(), false, "second commit rejected") && ok;
    ok      = expectUInt(tx.commitCalls, 1, "second commit skips hook") && ok;
    tx.rollback();
    ok = expectUInt(tx.rollbackCalls, 0, "rollback after commit skips hook") && ok;
    return ok;
}

auto testRejectedCommitRemainsOpen() -> bool
{
    ProbeTransaction tx;
    tx.commitResult = false;

    bool ok = true;
    ok      = expectBool(tx.commit(), false, "rejected commit returns false") && ok;
    ok      = expectBool(tx.isOpen(), true, "rejected commit stays open") && ok;
    ok      = expectUInt(tx.commitCalls, 1, "rejected commit calls hook") && ok;
    tx.rollback();
    ok = expectBool(tx.isOpen(), false, "rollback closes rejected commit") && ok;
    ok = expectUInt(tx.rollbackCalls, 1, "rollback after rejection calls hook") && ok;
    tx.rollback();
    ok = expectUInt(tx.rollbackCalls, 1, "second rollback skips hook") && ok;
    return ok;
}

auto testRollbackStateTransitions() -> bool
{
    ProbeTransaction tx;

    bool ok = true;
    tx.rollback();
    ok = expectBool(tx.isOpen(), false, "rollback closes tx") && ok;
    ok = expectUInt(tx.rollbackCalls, 1, "rollback calls hook") && ok;
    ok = expectBool(tx.commit(), false, "commit after rollback rejected") && ok;
    ok = expectUInt(tx.commitCalls, 0, "commit after rollback skips hook") && ok;
    return ok;
}

auto testRollbackIfOpenFromDestructor() -> bool
{
    int rollbackCalls = 0;
    {
        ProbeTransaction tx;
        tx.externalRollbackCalls = &rollbackCalls;
    }

    return expectUInt(rollbackCalls, 1, "destructor rollbackIfOpen calls hook");
}

auto testHoldsDispatch() -> bool
{
    CItem            held(0x4000);
    CItem            other(0x4001);
    ProbeTransaction tx;
    tx.held = &held;

    bool ok = true;
    ok      = expectBool(tx.holds(&held), true, "holds held item") && ok;
    ok      = expectBool(tx.holds(&other), false, "does not hold other item") && ok;
    ok      = expectBool(tx.holds(nullptr), false, "does not hold null") && ok;
    return ok;
}

} // namespace

auto runItemTransactionSelfTests() -> bool
{
    bool ok = true;
    ok      = testIDsAndInitialState() && ok;
    ok      = testCommitStateTransitions() && ok;
    ok      = testRejectedCommitRemainsOpen() && ok;
    ok      = testRollbackStateTransitions() && ok;
    ok      = testRollbackIfOpenFromDestructor() && ok;
    ok      = testHoldsDispatch() && ok;
    return ok;
}
