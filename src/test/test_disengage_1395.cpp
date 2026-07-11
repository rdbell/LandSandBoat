#include "test_disengage_1395.h"

#include "map/disengage_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "disengage 1395 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDisengage1395SelfTests() -> bool
{
    using namespace disengagehelpers;
    bool ok = true;

    ok = expect(ResolveDisengageState(AnimationAttack, 0) == DisengageState{ 0, AnimationNone, UpdateHP }, "attack reset") && ok;
    ok = expect(ResolveDisengageState(AnimationNone, 0x02) == DisengageState{ 0, AnimationNone, 0x06 }, "none preserve mask") && ok;
    ok = expect(ResolveDisengageState(2, 0x80) == DisengageState{ 0, 2, 0x84 }, "other animation preserve") && ok;
    ok = expect(ResolveDisengageState(AnimationAttack, 0xFF) == DisengageState{ 0, AnimationNone, 0xFF }, "idempotent mask") && ok;
    ok = expect(AnimationNone == 0 && AnimationAttack == 1 && UpdateHP == 0x04, "pins") && ok;
    return ok;
}
