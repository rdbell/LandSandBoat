#include "test_voidwalker_should_upgrade_ki_3563.h"

#include "map/voidwalker_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldUpgradeKI 3563 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua checkUpgrade roll gate for dual-wire cross-check
// (dedicated expand residual 3563):
//   rand == 5  // math.random(1, 10) == 5
auto inlineShouldUpgradeKI3563(const int32 roll) -> bool
{
    return roll == 5;
}

// Compact dual-wire pin matching Go pinShouldUpgradeKI3563 / C++ capacity
// UpgradeRollSuccess form (formula unchanged from 0987 / 2884 / 3173 / 3377 /
// 3433 / 3482 / 3527):
//   roll == UpgradeRollSuccess  // == 5
auto pinShouldUpgradeKI3563(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Prior dedicated 3173 pin form (residual pin under 3563).
auto pinShouldUpgradeKI3173Residual(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Prior dedicated 3377 pin form (residual pin under 3563).
auto pinShouldUpgradeKI3377Residual(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Prior dedicated 3433 pin form (residual pin under 3563).
auto pinShouldUpgradeKI3433Residual(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Prior dedicated 3482 pin form (residual pin under 3563).
auto pinShouldUpgradeKI3482Residual(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Prior dedicated 3527 pin form (residual pin under 3563).
auto pinShouldUpgradeKI3527Residual(const int32 roll) -> bool
{
    using voidwalkerhelpers::UpgradeRollSuccess;
    return roll == UpgradeRollSuccess;
}

// Residual 2884 inline form (residual pin under 3563).
auto inlineShouldUpgradeKI2884Residual(const int32 roll) -> bool
{
    return roll == 5;
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldUpgradeKI
// (Lua checkUpgrade math.random(1,10)==5 gate;
// OmegaXI internal/voidwalker; dedicated expand residual 3563 of residual
// 2884; prior dedicated 3527 / 3482 / 3433 / 3377 / 3173 / pure 0987).
//
// Coverage:
//   - free == inline == pin == pin3527
//   - positive pin: return roll == UpgradeRollSuccess
//   - UpgradeRollSuccess == 5 pin
//   - residual 2884 / 0987 / prior dedicated 3173 / 3377 / 3433 / 3482 / 3527 pins still hold
//   - poles across inject range 1..10 (success only on 5) + neighbors
auto runVoidwalkerShouldUpgradeKI3563SelfTests() -> bool
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

    // Prior dedicated 3173 free==inline==pin residual pin still holds.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3173Residual(UpgradeRollSuccess),
                "prior 3173 residual pin: free == pin3173 for UpgradeRollSuccess") &&
         ok;
    // Prior dedicated 3377 free==inline==pin residual pin still holds.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3377Residual(UpgradeRollSuccess),
                "prior 3377 residual pin: free == pin3377 for UpgradeRollSuccess") &&
         ok;
    // Prior dedicated 3433 free==inline==pin residual pin still holds.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3433Residual(UpgradeRollSuccess),
                "prior 3433 residual pin: free == pin3433 for UpgradeRollSuccess") &&
         ok;
    // Prior dedicated 3482 free==inline==pin residual pin still holds.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3482Residual(UpgradeRollSuccess),
                "prior 3482 residual pin: free == pin3482 for UpgradeRollSuccess") &&
         ok;
    // Prior dedicated 3527 free==inline==pin residual pin still holds.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3527Residual(UpgradeRollSuccess),
                "prior 3527 residual pin: free == pin3527 for UpgradeRollSuccess") &&
         ok;

    // --- Core poles: free == inline == pin == pin3527 ---
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
        const bool inlineF = inlineShouldUpgradeKI3563(c.roll);
        const bool pin     = pinShouldUpgradeKI3563(c.roll);
        // Positive form pin composition (explicit roll == UpgradeRollSuccess).
        const bool wantPin = c.roll == UpgradeRollSuccess;
        // Residual free == prior dedicated 3173 / 3377 / 3433 / 3482 / 3527 pin forms.
        const bool pin3173 = pinShouldUpgradeKI3173Residual(c.roll);
        const bool pin3377 = pinShouldUpgradeKI3377Residual(c.roll);
        const bool pin3433 = pinShouldUpgradeKI3433Residual(c.roll);
        const bool pin3482 = pinShouldUpgradeKI3482Residual(c.roll);
        const bool pin3527 = pinShouldUpgradeKI3527Residual(c.roll);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin && got == pin3527,
                    "ShouldUpgradeKI free == inline == pin == pin3527") &&
             ok;
        ok = expect(got == pin3173, "ShouldUpgradeKI free == prior 3173 pin residual") && ok;
        ok = expect(got == pin3377, "ShouldUpgradeKI free == prior 3377 pin residual") && ok;
        ok = expect(got == pin3433, "ShouldUpgradeKI free == prior 3433 pin residual") && ok;
        ok = expect(got == pin3482, "ShouldUpgradeKI free == prior 3482 pin residual") && ok;
    }

    // Free == pin across residual poles + UpgradeRollSuccess composition.
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess), "UpgradeRollSuccess should upgrade") && ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollSuccess) == pinShouldUpgradeKI3563(UpgradeRollSuccess),
                "free == pin for UpgradeRollSuccess") &&
         ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess - 1), "UpgradeRollSuccess-1 should not upgrade") && ok;
    ok = expect(!ShouldUpgradeKI(UpgradeRollSuccess + 1), "UpgradeRollSuccess+1 should not upgrade") && ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollMin) == pinShouldUpgradeKI3563(UpgradeRollMin),
                "free == pin for UpgradeRollMin") &&
         ok;
    ok = expect(ShouldUpgradeKI(UpgradeRollMax) == pinShouldUpgradeKI3563(UpgradeRollMax),
                "free == pin for UpgradeRollMax") &&
         ok;

    // Dense compose across inject range poles 1..10 and neighbors:
    // free == inline == pin == pin3527 (+ residual 2884 inline + prior pin forms).
    for (int32 roll = UpgradeRollMin - 2; roll <= UpgradeRollMax + 2; ++roll)
    {
        const bool got     = ShouldUpgradeKI(roll);
        const bool inlineF = inlineShouldUpgradeKI3563(roll);
        const bool pin     = pinShouldUpgradeKI3563(roll);
        const bool pin3527 = pinShouldUpgradeKI3527Residual(roll);
        const bool want    = roll == UpgradeRollSuccess;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3527,
                                    "compose free == inline == pin == pin3527") &&
             ok;
        ok = expect(got == inlineShouldUpgradeKI2884Residual(roll),
                    "compose free == residual 2884 inline") &&
             ok;
        ok = expect(got == pinShouldUpgradeKI3377Residual(roll),
                    "compose free == prior 3377 pin residual") &&
             ok;
        ok = expect(got == pinShouldUpgradeKI3433Residual(roll),
                    "compose free == prior 3433 pin residual") &&
             ok;
        ok = expect(got == pinShouldUpgradeKI3482Residual(roll),
                    "compose free == prior 3482 pin residual") &&
             ok;
    }

    // --- Production checkUpgrade path semantics ---
    // Host injects:
    //   roll = math.random(1, 10)
    // Poles 1..10: success only on 5.
    // when true  → delKeyItem / addKeyItem / messageSpecial
    // when false → no upgrade this kill
    for (int32 roll = UpgradeRollMin; roll <= UpgradeRollMax; ++roll)
    {
        const bool got     = ShouldUpgradeKI(roll);
        const bool inlineF = inlineShouldUpgradeKI3563(roll);
        const bool pin     = pinShouldUpgradeKI3563(roll);
        const bool pin3527 = pinShouldUpgradeKI3527Residual(roll);
        const bool want    = roll == 5;
        ok                 = expect(got == want, "host inject poles 1..10 success only on 5") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3527,
                                    "host inject free == inline == pin == pin3527") &&
             ok;
    }

    return ok;
}
