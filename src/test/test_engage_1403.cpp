#include "test_engage_1403.h"

#include "map/engage_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "engage 1403 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runEngage1403SelfTests() -> bool
{
    using namespace engagehelpers;
    bool ok = true;

    ok = expect(ResolveState(0) == EngageState{ 1, 0x04 }, "base state") && ok;
    ok = expect(ResolveState(0x82) == EngageState{ 1, 0x86 }, "preserve mask") && ok;
    ok = expect(ResolveState(0xFF) == EngageState{ 1, 0xFF }, "idempotent HP bit") && ok;

    std::vector<int> calls;
    EngageState applied{};
    Apply(
        0x02,
        [&](const EngageState state) { calls.push_back(1); applied = state; },
        [&]() { calls.push_back(2); });
    ok = expect(applied == EngageState{ 1, 0x06 } && calls == std::vector<int>{ 1, 2 }, "state before listener") && ok;
    ok = expect(
             battletransitionhelpers::AnimationNone == 0 &&
                 battletransitionhelpers::AnimationAttack == 1 &&
                 battletransitionhelpers::UpdateHP == 0x04,
             "shared pins") && ok;
    return ok;
}
