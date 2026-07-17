#include "test_dominion_xp_multiplier_2873.h"

#include "map/dominion_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dominion XPMultiplier 2873 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua completeDominionOp xpMultiplier formula for dual-wire cross-check:
//   local xpMultiplier = 1
//   if mainLvl < 75 then
//     xpMultiplier = xpMultiplier - (75 - mainLvl) * .02
//   end
auto inlineXPMultiplier(const int32 mainLvl) -> double
{
    double xpMultiplier = 1.0;
    if (mainLvl < 75)
    {
        xpMultiplier = xpMultiplier - static_cast<double>(75 - mainLvl) * 0.02;
    }
    return xpMultiplier;
}

auto nearlyEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

} // namespace

// Pure dual-wire expansion for dominionhelpers::XPMultiplier / RewardExp
// (Lua completeDominionOp level scale).
auto runDominionXPMultiplier2873SelfTests() -> bool
{
    using dominionhelpers::RewardExp;
    using dominionhelpers::XPMultiplier;

    bool ok = true;

    const struct
    {
        int32       mainLvl;
        double      want;
        const char* label;
    } cases[] = {
        // Boundary at 75 (full multiplier).
        { 75, 1.0, "lvl 75 → 1.0" },
        { 74, 0.98, "lvl 74 → 0.98" },
        { 76, 1.0, "lvl 76 → 1.0" },
        { 100, 1.0, "lvl 100 → 1.0" },
        { 99, 1.0, "lvl 99 → 1.0" },
        // Mid-scale pins.
        { 50, 0.5, "lvl 50 → 0.5" },
        { 25, 0.0, "lvl 25 → 0.0" },
        { 70, 0.9, "lvl 70 → 0.9" },
        { 60, 0.7, "lvl 60 → 0.7" },
        { 40, 0.3, "lvl 40 → 0.3" },
        // Below zero floor (mirrors Lua; no clamp).
        { 24, -0.02, "lvl 24 → -0.02" },
        { 0, -0.5, "lvl 0 → -0.5" },
        { 1, -0.48, "lvl 1 → -0.48" },
    };

    for (const auto& c : cases)
    {
        const double got     = XPMultiplier(c.mainLvl);
        const double inlineF = inlineXPMultiplier(c.mainLvl);

        ok = expect(nearlyEqual(got, c.want), c.label) && ok;
        ok = expect(nearlyEqual(got, inlineF), "dual-wire free == inline Lua formula") && ok;
    }

    // RewardExp compose: baseReward * XPMultiplier.
    constexpr uint16 baseReward = 1000; // dominion.lua baseRewardValue
    ok = expect(nearlyEqual(RewardExp(baseReward, 75), 1000.0), "RewardExp lvl75 = 1000") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 50), 500.0), "RewardExp lvl50 = 500") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 25), 0.0), "RewardExp lvl25 = 0") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 74), 980.0), "RewardExp lvl74 = 980") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 100), 1000.0), "RewardExp lvl100 = 1000") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 0), -500.0), "RewardExp lvl0 = -500") && ok;

    // Dual-wire: RewardExp == base * inline multiplier.
    ok = expect(nearlyEqual(RewardExp(baseReward, 60), static_cast<double>(baseReward) * inlineXPMultiplier(60)),
                "RewardExp dual-wire compose lvl60") &&
         ok;
    ok = expect(nearlyEqual(RewardExp(500, 70), 500.0 * inlineXPMultiplier(70)),
                "RewardExp dual-wire compose half base") &&
         ok;

    return ok;
}
