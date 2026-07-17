#include "test_attackround_create_daken_plan_2758.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround create daken plan 2758 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAttackRoundCreateDakenPlan2758SelfTests() -> bool
{
    using namespace attackroundhelpers;

    const auto notPC     = ResolveCreateDakenAttackPlan(false, true, true);
    const auto noAmmo    = ResolveCreateDakenAttackPlan(true, false, true);
    const auto missRate  = ResolveCreateDakenAttackPlan(true, true, false);
    const auto throwIt   = ResolveCreateDakenAttackPlan(true, true, true);

    return expect(!notPC.addDakenThrow, "non-pc empty") &&
           expect(!noAmmo.addDakenThrow, "non-shuriken empty") &&
           expect(!missRate.addDakenThrow, "miss empty") &&
           expect(throwIt.addDakenThrow, "shuriken throw");
}
