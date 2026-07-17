#include "test_merit_should_raise_merit_3160.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit ShouldRaiseMerit 3160 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RaiseMerit admission formula for dual-wire cross-check
// (slice 3160; residual 2805):
//   meritPoints >= nextCost && count < upgradeMax && categoryCount < categoryMaxPoints
auto inlineShouldRaiseMerit(
    const uint16 meritPoints,
    const uint16 nextCost,
    const uint8  count,
    const uint8  upgradeMax,
    const uint16 categoryCount,
    const uint8  categoryMaxPoints) -> bool
{
    return meritPoints >= nextCost && count < upgradeMax && categoryCount < categoryMaxPoints;
}

// Positive if/else pin matching free function / capacity body (slice 3160).
// Avoid De Morgan rewrites of the compound-not form.
auto pinShouldRaiseMerit(
    const uint16 meritPoints,
    const uint16 nextCost,
    const uint8  count,
    const uint8  upgradeMax,
    const uint16 categoryCount,
    const uint8  categoryMaxPoints) -> bool
{
    if (meritPoints < nextCost)
    {
        return false;
    }
    if (count >= upgradeMax)
    {
        return false;
    }
    if (categoryCount >= categoryMaxPoints)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for meritshelpers::ShouldRaiseMerit
// (points/upgrade/category RaiseMerit admission gate; slice 3160). Residual
// 2805 pins still hold; free == inline == pin (positive if/else).
auto runMeritShouldRaiseMerit3160SelfTests() -> bool
{
    using meritshelpers::PlanRaiseMerit;
    using meritshelpers::ShouldRaiseMerit;

    bool ok = true;

    // Residual 2805 pins still hold under dual-wire.
    ok = expect(ShouldRaiseMerit(1, 1, 0, 15, 0, 75), "residual exact points should raise") && ok;
    ok = expect(ShouldRaiseMerit(20, 1, 0, 15, 0, 75), "residual surplus points should raise") && ok;
    ok = expect(!ShouldRaiseMerit(0, 1, 0, 15, 0, 75), "residual zero points should block") && ok;
    ok = expect(!ShouldRaiseMerit(4, 5, 4, 15, 0, 75), "residual short one point should block") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 5, 5, 0, 10), "residual count == upgradeMax should block") && ok;
    ok = expect(ShouldRaiseMerit(100, 1, 4, 5, 0, 10), "residual count just under upgradeMax should raise") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 0, 5, 10, 10), "residual categoryCount == MaxPoints should block") && ok;
    ok = expect(ShouldRaiseMerit(100, 1, 0, 5, 9, 10), "residual categoryCount just under MaxPoints should raise") && ok;

    const struct
    {
        uint16      meritPoints;
        uint16      nextCost;
        uint8       count;
        uint8       upgradeMax;
        uint16      categoryCount;
        uint8       categoryMaxPoints;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual 2805 success poles.
        { 1, 1, 0, 15, 0, 75, true, "exact points admits raise" },
        { 20, 1, 0, 15, 0, 75, true, "surplus points admits raise" },
        { 5, 5, 4, 15, 9, 10, true, "exact under category cap admits raise" },

        // Residual 2805 insufficient points.
        { 0, 1, 0, 15, 0, 75, false, "zero points blocks raise" },
        { 4, 5, 4, 15, 0, 75, false, "short one point blocks raise" },

        // Residual 2805 upgrade max.
        { 100, 1, 5, 5, 0, 10, false, "count == upgradeMax blocks raise" },
        { 100, 1, 6, 5, 0, 10, false, "count > upgradeMax blocks raise" },
        { 100, 1, 4, 5, 0, 10, true, "count just under upgradeMax admits raise" },

        // Residual 2805 category cap.
        { 100, 1, 0, 5, 10, 10, false, "categoryCount == MaxPoints blocks raise" },
        { 100, 1, 0, 5, 11, 10, false, "categoryCount > MaxPoints blocks raise" },
        { 100, 1, 0, 5, 9, 10, true, "categoryCount just under MaxPoints admits raise" },

        // Dense boundary poles.
        { 0, 0, 0, 1, 0, 1, true, "zero cost zero points admits when under caps" },
        { 100, 1, 0, 0, 0, 10, false, "upgradeMax 0 blocks any count" },
        { 100, 1, 0, 1, 0, 0, false, "categoryMaxPoints 0 blocks any categoryCount" },
        { 100, 1, 14, 15, 74, 75, true, "under both caps admits" },
        { 100, 1, 15, 15, 74, 75, false, "at upgrade max blocks despite category room" },
        { 100, 1, 14, 15, 75, 75, false, "at category cap blocks despite upgrade room" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const bool inlineF = inlineShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const bool pinF    = pinShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const bool wantPin = c.meritPoints >= c.nextCost && c.count < c.upgradeMax && c.categoryCount < c.categoryMaxPoints;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRaiseMerit dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldRaiseMerit dual-wire == positive if/else pin") && ok;
        ok = expect(got == wantPin, "ShouldRaiseMerit == pin formula AND gate") && ok;
    }

    // Pin composition: admit raise iff all three gates pass.
    ok = expect(ShouldRaiseMerit(1, 1, 0, 15, 0, 75), "exact under all caps must admit") && ok;
    ok = expect(!ShouldRaiseMerit(0, 1, 0, 15, 0, 75), "points gate alone must block") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 5, 5, 0, 10), "upgrade gate alone must block") && ok;
    ok = expect(!ShouldRaiseMerit(100, 1, 0, 5, 10, 10), "category gate alone must block") && ok;

    // Host-style compose: RaiseMerit injects balances/fields into ShouldRaiseMerit.
    const struct
    {
        uint16      meritPoints;
        uint16      nextCost;
        uint8       count;
        uint8       upgradeMax;
        uint16      categoryCount;
        uint8       categoryMaxPoints;
        bool        wantRaise;
        const char* label;
    } composeCases[] = {
        { 1, 1, 0, 15, 0, 75, true, "exact points admits raise" },
        { 20, 5, 4, 15, 4, 75, true, "surplus with cost 5 admits raise" },
        { 50, 1, 0, 5, 9, 10, true, "under category cap admits raise" },
        { 0, 1, 0, 15, 0, 75, false, "zero points blocks raise" },
        { 50, 1, 5, 5, 0, 10, false, "upgrade max blocks raise" },
        { 50, 3, 0, 5, 10, 10, false, "category cap blocks raise" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        ok             = expect(got == c.wantRaise, c.label) && ok;
        ok             = expect(got == inlineShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "compose free == pin") &&
             ok;
    }

    // Sibling residual PlanRaiseMerit is orthogonal composition on this gate:
    // apply = ShouldRaiseMerit(...); spend = nextCost when apply else 0.
    for (const auto& c : composeCases)
    {
        const auto plan      = PlanRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const bool wantApply = ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const uint16 wantSpend = wantApply ? c.nextCost : uint16{ 0 };
        ok                     = expect(plan.apply == wantApply, "PlanRaiseMerit compose apply") && ok;
        ok                     = expect(plan.spend == wantSpend, "PlanRaiseMerit compose spend") && ok;
        ok                     = expect(ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints) ==
                        pinShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "gate free == pin under PlanRaiseMerit compose") &&
             ok;
        ok = expect(ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints) ==
                        inlineShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "gate free == inline under PlanRaiseMerit compose") &&
             ok;
    }

    // Explicit dual-wire poles: free == inline == pin across residual poles.
    for (const auto& c : cases)
    {
        const bool got  = ShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints);
        const bool want = c.meritPoints >= c.nextCost && c.count < c.upgradeMax && c.categoryCount < c.categoryMaxPoints;
        ok              = expect(got == want, "host inject dual-wire pin") && ok;
        ok              = expect(got == inlineShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "host inject free == inline") &&
             ok;
        ok = expect(got == pinShouldRaiseMerit(c.meritPoints, c.nextCost, c.count, c.upgradeMax, c.categoryCount, c.categoryMaxPoints),
                    "host inject free == pin") &&
             ok;
    }

    return ok;
}
