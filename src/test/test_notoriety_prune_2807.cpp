#include "test_notoriety_prune_2807.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety prune 2807 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNotorietyPrune2807SelfTests() -> bool
{
    using namespace notorietyhelpers;

    bool ok = true;

    // ShouldScanNotorietyForPrune: owner present && lookup non-empty
    ok = expect(ShouldScanNotorietyForPrune(true, true), "scan owner+nonempty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true), "no scan nil owner") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false), "no scan empty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false), "no scan nil+empty") && ok;

    // Non-mobs never pruned regardless of liveness / enmity flags
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true), "non-mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, true, false), "non-mob dead") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, false, false, true), "non-mob transitional") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, true, true), "non-mob all true") && ok;

    // Mob: (alive && notOnEnmityList) || dead
    ok = expect(ShouldPruneMobFromNotoriety(true, true, false, true), "mob live missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, true, false, false), "mob live with owner") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, false), "mob dead with owner") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, false, true, true), "mob dead missing") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, true), "mob transitional missing retained") && ok;
    ok = expect(!ShouldPruneMobFromNotoriety(true, false, false, false), "mob transitional with owner retained") && ok;

    // Alive+dead both true still prunes (dead branch); host injects both
    ok = expect(ShouldPruneMobFromNotoriety(true, true, true, false), "mob alive+dead with owner") && ok;
    ok = expect(ShouldPruneMobFromNotoriety(true, true, true, true), "mob alive+dead missing") && ok;

    // Exhaustive truth table for isMob=true over alive/dead/notOnEnmityList
    const struct
    {
        bool        isAlive;
        bool        isDead;
        bool        notOnEnmityList;
        bool        wantPrune;
        const char* label;
    } cases[] = {
        { true, false, true, true, "table live missing" },
        { true, false, false, false, "table live present" },
        { false, true, true, true, "table dead missing" },
        { false, true, false, true, "table dead present" },
        { false, false, true, false, "table neither missing" },
        { false, false, false, false, "table neither present" },
        { true, true, true, true, "table both missing" },
        { true, true, false, true, "table both present" },
    };
    for (const auto& c : cases)
    {
        const bool got = ShouldPruneMobFromNotoriety(true, c.isAlive, c.isDead, c.notOnEnmityList);
        ok             = expect(got == c.wantPrune, c.label) && ok;
        const bool expected =
            (c.isAlive && c.notOnEnmityList) || c.isDead;
        ok = expect(got == expected, "compose expression") && ok;
        ok = expect(!ShouldPruneMobFromNotoriety(false, c.isAlive, c.isDead, c.notOnEnmityList),
                    "non-mob mirror table") &&
            ok;
    }

    return ok;
}
