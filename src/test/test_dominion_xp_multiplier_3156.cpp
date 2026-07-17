#include "test_dominion_xp_multiplier_3156.h"

#include "map/dominion_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dominion XPMultiplier 3156 self-test failed: " << label << '\n';
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

// Compact dual-wire pin matching Go pinXPMultiplier3156:
//   if mainLvl < 75 then 1 - (75 - mainLvl) * 0.02 else 1.0
auto pinXPMultiplier(const int32 mainLvl) -> double
{
    if (mainLvl < 75)
    {
        return 1.0 - static_cast<double>(75 - mainLvl) * 0.02;
    }
    return 1.0;
}

auto nearlyEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

} // namespace

// Pure dual-wire expansion for dominionhelpers::XPMultiplier / RewardExp
// (Lua completeDominionOp level scale; OmegaXI internal/dominion;
// dedicated slice 3156).
//
// Coverage:
//   - poles 1 / 50 / 74 / 75 / 99
//   - free == inline == pin
//   - residual 1033 / 2873 pins still hold
auto runDominionXPMultiplier3156SelfTests() -> bool
{
    using dominionhelpers::RewardExp;
    using dominionhelpers::XPMultiplier;

    bool ok = true;

    // Residual 1033 / 2873 XPMultiplier pins still hold under dual-wire.
    ok = expect(nearlyEqual(XPMultiplier(75), 1.0), "residual XPMultiplier(75)") && ok;
    ok = expect(nearlyEqual(XPMultiplier(99), 1.0), "residual XPMultiplier(99)") && ok;
    ok = expect(nearlyEqual(XPMultiplier(50), 0.5), "residual XPMultiplier(50)") && ok;
    ok = expect(nearlyEqual(XPMultiplier(25), 0.0), "residual XPMultiplier(25)") && ok;
    ok = expect(nearlyEqual(XPMultiplier(74), 0.98), "residual XPMultiplier(74)") && ok;
    ok = expect(nearlyEqual(XPMultiplier(1), -0.48), "residual XPMultiplier(1)") && ok;

    // Residual RewardExp compose pins (1033 / 2873).
    constexpr uint16 baseReward = 1000; // dominion.lua baseRewardValue
    ok = expect(nearlyEqual(RewardExp(baseReward, 75), 1000.0), "residual RewardExp lvl75") && ok;
    ok = expect(nearlyEqual(RewardExp(baseReward, 50), 500.0), "residual RewardExp lvl50") && ok;

    // --- Composition table: free == inline == pin ---
    // Required poles: 1 / 50 / 74 / 75 / 99.
    const struct
    {
        int32       mainLvl;
        double      want;
        const char* label;
    } cases[] = {
        { 1, -0.48, "lvl 1 → -0.48" },
        { 50, 0.5, "lvl 50 → 0.5" },
        { 74, 0.98, "lvl 74 → 0.98" },
        { 75, 1.0, "lvl 75 → 1.0" },
        { 99, 1.0, "lvl 99 → 1.0" },
        // Extra residual / boundary pins for free==inline==pin.
        { 0, -0.5, "lvl 0 → -0.5" },
        { 25, 0.0, "lvl 25 → 0.0" },
        { 76, 1.0, "lvl 76 → 1.0" },
        { 100, 1.0, "lvl 100 → 1.0" },
        { 70, 0.9, "lvl 70 → 0.9" },
        { 60, 0.7, "lvl 60 → 0.7" },
        { 24, -0.02, "lvl 24 → -0.02" },
    };

    for (const auto& c : cases)
    {
        const double got     = XPMultiplier(c.mainLvl);
        const double inlineF = inlineXPMultiplier(c.mainLvl);
        const double pin     = pinXPMultiplier(c.mainLvl);

        ok = expect(nearlyEqual(got, c.want), c.label) && ok;
        ok = expect(nearlyEqual(got, inlineF) && nearlyEqual(got, pin),
                    "dual-wire free == inline == pin") &&
             ok;
    }

    // Free == pin across residual poles (positive pin form).
    const int32 residualPoles[] = { 1, 50, 74, 75, 99 };
    for (const int32 lvl : residualPoles)
    {
        ok = expect(nearlyEqual(XPMultiplier(lvl), pinXPMultiplier(lvl)),
                    "free == pin residual pole") &&
             ok;
    }

    // RewardExp dual-wire: free == base * pin.
    for (const int32 lvl : residualPoles)
    {
        const double got  = RewardExp(baseReward, lvl);
        const double want = static_cast<double>(baseReward) * pinXPMultiplier(lvl);
        ok = expect(nearlyEqual(got, want), "RewardExp pin compose") && ok;
    }

    return ok;
}
