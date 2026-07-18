#include "test_attackutils_occ_ladder_3908.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils Occ ladder 3908 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackutilsOccLadder3908SelfTests() -> bool
{
    using attackutilshelpers::OccProcResult;
    using attackutilshelpers::ResolveAllowProcLadder;
    bool ok = true;
    ok = expect(ResolveAllowProcLadder(false, 4.0f, true, true, true, true, true) == OccProcResult::None,
                "allowProc gate wins") && ok;
    ok = expect(ResolveAllowProcLadder(true, 4.0f, true, true, true, true, true) == OccProcResult::ExtraDamage,
                "extra over three precedes every later arm") && ok;
    ok = expect(ResolveAllowProcLadder(true, 4.0f, false, true, true, true, true) == OccProcResult::RemTriple,
                "rem triple precedes lower extra and double") && ok;
    ok = expect(ResolveAllowProcLadder(true, 3.0f, false, false, true, true, true) == OccProcResult::ExtraDamage,
                "extra over two strict threshold") && ok;
    ok = expect(ResolveAllowProcLadder(true, 2.0f, false, false, true, true, true) == OccProcResult::RemDouble,
                "two is not over two") && ok;
    ok = expect(ResolveAllowProcLadder(true, 1.0f, false, false, false, false, true) == OccProcResult::ExtraDamage,
                "positive fallback extra") && ok;
    ok = expect(ResolveAllowProcLadder(true, 0.0f, false, false, false, false, true) == OccProcResult::None,
                "zero cannot use extra fallback") && ok;
    return ok;
}
