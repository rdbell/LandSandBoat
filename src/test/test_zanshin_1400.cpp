#include "test_zanshin_1400.h"

#include "map/zanshin_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "zanshin 1400 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runZanshin1400SelfTests() -> bool
{
    using namespace zanshinhelpers;
    bool ok = true;

    ok = expect(ShouldEvaluate(true, 1) && !ShouldEvaluate(false, 1) && !ShouldEvaluate(true, 2), "eligibility") && ok;
    ok = expect(ResolveChance(20, 5) == 25 && ResolveChance(90, 20) == 100, "chance clamp") && ok;
    ok = expect(ResolveChance(-1, 0) == 100, "signed conversion wrap") && ok;

    auto run = [](uint8 resolution, uint8 reaction, uint16 chance, bool bonus, bool hasso, std::vector<int> rolls, int& hassoChecks, int& rollChecks) {
        std::size_t index = 0;
        return ResolveProcs(
            resolution, reaction, chance, bonus,
            [&]() { ++hassoChecks; return hasso; },
            [&]() { ++rollChecks; return rolls.at(index++); });
    };

    int hassoChecks = 0;
    int rollChecks  = 0;
    ok = expect(run(ResolutionHit, 0, 50, false, false, {}, hassoChecks, rollChecks) == ProcResult{ false, false }, "plain hit") && ok;
    ok = expect(hassoChecks == 0 && rollChecks == 0, "plain hit lazy") && ok;

    hassoChecks = rollChecks = 0;
    ok = expect(run(1, 0, 50, false, false, { 49 }, hassoChecks, rollChecks) == ProcResult{ true, false }, "normal miss proc") && ok;
    ok = expect(rollChecks == 1, "normal roll") && ok;

    hassoChecks = rollChecks = 0;
    const auto bothEligible = run(1, 0, 50, true, true, { 0, 12 }, hassoChecks, rollChecks);
    ok = expect(bothEligible == ProcResult{ true, false } && bothEligible.addSwing(), "both rolls boundary") && ok;
    ok = expect(hassoChecks == 1 && rollChecks == 2, "Hasso evaluated after normal proc") && ok;

    hassoChecks = rollChecks = 0;
    ok = expect(run(ResolutionHit, ReactionCounter, 40, false, false, { 39 }, hassoChecks, rollChecks).normal, "counter eligible") && ok;
    hassoChecks = rollChecks = 0;
    const auto hassoOnly = run(ResolutionHit, 0, 40, true, true, { 9 }, hassoChecks, rollChecks);
    ok = expect(hassoOnly == ProcResult{ false, true } && hassoOnly.addSwing(), "Hasso-only add swing") && ok;
    hassoChecks = rollChecks = 0;
    ok = expect(!run(1, 0, 0, false, false, { 0 }, hassoChecks, rollChecks).addSwing() && rollChecks == 1, "zero chance still rolls") && ok;

    ok = expect(ResolutionHit == 0 && ReactionCounter == 63, "pins") && ok;
    return ok;
}
