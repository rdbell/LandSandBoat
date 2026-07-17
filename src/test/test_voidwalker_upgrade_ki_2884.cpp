#include "test_voidwalker_upgrade_ki_2884.h"

#include "map/voidwalker_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldUpgradeKI 2884 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua checkUpgrade roll gate for dual-wire cross-check:
//   rand == 5  // math.random(1, 10) == 5
auto inlineShouldUpgradeKI(const int32 roll) -> bool
{
    return roll == 5;
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldUpgradeKI
// (Lua checkUpgrade math.random(1,10)==5 gate; slice 2884).
auto runVoidwalkerUpgradeKI2884SelfTests() -> bool
{
    using voidwalkerhelpers::ShouldUpgradeKI;
    using voidwalkerhelpers::UpgradeRollMax;
    using voidwalkerhelpers::UpgradeRollMin;
    using voidwalkerhelpers::UpgradeRollSuccess;

    bool ok = true;

    ok = expect(UpgradeRollMin == 1, "UpgradeRollMin pin 1") && ok;
    ok = expect(UpgradeRollMax == 10, "UpgradeRollMax pin 10") && ok;
    ok = expect(UpgradeRollSuccess == 5, "UpgradeRollSuccess pin 5") && ok;

    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        { 5, true, "success roll 5 upgrades" },
        { 1, false, "min roll does not upgrade" },
        { 2, false, "roll 2 does not upgrade" },
        { 3, false, "roll 3 does not upgrade" },
        { 4, false, "roll 4 does not upgrade" },
        { 6, false, "roll 6 does not upgrade" },
        { 7, false, "roll 7 does not upgrade" },
        { 8, false, "roll 8 does not upgrade" },
        { 9, false, "roll 9 does not upgrade" },
        { 10, false, "max roll does not upgrade" },
        { 0, false, "below range does not upgrade" },
        { -1, false, "negative roll does not upgrade" },
        { 11, false, "above range does not upgrade" },
        { 55, false, "large non-5 does not upgrade" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpgradeKI(c.roll);
        const bool inlineF = inlineShouldUpgradeKI(c.roll);
        const bool wantPin = c.roll == UpgradeRollSuccess;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldUpgradeKI dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "ShouldUpgradeKI == roll == UpgradeRollSuccess") && ok;
    }

    // Pin composition: free function uses UpgradeRollSuccess constant.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess), "UpgradeRollSuccess should upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess - 1), "UpgradeRollSuccess-1 should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess + 1), "UpgradeRollSuccess+1 should not upgrade") && ok;

    // Dense compose across the inject range and neighbors.
    for (int32 roll = UpgradeRollMin - 2; roll <= UpgradeRollMax + 2; ++roll)
    {
        const bool got  = ShouldUpgradeKI(roll);
        const bool want = roll == UpgradeRollSuccess;
        ok = expect(got == want, "compose range free == roll==UpgradeRollSuccess") && ok;
        ok = expect(got == inlineShouldUpgradeKI(roll), "compose range free == inline") && ok;
    }

    return ok;
}
