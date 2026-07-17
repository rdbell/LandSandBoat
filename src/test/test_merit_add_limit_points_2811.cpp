#include "test_merit_add_limit_points_2811.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

// Production MAX_LIMIT_POINTS (merit.cpp).
constexpr uint16 kMaxLimitPoints = 10000;

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit add limit points 2811 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU16(const uint16 got, const uint16 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "merit add limit points 2811 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

auto expectU8(const uint8 got, const uint8 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "merit add limit points 2811 self-test failed: " << label
                  << " got=" << static_cast<unsigned>(got)
                  << " want=" << static_cast<unsigned>(want) << '\n';
        return false;
    }
    return true;
}

auto expectPlan(
    const meritshelpers::AddLimitPointsPlan& plan,
    const uint16                             wantLimit,
    const uint8                              wantMerit,
    const bool                               wantIncreased,
    const char* const                        labelLimit,
    const char* const                        labelMerit,
    const char* const                        labelIncreased) -> bool
{
    bool ok = true;
    ok      = expectU16(plan.newLimit, wantLimit, labelLimit) && ok;
    ok      = expectU8(plan.newMerit, wantMerit, labelMerit) && ok;
    ok      = expect(plan.meritIncreased == wantIncreased, labelIncreased) && ok;
    return ok;
}

} // namespace

auto runMeritAddLimitPoints2811SelfTests() -> bool
{
    using namespace meritshelpers;

    bool ok = true;

    constexpr uint8 cap30 = 30;

    // --- below conversion threshold ---
    {
        const auto plan = PlanAddLimitPoints(100, 5, 50, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 150, 5, false,
                         "below threshold limit", "below threshold merit", "below threshold increased") &&
             ok;
    }
    {
        const auto plan = PlanAddLimitPoints(0, 0, kMaxLimitPoints - 1, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, kMaxLimitPoints - 1, 0, false,
                         "just under threshold limit", "just under threshold merit", "just under threshold increased") &&
             ok;
    }

    // --- exact one merit conversion ---
    {
        const auto plan = PlanAddLimitPoints(0, 0, kMaxLimitPoints, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 0, 1, true,
                         "exactly one merit limit", "exactly one merit merit", "exactly one merit increased") &&
             ok;
    }

    // --- remainder after conversion ---
    // 500 + 10250 = 10750 → +1 merit, remainder 750
    {
        const auto plan = PlanAddLimitPoints(500, 2, kMaxLimitPoints + 250, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 750, 3, true,
                         "remainder limit", "remainder merit", "remainder increased") &&
             ok;
    }

    // --- multiple merits in one add ---
    {
        const auto plan = PlanAddLimitPoints(0, 0, kMaxLimitPoints * 3 + 123, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 123, 3, true,
                         "multiple merits limit", "multiple merits merit", "multiple merits increased") &&
             ok;
    }

    // --- caps at max merits ---
    // min(28+5, 30) = 30, remainder 0
    {
        const auto plan = PlanAddLimitPoints(0, 28, kMaxLimitPoints * 5, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 0, 30, true,
                         "caps at max limit", "caps at max merit", "caps at max increased") &&
             ok;
    }

    // --- already at max freezes limit points at threshold ---
    // Below threshold, points still accumulate even at merit cap.
    {
        const auto plan = PlanAddLimitPoints(100, 30, 5000, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 5100, 30, false,
                         "at cap pre-threshold limit", "at cap pre-threshold merit", "at cap pre-threshold increased") &&
             ok;
    }
    // Crossing MaxLimitPoints while at merit cap freezes limit to MaxLimitPoints-1.
    {
        const auto plan = PlanAddLimitPoints(5100, 30, 6000, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, kMaxLimitPoints - 1, 30, false,
                         "at cap freeze limit", "at cap freeze merit", "at cap freeze increased") &&
             ok;
    }

    // --- bonus expands cap (base 30 + bonus 5 → cap 35) ---
    {
        const auto plan = PlanAddLimitPoints(0, 30, kMaxLimitPoints, 35, kMaxLimitPoints);
        ok              = expectPlan(plan, 0, 31, true,
                         "bonus expands cap limit", "bonus expands cap merit", "bonus expands cap increased") &&
             ok;
    }

    // --- zero add / zero state ---
    {
        const auto plan = PlanAddLimitPoints(0, 0, 0, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 0, 0, false,
                         "zero add limit", "zero add merit", "zero add increased") &&
             ok;
    }

    // --- conversion with existing remainder that crosses ---
    // 9999 + 1 = 10000 → +1, remainder 0
    {
        const auto plan = PlanAddLimitPoints(kMaxLimitPoints - 1, 7, 1, cap30, kMaxLimitPoints);
        ok              = expectPlan(plan, 0, 8, true,
                         "cross with one point limit", "cross with one point merit", "cross with one point increased") &&
             ok;
    }

    // --- injected maxLimitPoints other than production (scaled math) ---
    {
        constexpr uint16 maxLP = 100;
        const auto       plan  = PlanAddLimitPoints(50, 1, 150, cap30, maxLP);
        // 50+150=200 → +2 merits, remainder 0
        ok = expectPlan(plan, 0, 3, true,
                        "scaled maxLimitPoints limit", "scaled maxLimitPoints merit", "scaled maxLimitPoints increased") &&
             ok;
    }
    {
        constexpr uint16 maxLP = 100;
        const auto       plan  = PlanAddLimitPoints(0, 30, 100, cap30, maxLP);
        ok                     = expectPlan(plan, 99, 30, false,
                         "scaled freeze limit", "scaled freeze merit", "scaled freeze increased") &&
             ok;
    }

    return ok;
}
