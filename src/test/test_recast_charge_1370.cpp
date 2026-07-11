#include "test_recast_charge_1370.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast charge 1370 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "recast charge 1370 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runRecastCharge1370SelfTests() -> bool
{
    using namespace recasthelpers;
    bool ok = true;

    ok = expect(ShouldUpdateChargeTime(true) && !ShouldUpdateChargeTime(false), "upd charge") && ok;
    ok = expect(ShouldUpdateMaxCharges(true) && !ShouldUpdateMaxCharges(false), "upd max") && ok;
    ok = expect(IsSimpleRecast(true) && !IsSimpleRecast(false), "simple") && ok;
    ok = expect(ShouldStampOnZeroRecast(true) && !ShouldStampOnZeroRecast(false), "stamp") && ok;
    ok = expectEq(ChargeCapUnits(10, 3), static_cast<int64>(30), "cap") && ok;
    ok = expect(ExceedsChargeCap(20, 15, 30) && !ExceedsChargeCap(10, 5, 30), "exceeds") && ok;
    ok = expectEq(ChargeCapOverflowDiff(20, 15, 30), static_cast<int64>(5), "diff") && ok;
    ok = expect(IsMatchingActiveRecast(true, true) && !IsMatchingActiveRecast(true, false), "active") && ok;
    ok = expect(HasRecastWhenSimple(true) && !HasRecastWhenSimple(false), "has simple") && ok;
    ok = expect(RequestExceedsMaxCharges(5, 3) && !RequestExceedsMaxCharges(2, 3), "req max") && ok;
    ok = expectEq(AvailableCharges(25, 10), static_cast<uint8>(2), "available") && ok;
    ok = expectEq(AvailableCharges(5, 0), static_cast<uint8>(0), "avail zero charge") && ok;
    ok = expectEq(RemainingChargesAfterRequest(3, 1), static_cast<int64>(1), "remaining") && ok;
    // max 3, available 1 → 3-1-1 = 1; request 2 → insufficient
    ok = expect(HasInsufficientCharges(1, 2) && !HasInsufficientCharges(2, 2), "insuff") && ok;
    ok = expectEq(CurrentRecastRemaining(100, 40, 20), static_cast<int64>(80), "current") && ok;
    ok = expect(IsAbilityRecastType(true) && !IsAbilityRecastType(false), "ability type") && ok;
    ok = expect(ShouldExpireRecast(50, 10, 40) && !ShouldExpireRecast(49, 10, 40), "expire") && ok;
    ok = expect(ShouldEraseOnExpire(false) && !ShouldEraseOnExpire(true), "erase") && ok;
    ok = expect(IsNewRecastEntry(true) && !IsNewRecastEntry(false), "new entry") && ok;

    return ok;
}
