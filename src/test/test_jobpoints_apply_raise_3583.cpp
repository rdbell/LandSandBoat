#include "test_jobpoints_apply_raise_3583.h"

#include "map/job_points_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jobpoints ShouldApplyRaiseJobPoint 3583 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RaiseJobPoint spend-gate formula for dual-wire cross-check
// (dedicated slice 3583):
//   cost != 0 && currentJp >= cost
auto inlineShouldApplyRaiseJobPoint3583(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

// Compact dual-wire pin matching Go pinShouldApplyRaiseJobPoint3583 / C++ capacity:
//   cost != 0 && currentJp >= cost
// Direct return of the formula (not if/else true/false).
auto pinShouldApplyRaiseJobPoint3583(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

// Prior dedicated 3539 pin surface (free == pin3539 independence).
auto pinShouldApplyRaiseJobPoint3539(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

auto inlineShouldApplyRaiseJobPoint3539(const uint8 cost, const uint16 currentJp) -> bool
{
    return cost != 0 && currentJp >= cost;
}

} // namespace

// Pure dual-wire expansion for jobpointshelpers::ShouldApplyRaiseJobPoint
// (RaiseJobPoint spend gate; OmegaXI internal/jobpoints;
// dedicated slice 3583 expand residual 3012 / prior dedicated 3219 /
// prior dedicated 3275 / prior dedicated 3371 / prior dedicated 3421 /
// prior dedicated 3475 / prior dedicated 3539).
//
// Coverage:
//   - free == inline == pin == pin3539 == (cost != 0 && currentJp >= cost)
//   - residual 2803 / 3012 / prior dedicated 3219 / prior dedicated 3275 /
//     prior dedicated 3371 / prior dedicated 3421 / prior dedicated 3475 /
//     prior dedicated 3539 pins still hold
//   - dense poles: cost 0 any JP, exact equality, short-by-one,
//     uint8 cost / uint16 jp boundaries (0,1,10,20,255 costs;
//     0,1,254,255,256,500,65535 jp)
auto runJobpointsApplyRaise3583SelfTests() -> bool
{
    using namespace jobpointshelpers;

    bool ok = true;

    // Residual 2803 / 3012 / prior dedicated 3219 / prior dedicated 3275 /
    // prior dedicated 3371 / prior dedicated 3421 / prior dedicated 3475 /
    // prior dedicated 3539 pins still hold under dual-wire.
    ok = expect(ShouldApplyRaiseJobPoint(1, 1), "residual 2803/3012/3219/3275/3371/3421/3475/3539: cost 1 / jp 1 applies") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 100), "residual 2803/3012/3219/3275/3371/3421/3475/3539: cost 0 blocks raise") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(10, 9), "residual 2803/3012/3219/3275/3371/3421/3475/3539: short JP blocks raise") && ok;
    ok = expect(ShouldApplyRaiseJobPoint(10, 10), "residual 2803/3012/3219/3275/3371/3421/3475/3539: exact JP applies") && ok;

    // --- Composition table: free == inline == pin == pin3539 ---
    const struct
    {
        uint8       cost;
        uint16      currentJp;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2803 / 3012 / prior dedicated 3219 / prior dedicated 3275 /
        // prior dedicated 3371 / prior dedicated 3421 / prior dedicated 3475 /
        // prior dedicated 3539 poles.
        { 0, 0, false, "residual cost 0 / jp 0 blocks" },
        { 0, 100, false, "residual cost 0 / surplus blocks (cap)" },
        { 0, 500, false, "residual cost 0 / max JP blocks" },
        { 1, 0, false, "residual cost 1 / jp 0 short" },
        { 1, 1, true, "residual cost 1 / jp 1 exact" },
        { 1, 50, true, "residual cost 1 / surplus" },
        { 10, 9, false, "residual cost 10 / jp 9 short" },
        { 10, 10, true, "residual cost 10 / jp 10 exact" },
        { 10, 11, true, "residual cost 10 / jp 11 surplus" },
        { 20, 19, false, "residual cost 20 / jp 19 short" },
        { 20, 20, true, "residual cost 20 / jp 20 exact" },
        { 20, 500, true, "residual cost 20 / max JP" },
        { 255, 254, false, "residual cost 255 / jp 254 short" },
        { 255, 255, true, "residual cost 255 / jp 255 exact" },
        { 255, 256, true, "residual cost 255 / jp 256 surplus" },

        // Pole: cost 0 blocks any JP.
        { 0, 0, false, "pole cost0 jp0" },
        { 0, 1, false, "pole cost0 jp1" },
        { 0, 254, false, "pole cost0 jp254" },
        { 0, 255, false, "pole cost0 jp255" },
        { 0, 256, false, "pole cost0 jp256" },
        { 0, 500, false, "pole cost0 jp500" },
        { 0, 65535, false, "pole cost0 jp max uint16" },

        // Pole: exact equality (currentJp == cost, cost != 0).
        { 1, 1, true, "pole exact cost1" },
        { 10, 10, true, "pole exact cost10" },
        { 20, 20, true, "pole exact cost20" },
        { 255, 255, true, "pole exact cost255" },

        // Pole: short-by-one (currentJp == cost-1).
        { 1, 0, false, "pole short cost1" },
        { 10, 9, false, "pole short cost10" },
        { 20, 19, false, "pole short cost20" },
        { 255, 254, false, "pole short cost255" },

        // Boundary neighbors around cost.
        { 10, 9, false, "one short of cost10" },
        { 10, 10, true, "exact cost10" },
        { 10, 11, true, "one over cost10" },
        { 20, 19, false, "one short of cost20" },
        { 20, 20, true, "exact cost20" },
        { 20, 21, true, "one over cost20" },
        { 255, 254, false, "one short of cost255" },
        { 255, 255, true, "exact cost255" },
        { 255, 256, true, "one over cost255" },

        // uint8 cost / uint16 jp boundary poles.
        { 1, 0, false, "boundary cost1 jp0" },
        { 1, 1, true, "boundary cost1 jp1" },
        { 1, 254, true, "boundary cost1 jp254" },
        { 1, 255, true, "boundary cost1 jp255" },
        { 1, 256, true, "boundary cost1 jp256" },
        { 1, 500, true, "boundary cost1 jp500" },
        { 1, 65535, true, "boundary cost1 jp max" },
        { 255, 0, false, "boundary cost255 jp0" },
        { 255, 1, false, "boundary cost255 jp1" },
        { 255, 254, false, "boundary cost255 jp254" },
        { 255, 255, true, "boundary cost255 jp255" },
        { 255, 256, true, "boundary cost255 jp256" },
        { 255, 500, true, "boundary cost255 jp500" },
        { 255, 65535, true, "boundary cost255 jp max" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyRaiseJobPoint(c.cost, c.currentJp);
        const bool inlineF = inlineShouldApplyRaiseJobPoint3583(c.cost, c.currentJp);
        const bool pin     = pinShouldApplyRaiseJobPoint3583(c.cost, c.currentJp);
        const bool pin3539 = pinShouldApplyRaiseJobPoint3539(c.cost, c.currentJp);
        const bool wantPin = c.cost != 0 && c.currentJp >= c.cost;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin == pin3539.
        ok = expect(got == inlineF && got == pin && got == pin3539, "dual-wire free == inline == pin == pin3539") && ok;
        ok = expect(got == wantPin, "free == pin formula cost!=0 && currentJp>=cost") && ok;
    }

    // Pin composition: zero-cost gate is independent of JP.
    ok = expect(!ShouldApplyRaiseJobPoint(0, 0), "cost 0 / jp 0") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 1), "cost 0 / jp 1") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(0, 65535), "cost 0 / jp max") && ok;
    // Exact equality boundary.
    ok = expect(ShouldApplyRaiseJobPoint(5, 5), "exact equal JP must apply when cost != 0") && ok;
    ok = expect(!ShouldApplyRaiseJobPoint(5, 4), "one short must block") && ok;

    // Explicit poles free == inline == pin == pin3539 for cost 0 / exact / short-by-one.
    const uint8 poleCosts[] = { 0, 1, 10, 20, 255 };
    for (const uint8 cost : poleCosts)
    {
        if (cost == 0)
        {
            const uint16 zeroJPs[] = { 0, 1, 254, 255, 256, 500, 65535 };
            for (const uint16 jp : zeroJPs)
            {
                const bool got     = ShouldApplyRaiseJobPoint(cost, jp);
                const bool inlineF = inlineShouldApplyRaiseJobPoint3583(cost, jp);
                const bool pin     = pinShouldApplyRaiseJobPoint3583(cost, jp);
                const bool pin3539 = pinShouldApplyRaiseJobPoint3539(cost, jp);
                ok = expect(!got && got == inlineF && got == pin && got == pin3539,
                            "pole cost0 free == inline == pin == pin3539") &&
                     ok;
            }
            continue;
        }
        // exact equality
        {
            const uint16 jp      = static_cast<uint16>(cost);
            const bool   got     = ShouldApplyRaiseJobPoint(cost, jp);
            const bool   inlineF = inlineShouldApplyRaiseJobPoint3583(cost, jp);
            const bool   pin     = pinShouldApplyRaiseJobPoint3583(cost, jp);
            const bool   pin3539 = pinShouldApplyRaiseJobPoint3539(cost, jp);
            ok                   = expect(got && got == inlineF && got == pin && got == pin3539,
                        "pole exact free == inline == pin == pin3539") &&
                 ok;
        }
        // short-by-one
        {
            const uint16 jp      = static_cast<uint16>(cost - 1);
            const bool   got     = ShouldApplyRaiseJobPoint(cost, jp);
            const bool   inlineF = inlineShouldApplyRaiseJobPoint3583(cost, jp);
            const bool   pin     = pinShouldApplyRaiseJobPoint3583(cost, jp);
            const bool   pin3539 = pinShouldApplyRaiseJobPoint3539(cost, jp);
            ok = expect(!got && got == inlineF && got == pin && got == pin3539,
                        "pole short free == inline == pin == pin3539") &&
                 ok;
        }
    }

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
        const uint8 cost    = JobPointCost(c.value);
        const bool  got     = ShouldApplyRaiseJobPoint(cost, c.currentJp);
        const bool  inlineF = inlineShouldApplyRaiseJobPoint3583(cost, c.currentJp);
        const bool  pin     = pinShouldApplyRaiseJobPoint3583(cost, c.currentJp);
        const bool  pin3539 = pinShouldApplyRaiseJobPoint3539(cost, c.currentJp);
        const bool  wantPin = cost != 0 && c.currentJp >= cost;

        ok = expect(got == c.wantApply, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == pin3539,
                    "host compose dual-wire free == inline == pin == pin3539") &&
             ok;
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

    // Dense compose over representative cost×jp poles (task boundary set).
    const uint8  costs[]    = { 0, 1, 10, 20, 255 };
    const uint16 jps[]      = { 0, 1, 254, 255, 256, 500, 65535 };
    const uint16 extraJPs[] = { 4, 5, 9, 10, 19, 20 };
    for (const uint8 cost : costs)
    {
        for (const uint16 jp : jps)
        {
            const bool got     = ShouldApplyRaiseJobPoint(cost, jp);
            const bool inlineF = inlineShouldApplyRaiseJobPoint3583(cost, jp);
            const bool pin     = pinShouldApplyRaiseJobPoint3583(cost, jp);
            const bool pin3539 = pinShouldApplyRaiseJobPoint3539(cost, jp);
            const bool want    = cost != 0 && jp >= cost;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin && got == pin3539,
                        "compose free == inline == pin == pin3539") &&
                 ok;
        }
        for (const uint16 jp : extraJPs)
        {
            const bool got     = ShouldApplyRaiseJobPoint(cost, jp);
            const bool inlineF = inlineShouldApplyRaiseJobPoint3583(cost, jp);
            const bool pin     = pinShouldApplyRaiseJobPoint3583(cost, jp);
            const bool pin3539 = pinShouldApplyRaiseJobPoint3539(cost, jp);
            const bool want    = cost != 0 && jp >= cost;
            ok                 = expect(got == want, "compose extra free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin && got == pin3539,
                        "compose extra free == inline == pin == pin3539") &&
                 ok;
        }
    }

    // Residual independence (2803 / 3012 / prior 3219 / prior 3275 / prior 3371 /
    // prior 3421 / prior 3475 / prior 3539): spend gate is distinct from presence
    // short-circuit and JobPointCost formula that compose PlanRaiseJobPoint.
    // Prior dedicated 3539 independence: free == prior suite inline/pin forms
    // (local mirrors of test_jobpoints_apply_raise_3539 pin surface).
    for (const uint8 cost : costs)
    {
        for (const uint16 jp : jps)
        {
            const bool got = ShouldApplyRaiseJobPoint(cost, jp);
            ok = expect(got == inlineShouldApplyRaiseJobPoint3539(cost, jp),
                        "prior 3539 independence free == 3539 inline") &&
                 ok;
            ok = expect(got == pinShouldApplyRaiseJobPoint3539(cost, jp),
                        "prior 3539 independence free == 3539 pin") &&
                 ok;
        }
        for (const uint16 jp : extraJPs)
        {
            const bool got = ShouldApplyRaiseJobPoint(cost, jp);
            ok = expect(got == inlineShouldApplyRaiseJobPoint3539(cost, jp),
                        "prior 3539 independence extra free == 3539 inline") &&
                 ok;
            ok = expect(got == pinShouldApplyRaiseJobPoint3539(cost, jp),
                        "prior 3539 independence extra free == 3539 pin") &&
                 ok;
        }
    }
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
