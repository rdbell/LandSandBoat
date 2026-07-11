#include "test_attack_loop_1402.h"

#include "map/attack_loop_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack loop 1402 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackLoop1402SelfTests() -> bool
{
    using namespace attackloophelpers;
    bool ok = true;

    auto continueWith = [](uint8 swings, bool targetAlive, bool attackerAlive, std::vector<int>& calls) {
        return ShouldContinue(
            [&]() { calls.push_back(1); return swings; },
            [&]() { calls.push_back(2); return targetAlive; },
            [&]() { calls.push_back(3); return attackerAlive; });
    };
    std::vector<int> calls;
    ok = expect(continueWith(1, true, true, calls) && calls == std::vector<int>{ 1, 2, 3 }, "continue") && ok;
    calls.clear();
    ok = expect(!continueWith(0, true, true, calls) && calls == std::vector<int>{ 1 }, "no swings lazy") && ok;
    calls.clear();
    ok = expect(!continueWith(1, false, true, calls) && calls == std::vector<int>{ 1, 2 }, "dead target lazy") && ok;
    calls.clear();
    ok = expect(!continueWith(1, true, false, calls) && calls == std::vector<int>{ 1, 2, 3 }, "dead attacker") && ok;
    ok = expect(ShouldStopAtResultCap(8) && !ShouldStopAtResultCap(7) && !ShouldStopAtResultCap(9), "exact cap") && ok;

    calls.clear();
    const bool stop = FinishSwing(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); return std::size_t{ 8 }; });
    ok = expect(stop && calls == std::vector<int>{ 1, 2 }, "delete before count") && ok;

    calls.clear();
    ApplyFinalization(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); });
    ok = expect(calls == std::vector<int>{ 1, 2, 3, 4 }, "finalization order") && ok;
    ok = expect(MaxActionResults == 8, "cap pin") && ok;
    return ok;
}
