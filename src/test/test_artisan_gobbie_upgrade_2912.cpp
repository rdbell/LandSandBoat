#include "test_artisan_gobbie_upgrade_2912.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan GobbieCanUpgradeFlag 2912 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 2 pure failure param for dual-wire cross-check:
//   gobbieSize < 80 and 1 or 0
auto inlineGobbieCanUpgradeFlag(const int32 gobbieSize) -> int32
{
    return gobbieSize < 80 ? 1 : 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::GobbieCanUpgradeFlag
// (artisan.lua moogleOnUpdate option 2 expand-failure event param).
auto runArtisanGobbieUpgrade2912SelfTests() -> bool
{
    using artisanhelpers::GobbieCanUpgradeFlag;
    using artisanhelpers::GobbieUpgradeCap;

    bool ok = true;

    // Constant pin: 80 from artisan.lua gobbieSize < 80.
    ok = expect(GobbieUpgradeCap == 80, "GobbieUpgradeCap == 80") && ok;

    const struct
    {
        int32       gobbieSize;
        int32       want;
        const char* label;
    } cases[] = {
        // Residual 0948 pins.
        { 79, 1, "residual 79 -> 1" },
        { 80, 0, "residual 80 -> 0" },
        { 0, 1, "residual 0 -> 1" },
        // Boundary matrix.
        { 79, 1, "one below cap" },
        { 80, 0, "exact cap" },
        { 81, 0, "one above cap" },
        { 1, 1, "min positive" },
        { 30, 1, "buy sack size" },
        { 50, 1, "mid inventory" },
        { 100, 0, "oversized" },
        { -1, 1, "negative always below cap" },
        { -100, 1, "large negative" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = GobbieCanUpgradeFlag(c.gobbieSize);
        const int32 inlineF = inlineGobbieCanUpgradeFlag(c.gobbieSize);
        const int32 wantF   = c.gobbieSize < GobbieUpgradeCap ? 1 : 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline gobbieSize<80") && ok;
        ok = expect(got == wantF, "dual-wire free == formula") && ok;
    }

    // Residual 0948 explicit pins.
    ok = expect(GobbieCanUpgradeFlag(79) == 1, "residual GobbieCanUpgradeFlag(79)") && ok;
    ok = expect(GobbieCanUpgradeFlag(80) == 0, "residual GobbieCanUpgradeFlag(80)") && ok;
    ok = expect(GobbieCanUpgradeFlag(0) == 1, "residual GobbieCanUpgradeFlag(0)") && ok;

    // Exact boundary neighbors.
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap - 1) == 1, "cap-1 -> 1") && ok;
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap) == 0, "cap -> 0") && ok;
    ok = expect(GobbieCanUpgradeFlag(GobbieUpgradeCap + 1) == 0, "cap+1 -> 0") && ok;

    // Dense size grid: free function matches formula everywhere.
    for (int32 size = -5; size <= 100; ++size)
    {
        const int32 got  = GobbieCanUpgradeFlag(size);
        const int32 want = size < GobbieUpgradeCap ? 1 : 0;
        ok = expect(got == want, "dense GobbieCanUpgradeFlag formula") && ok;
        ok = expect(got == inlineGobbieCanUpgradeFlag(size), "dense dual-wire == inline") && ok;
    }

    return ok;
}
