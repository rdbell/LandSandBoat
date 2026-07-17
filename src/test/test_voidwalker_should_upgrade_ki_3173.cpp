#include "test_voidwalker_should_upgrade_ki_3173.h"

#include "map/voidwalker_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldUpgradeKI 3173 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua checkUpgrade roll gate for dual-wire cross-check
// (dedicated 3173):
//   rand == 5  // math.random(1, 10) == 5
auto inlineShouldUpgradeKI3173(const int32 roll) -> bool
{
    return roll == 5;
}

// Compact dual-wire pin matching Go pinShouldUpgradeKI3173 / C++ capacity
// UpgradeRollSuccess form (formula unchanged from 0987 / 2884):
//   roll == UpgradeRollSuccess  // == 5
auto pinShouldUpgradeKI3173(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldUpgradeKI
// (Lua checkUpgrade math.random(1,10)==5 gate;
// OmegaXI internal/voidwalker; dedicated slice 3173; residual expand 2884 / pure 0987).
//
// Coverage:
//   - free == inline == pin
//   - UpgradeRollSuccess == 5 pin
//   - residual 2884 / 0987 pins still hold
//   - poles across inject range 1..10 + neighbors
auto runVoidwalkerShouldUpgradeKI3173SelfTests() -> bool
{
    using voidwalkerhelpers::ShouldUpgradeKI;
    using voidwalkerhelpers::UpgradeRollMax;
    using voidwalkerhelpers::UpgradeRollMin;
    using voidwalkerhelpers::UpgradeRollSuccess;

    bool ok = true;

    // UpgradeRollSuccess == 5 pin (and inject range).
    ok = expect(UpgradeRollMin == 1, "UpgradeRollMin pin 1") && ok;
    ok = expect(UpgradeRollMax == 10, "UpgradeRollMax pin 10") && ok;
    ok = expect(UpgradeRollSuccess == 5, "UpgradeRollSuccess pin 5") && ok;

    // Residual 0987 / 2884 pins still hold under dedicated dual-wire.
    ok = expect(ShouldUpgradeKI(5), "residual 0987: roll 5 should upgrade") && ok;
    for (const int32 r : { 1, 2, 3, 4, 6, 7, 8, 9, 10 })
    {
        ok = expect(!ShouldUpgradeKI(r), "residual 0987: non-5 inject range should not upgrade") && ok;
    }
    // Residual 2884 out-of-range poles.
    ok = expect(!ShouldUpgradeKI(0), "residual 2884: below range should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(-1), "residual 2884: negative roll should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(11), "residual 2884: above range should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(55), "residual 2884: large non-5 should not upgrade") && ok;

    // --- Core poles: free == inline == pin ---
    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        { 5, true, "success roll 5 upgrades" },
        { 1, false, "min roll 1 does not upgrade" },
        { 2, false, "roll 2 does not upgrade" },
        { 3, false, "roll 3 does not upgrade" },
        { 4, false, "roll 4 does not upgrade" },
        { 6, false, "roll 6 does not upgrade" },
        { 7, false, "roll 7 does not upgrade" },
        { 8, false, "roll 8 does not upgrade" },
        { 9, false, "roll 9 does not upgrade" },
        { 10, false, "max roll 10 does not upgrade" },
        { 0, false, "below range does not upgrade" },
        { -1, false, "negative roll does not upgrade" },
        { 11, false, "above range does not upgrade" },
        { 55, false, "large non-5 does not upgrade" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpgradeKI(c.roll);
        const bool inlineF = inlineShouldUpgradeKI3173(c.roll);
        const bool pin     = pinShouldUpgradeKI3173(c.roll);
        // Positive form pin composition (explicit roll == UpgradeRollSuccess).
        const bool wantPin = c.roll == UpgradeRollSuccess;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldUpgradeKI free == inline == pin") &&
             ok;
    }

    // Free == pin across residual poles + UpgradeRollSuccess composition.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess), "UpgradeRollSuccess should upgrade") && ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3173(UpgradeRollSuccess),
                "free == pin for UpgradeRollSuccess") &&
         ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess - 1), "UpgradeRollSuccess-1 should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess + 1), "UpgradeRollSuccess+1 should not upgrade") && ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollMin) == pinShouldUpgradeKI3173(UpgradeRollMin),
                "free == pin for UpgradeRollMin") &&
         ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollMax) == pinShouldUpgradeKI3173(UpgradeRollMax),
                "free == pin for UpgradeRollMax") &&
         ok;

    // Dense compose across inject range poles 1..10 and neighbors:
    // free == inline == pin.
    for (int32 roll = UpgradeRollMin - 2; roll <= UpgradeRollMax + 2; ++roll)
    {
        const bool got     = ShouldUpgradeKI(roll);
        const bool inlineF = inlineShouldUpgradeKI3173(roll);
        const bool pin     = pinShouldUpgradeKI3173(roll);
        const bool want    = roll == UpgradeRollSuccess;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // --- Production checkUpgrade path semantics ---
    // Host injects:
    //   roll = math.random(1, 10)
    // when true  → delKeyItem / addKeyItem / messageSpecial
    // when false → no upgrade this kill
    for (int32 roll = UpgradeRollMin; roll <= UpgradeRollMax; ++roll)
    {
        const bool got     = ShouldUpgradeKI(roll);
        const bool inlineF = inlineShouldUpgradeKI3173(roll);
        const bool pin     = pinShouldUpgradeKI3173(roll);
        const bool want    = roll == 5;
        ok                 = expect(got == want, "host inject poles 1..10") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
