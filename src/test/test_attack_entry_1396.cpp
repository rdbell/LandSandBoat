#include "test_attack_entry_1396.h"

#include "map/attack_entry_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack entry 1396 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackEntry1396SelfTests() -> bool
{
    using namespace attackentryhelpers;
    bool ok = true;

    std::vector<int> effects;
    ApplyInitialEffects([&effects]() { effects.push_back(1); }, [&effects]() { effects.push_back(2); });
    ok = expect(effects == std::vector<int>{ 1, 2 }, "claim then last-attacked") && ok;

    int intimidationChecks = 0;
    auto intimidated       = [&intimidationChecks]() {
        ++intimidationChecks;
        return true;
    };
    ok = expect(ResolveInterrupt(true, intimidated) == AttackEntryInterrupt::Paralyzed, "paralyzed") && ok;
    ok = expect(intimidationChecks == 0, "paralysis short-circuit") && ok;
    ok = expect(ResolveInterrupt(false, intimidated) == AttackEntryInterrupt::Intimidated, "intimidated") && ok;
    ok = expect(intimidationChecks == 1, "intimidation evaluated once") && ok;

    auto notIntimidated = []() { return false; };
    ok = expect(ResolveInterrupt(false, notIntimidated) == AttackEntryInterrupt::None, "proceed") && ok;
    return ok;
}
