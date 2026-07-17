#include "test_artisan_gobbie_upgrade_flag_3147.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan GobbieCanUpgradeFlag 3147 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 2 pure failure param for dual-wire cross-check:
//   gobbieSize < 80 and 1 or 0
auto inlineGobbieCanUpgradeFlag(const int32 gobbieSize) -> int32
{
    return gobbieSize < 80 ? 1 : 0;
}

// Compact dual-wire pin matching Go pinGobbieCanUpgradeFlag3147:
//   gobbieSize < GobbieUpgradeCap ? 1 : 0
auto pinGobbieCanUpgradeFlag(const int32 gobbieSize) -> int32
{
    return gobbieSize < artisanhelpers::GobbieUpgradeCap ? 1 : 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::GobbieCanUpgradeFlag
// (Lua moogleOnUpdate option 2 expand-failure event param; OmegaXI
// internal/artisan; slice 3147).
//
// Coverage:
//   - poles 0 / 79 / 80 / 81 / 100 / negative
//   - free == inline == pin == gobbieSize < GobbieUpgradeCap ? 1 : 0
//   - GobbieUpgradeCap == 80
//   - residual 0948 / 2912 pins still hold
auto runArtisanGobbieUpgradeFlag3147SelfTests() -> bool
{
    using artisanhelpers::GobbieCanUpgradeFlag;
    using artisanhelpers::GobbieUpgradeCap;

    bool ok = true;

    // Cap pin: 80 from artisan.lua gobbieSize < 80.
    ok = expect(GobbieUpgradeCap == 80, "GobbieUpgradeCap == 80") && ok;

    // Residual 0948 / 2912 pins still hold under dual-wire.
    ok = expect(GobbieCanUpgradeFlag(79) == 1, "residual: gobbieSize 79 -> 1") && ok;
    ok = expect(GobbieCanUpgradeFlag(80) == 0, "residual: gobbieSize 80 -> 0") && ok;
    ok = expect(GobbieCanUpgradeFlag(0) == 1, "residual: gobbieSize 0 -> 1") && ok;

    // --- Required poles: free == inline == pin == formula ---
    // Positive form: below-cap path returns 1; at/above-cap path returns 0.
    const struct
    {
        int32       gobbieSize;
        int32       want;
        const char* label;
    } cases[] = {
        // required poles
        { 0, 1, "pole 0 -> 1" },
        { 79, 1, "pole 79 (cap-1) -> 1" },
        { 80, 0, "pole 80 (exact cap) -> 0" },
        { 81, 0, "pole 81 (cap+1) -> 0" },
        { 100, 0, "pole 100 -> 0" },
        { -1, 1, "pole negative -> 1" },
        { -100, 1, "pole large negative -> 1" },
        // residual 2912 / 0948 extras
        { 1, 1, "min positive" },
        { 30, 1, "buy sack size" },
        { 50, 1, "mid inventory" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = GobbieCanUpgradeFlag(c.gobbieSize);
        const int32 inlineF = inlineGobbieCanUpgradeFlag(c.gobbieSize);
        const int32 pinGot  = pinGobbieCanUpgradeFlag(c.gobbieSize);
        // Positive formula pin (avoid De Morgan): below-cap -> 1, else -> 0.
        const int32 wantF   = c.gobbieSize < GobbieUpgradeCap ? 1 : 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantF, "formula free==gobbieSize<GobbieUpgradeCap ? 1 : 0") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(GobbieCanUpgradeFlag(0) == pinGobbieCanUpgradeFlag(0), "free==pin gobbieSize 0") && ok;
    ok = expect(GobbieCanUpgradeFlag(79) == pinGobbieCanUpgradeFlag(79), "free==pin gobbieSize 79") && ok;
    ok = expect(GobbieCanUpgradeFlag(80) == pinGobbieCanUpgradeFlag(80), "free==pin gobbieSize 80") && ok;
    ok = expect(GobbieCanUpgradeFlag(81) == pinGobbieCanUpgradeFlag(81), "free==pin gobbieSize 81") && ok;
    ok = expect(GobbieCanUpgradeFlag(100) == pinGobbieCanUpgradeFlag(100), "free==pin gobbieSize 100") && ok;
    ok = expect(GobbieCanUpgradeFlag(-1) == pinGobbieCanUpgradeFlag(-1), "free==pin negative") && ok;

    // Exact boundary neighbors via cap constant.
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap - 1) == 1, "cap-1 -> 1") && ok;
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap) == 0, "cap -> 0") && ok;
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap + 1) == 0, "cap+1 -> 0") && ok;

    // Dense size grid: free == inline == pin == formula.
    for (int32 size = -5; size <= 100; ++size)
    {
        const int32 got  = GobbieCanUpgradeFlag(size);
        const int32 want = size < GobbieUpgradeCap ? 1 : 0;
        ok = expect(got == want, "dense GobbieCanUpgradeFlag formula") && ok;
        ok = expect(got == inlineGobbieCanUpgradeFlag(size), "dense free==inline") && ok;
        ok = expect(got == pinGobbieCanUpgradeFlag(size), "dense free==pin") && ok;
    }

    // Production moogleOnUpdate option 2 expand-failure path semantics:
    // free -> gobbieCanUpgrade event param for updateEvent; host owns updateEvent.
    // Positive form: below-cap inventory may upgrade (flag 1); at/above-cap flag 0.
    ok = expect(GobbieCanUpgradeFlag(50) == 1, "moogleOnUpdate expand-fail below-cap -> flag 1") && ok;
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap) == 0, "moogleOnUpdate expand-fail at-cap -> flag 0") && ok;
    ok = expect(GobbieCanUpgradeFlag(100) == 0, "moogleOnUpdate expand-fail oversized -> flag 0") && ok;

    return ok;
}
