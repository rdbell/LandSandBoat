#include "test_jobpoints_apply_raise_3012.h"

#include "map/job_points_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jobpoints ShouldApplyRaiseJobPoint 3012 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RaiseJobPoint spend-gate formula for dual-wire cross-check (slice 3012):
//   cost != 0 && currentJp >= cost
auto inlineShouldApplyRaiseJobPoint(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

} // namespace

// Pure dual-wire expansion for jobpointshelpers::ShouldApplyRaiseJobPoint
// (cost != 0 && currentJp >= cost; slice 3012).
auto runJobpointsApplyRaise3012SelfTests() -> bool
{
    using namespace jobpointshelpers;

    bool ok = true;

    // Residual 2803 pins still hold under dual-wire.
    ok = expect(ShouldApplyRaiseJobPoint(1, 1), "residual: cost 1 / jp 1 applies") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 100), "residual: cost 0 blocks raise") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(10, 9), "residual: short JP blocks raise") && ok;
    ok = expect(ShouldApplyRaiseJobPoint(10, 10), "residual: exact JP applies") && ok;

    const struct
    {
        uint8       cost;
        uint16      currentJp;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 0, false, "cost 0 / jp 0 blocks" },
        { 0, 100, false, "cost 0 / surplus blocks (cap)" },
        { 0, 500, false, "cost 0 / max JP blocks" },
        { 1, 0, false, "cost 1 / jp 0 short" },
        { 1, 1, true, "cost 1 / jp 1 exact" },
        { 1, 50, true, "cost 1 / surplus" },
        { 10, 9, false, "cost 10 / jp 9 short" },
        { 10, 10, true, "cost 10 / jp 10 exact" },
        { 10, 11, true, "cost 10 / jp 11 surplus" },
        { 20, 19, false, "cost 20 / jp 19 short" },
        { 20, 20, true, "cost 20 / jp 20 exact" },
        { 20, 500, true, "cost 20 / max JP" },
        { 255, 254, false, "cost 255 / jp 254 short" },
        { 255, 255, true, "cost 255 / jp 255 exact" },
        { 255, 256, true, "cost 255 / jp 256 surplus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyRaiseJobPoint(c.cost, c.currentJp);
        const bool inlineF = inlineShouldApplyRaiseJobPoint(c.cost, c.currentJp);
        const bool wantPin = c.cost != 0 && c.currentJp >= c.cost;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyRaiseJobPoint dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyRaiseJobPoint == pin formula cost!=0 && currentJp>=cost") && ok;
    }

    // Pin composition: zero-cost gate is independent of JP.
    ok = expect(!ShouldApplyRaiseJobPoint(0, 0), "cost 0 / jp 0") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 1), "cost 0 / jp 1") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 65535), "cost 0 / jp max") && ok;
    // Exact equality boundary.
    ok = expect(ShouldApplyRaiseJobPoint(5, 5), "exact equal JP must apply when cost != 0") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(5, 4), "one short must block") && ok;

    // Host path: PlanRaiseJobPoint injects cost = JobPointCost(currentValue)
    // and currentJp after presence checks; ShouldRaiseAffordable injects the
    // same after JobPointCost only (no presence).
    const struct
    {
        uint8       value;
        uint16      currentJp;
        bool        wantApply;
        const char* label;
    } hostCases[] = {
        { 0, 0, false, "value0 cost1 / jp0 → reject" },
        { 0, 1, true, "value0 cost1 / jp1 → apply" },
        { 9, 5, false, "value9 cost10 / jp5 → reject" },
        { 9, 10, true, "value9 cost10 / jp10 → apply" },
        { 19, 19, false, "value19 cost20 / jp19 → reject" },
        { 19, 20, true, "value19 cost20 / jp20 → apply" },
        { 20, 500, false, "value20 cost0 / jp500 → reject (cap)" },
        { 4, 5, true, "value4 cost5 / jp5 → apply" },
    };

    for (const auto& c : hostCases)
    {
        const uint8 cost   = JobPointCost(c.value);
        const bool  got    = ShouldApplyRaiseJobPoint(cost, c.currentJp);
        const bool  inlineF = inlineShouldApplyRaiseJobPoint(cost, c.currentJp);
        const bool  wantPin = cost != 0 && c.currentJp >= cost;

        ok = expect(got == c.wantApply, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == wantPin, "host compose free == pin formula") && ok;
        ok = expect(ShouldRaiseAffordable(c.value, c.currentJp) == got,
                    "ShouldRaiseAffordable == ShouldApplyRaiseJobPoint(JobPointCost)") &&
             ok;
        {
            const auto plan = PlanRaiseJobPoint(true, true, c.value, c.currentJp);
            ok = expect(plan.apply == got && plan.cost == cost,
                        "PlanRaiseJobPoint Apply/Cost dual-wire through spend gate") &&
                 ok;
        }
    }

    // Dense compose over representative cost×jp poles.
    const uint8  costs[] = { 0, 1, 5, 10, 20, 255 };
    const uint16 jps[]   = { 0, 1, 4, 5, 9, 10, 19, 20, 254, 255, 256, 500 };
    for (const uint8 cost : costs)
    {
        for (const uint16 jp : jps)
        {
            const bool got  = ShouldApplyRaiseJobPoint(cost, jp);
            const bool want = cost != 0 && jp >= cost;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplyRaiseJobPoint(cost, jp),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (2803): spend gate is distinct from presence
    // short-circuit and JobPointCost formula that compose PlanRaiseJobPoint.
    {
        const auto missingJob = PlanRaiseJobPoint(false, true, 0, 100);
        ok = expect(!missingJob.apply && missingJob.cost == 0,
                    "PlanRaiseJobPoint missing-job residual still holds under dual-wire") &&
             ok;
    }
    {
        const auto missingType = PlanRaiseJobPoint(true, false, 0, 100);
        ok = expect(!missingType.apply && missingType.cost == 0,
                    "PlanRaiseJobPoint missing-type residual still holds under dual-wire") &&
             ok;
    }
    ok = expect(JobPointCost(0) == 1, "JobPointCost(0) residual") && ok;
    ok = expect(JobPointCost(20) == 0, "JobPointCost(20) residual") && ok;
    ok = expect(JobPointCost(19) == 20, "JobPointCost(19) residual") && ok;
    // Presence false still blocks even when spend gate alone would apply.
    ok = expect(ShouldApplyRaiseJobPoint(1, 1), "setup: spend gate alone would apply") && ok;
    {
        const auto blocked = PlanRaiseJobPoint(false, false, 0, 1);
        ok = expect(!blocked.apply, "presence short-circuit still independent of spend gate") && ok;
    }

    return ok;
}
