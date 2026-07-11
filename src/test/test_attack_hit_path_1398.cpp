#include "test_attack_hit_path_1398.h"

#include "map/attack_hit_path_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack hit path 1398 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackHitPath1398SelfTests() -> bool
{
    using namespace attackhitpathhelpers;
    bool ok = true;

    auto run = [](bool parried, bool sata, bool shadow, bool anticipated, bool countered, std::vector<int>& calls) {
        return ResolveHitPath(
            [&]() { calls.push_back(1); return parried; },
            [&]() { calls.push_back(2); return sata; },
            [&]() { calls.push_back(3); return shadow; },
            [&]() { calls.push_back(4); return anticipated; },
            [&]() { calls.push_back(5); return countered; });
    };

    std::vector<int> calls;
    ok = expect(run(true, false, true, true, true, calls) == HitPathOutcome::Parried && calls == std::vector<int>{ 1 }, "parry first") && ok;
    calls.clear();
    ok = expect(run(false, false, true, true, true, calls) == HitPathOutcome::ShadowAbsorbed && calls == std::vector<int>{ 1, 2, 3 }, "shadow second") && ok;
    calls.clear();
    ok = expect(run(false, true, true, true, true, calls) == HitPathOutcome::Reactive && calls == std::vector<int>{ 1, 2, 4 }, "SATA skips shadow") && ok;
    calls.clear();
    ok = expect(run(false, false, false, true, true, calls) == HitPathOutcome::Reactive && calls == std::vector<int>{ 1, 2, 3, 4 }, "anticipate skips counter") && ok;
    calls.clear();
    ok = expect(run(false, false, false, false, true, calls) == HitPathOutcome::Reactive && calls == std::vector<int>{ 1, 2, 3, 4, 5 }, "counter fallback") && ok;
    calls.clear();
    ok = expect(run(false, false, false, false, false, calls) == HitPathOutcome::Landed && calls == std::vector<int>{ 1, 2, 3, 4, 5 }, "landed") && ok;
    return ok;
}
