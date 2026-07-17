#include "test_attackround_create_kick_plan_2757.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround create kick plan 2757 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runAttackRoundCreateKickPlan2757SelfTests() -> bool
{
    using namespace attackroundhelpers;

    const auto notH2H   = ResolveCreateKickAttacksPlan(false, true, true);
    const auto miss     = ResolveCreateKickAttacksPlan(true, false, true);
    const auto rightOnly = ResolveCreateKickAttacksPlan(true, true, false);
    const auto both     = ResolveCreateKickAttacksPlan(true, true, true);

    return expect(!notH2H.addRightKick && !notH2H.addLeftKick && !notH2H.markKickOccurred, "non-h2h empty") &&
           expect(!miss.addRightKick && !miss.addLeftKick && !miss.markKickOccurred, "miss empty") &&
           expect(rightOnly.addRightKick && !rightOnly.addLeftKick && rightOnly.markKickOccurred, "right only") &&
           expect(both.addRightKick && both.addLeftKick && both.markKickOccurred, "both kicks");
}
