#include "test_attack_swing_gate_1397.h"

#include "map/attack_swing_gate_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack swing gate 1397 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackSwingGate1397SelfTests() -> bool
{
    using namespace attackswinggatehelpers;
    bool ok = true;

    int hitChecks = 0;
    int allMissChecks = 0;
    auto hit = [&]() { ++hitChecks; return true; };
    auto miss = [&]() { ++hitChecks; return false; };
    auto allMiss = [&]() { ++allMissChecks; return true; };
    auto noAllMiss = [&]() { ++allMissChecks; return false; };

    ok = expect(ResolveSwingGate(true, true, hit, allMiss) == SwingGateOutcome::PerfectDodge, "perfect dodge precedence") && ok;
    ok = expect(hitChecks == 0 && allMissChecks == 0, "perfect dodge lazy") && ok;
    ok = expect(ResolveSwingGate(false, true, hit, allMiss) == SwingGateOutcome::Deflected, "deflect precedence") && ok;
    ok = expect(hitChecks == 0 && allMissChecks == 0, "deflect lazy") && ok;
    ok = expect(ResolveSwingGate(false, false, hit, noAllMiss) == SwingGateOutcome::HitPath, "hit path") && ok;
    ok = expect(hitChecks == 1 && allMissChecks == 1, "hit evaluations") && ok;
    ok = expect(ResolveSwingGate(false, false, hit, allMiss) == SwingGateOutcome::Miss, "all miss override") && ok;
    ok = expect(ResolveSwingGate(false, false, miss, allMiss) == SwingGateOutcome::Miss, "roll miss") && ok;
    ok = expect(allMissChecks == 2, "all miss skipped after roll miss") && ok;
    return ok;
}
