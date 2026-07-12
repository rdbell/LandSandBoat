#include "test_char_is_mob_owner_1483.h"

#include "map/char_is_mob_owner_capacity.h"

#include <iostream>

namespace
{
auto CheckEvaluate() -> bool
{
    // Unclaimed, self-owned, and PC targets always succeed without alliance scan.
    if (!charismobownerhelpers::Evaluate(0, 10, false, false, []() { return false; }))
    {
        return false;
    }
    if (!charismobownerhelpers::Evaluate(10, 10, false, false, []() { return false; }))
    {
        return false;
    }
    if (!charismobownerhelpers::Evaluate(99, 10, true, false, []() { return false; }))
    {
        return false;
    }

    // Non-exclusive claim short-circuits before alliance scan.
    bool allianceCalled = false;
    if (!charismobownerhelpers::Evaluate(99, 10, false, true, [&]() {
            allianceCalled = true;
            return false;
        }) ||
        allianceCalled)
    {
        return false;
    }

    // Claimed by other: alliance membership decides.
    if (charismobownerhelpers::Evaluate(99, 10, false, false, []() { return false; }))
    {
        return false;
    }
    if (!charismobownerhelpers::Evaluate(99, 10, false, false, []() { return true; }))
    {
        return false;
    }

    // Precedence: self-owned wins over non-exclusive/alliance callbacks.
    allianceCalled = false;
    if (!charismobownerhelpers::Evaluate(10, 10, false, true, [&]() {
            allianceCalled = true;
            return false;
        }) ||
        allianceCalled)
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharIsMobOwner1483SelfTests() -> bool
{
    const bool ok = CheckEvaluate();
    if (!ok)
    {
        std::cerr << "char is-mob-owner 1483 self-test failed\n";
    }
    return ok;
}
