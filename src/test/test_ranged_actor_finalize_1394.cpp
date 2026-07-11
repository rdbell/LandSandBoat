#include "test_ranged_actor_finalize_1394.h"

#include "map/ranged_actor_finalize_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged actor finalize 1394 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runRangedActorFinalize1394SelfTests() -> bool
{
    using namespace rangedactorfinalizehelpers;
    bool ok = true;

    ok = expect(ResolveRangedActorFinalization(true, false) == RangedActorFinalization{ true, true, false, false }, "character") && ok;
    ok = expect(ResolveRangedActorFinalization(false, true) == RangedActorFinalization{ true, false, true, true }, "trust") && ok;
    ok = expect(ResolveRangedActorFinalization(false, false) == RangedActorFinalization{ false, false, true, true }, "mob") && ok;
    ok = expect(ResolveRangedActorFinalization(true, true) == RangedActorFinalization{ true, true, false, false }, "character precedence") && ok;
    return ok;
}
