#include "test_ranged_consume_ammo_2986.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldConsumeAmmo 2986 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack ammo-consume gate for dual-wire cross-check (slice 2986):
//   if !hasAmmo → false
//   else → roll0to99 > recycleChance
auto inlineShouldConsumeAmmo(const bool hasAmmo, const int16 recycleChance, const int roll0to99) -> bool
{
    if (!hasAmmo)
    {
        return false;
    }
    return roll0to99 > recycleChance;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldConsumeAmmo
// (OnRangedAttack ammo consume gate; slice 2986).
auto runRangedConsumeAmmo2986SelfTests() -> bool
{
    using rangedammohelpers::ShouldConsumeAmmo;

    bool ok = true;

    const struct
    {
        bool        hasAmmo;
        int16       recycleChance;
        int         roll0to99;
        bool        want;
        const char* label;
    } cases[] = {
        // No ammo always skips consume.
        { false, 0, 99, false, "no ammo → false" },
        { false, 0, 0, false, "no ammo roll 0 → false" },
        { false, 50, 99, false, "no ammo high recycle → false" },
        { false, 100, 0, false, "no ammo unlimited recycle → false" },

        // Strict > at recycle boundary.
        { true, 25, 25, false, "roll == recycle → false (strict >)" },
        { true, 25, 26, true, "roll > recycle → true" },
        { true, 25, 24, false, "roll < recycle → false" },

        // recycle 0 edges (residual 1390 pins).
        { true, 0, 0, false, "recycle 0 roll 0: 0 > 0 is false" },
        { true, 0, 1, true, "recycle 0 roll 1 → consume" },
        { true, 0, 99, true, "recycle 0 roll 99 → consume" },

        // recycle 99 edges (roll in [0,100)).
        { true, 99, 99, false, "recycle 99 roll 99: 99 > 99 is false" },
        { true, 99, 98, false, "recycle 99 roll 98 → false" },
        { true, 99, 0, false, "recycle 99 roll 0 → false" },

        // recycle 100-ish (UnlimitedShot path forces 100).
        { true, 100, 0, false, "recycle 100 roll 0 → never consume" },
        { true, 100, 99, false, "recycle 100 roll 99 → never consume" },
        { true, 100, 100, false, "recycle 100 roll 100: 100 > 100 is false" },

        // roll domain poles with mid recycle.
        { true, 50, 0, false, "roll 0 mid recycle → false" },
        { true, 50, 99, true, "roll 99 mid recycle → true" },
        { true, 50, 50, false, "roll == 50 recycle → false" },
        { true, 50, 51, true, "roll 51 > 50 → true" },

        // Residual 1390 pins.
        { true, 25, 26, true, "residual consume edge roll>" },
        { true, 25, 25, false, "residual consume edge roll==" },
        { false, 0, 99, false, "residual no ammo" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldConsumeAmmo(c.hasAmmo, c.recycleChance, c.roll0to99);
        const bool inlineF = inlineShouldConsumeAmmo(c.hasAmmo, c.recycleChance, c.roll0to99);
        const bool wantPin = c.hasAmmo && (c.roll0to99 > c.recycleChance);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldConsumeAmmo dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldConsumeAmmo == pin formula hasAmmo && roll > recycle") && ok;
    }

    // Pin composition: no-ammo short-circuit and strict inequality.
    ok = expect(!ShouldConsumeAmmo(false, 0, 99), "no ammo must not consume") && ok;
    ok = expect(!ShouldConsumeAmmo(true, 25, 25), "roll == recycle must not consume (strict >)") && ok;
    ok = expect(ShouldConsumeAmmo(true, 25, 26), "roll > recycle must consume") && ok;
    ok = expect(!ShouldConsumeAmmo(true, 0, 0), "0 > 0 is false") && ok;
    ok = expect(ShouldConsumeAmmo(true, 0, 1), "roll 1 with recycle 0 must consume") && ok;

    // Dense compose: hasAmmo × recycle poles × roll poles.
    for (const bool hasAmmo : { false, true })
    {
        for (const int16 recycle : { static_cast<int16>(0), static_cast<int16>(25), static_cast<int16>(50),
                                     static_cast<int16>(99), static_cast<int16>(100) })
        {
            for (const int roll : { 0, 1, 25, 26, 50, 51, 98, 99 })
            {
                const bool got  = ShouldConsumeAmmo(hasAmmo, recycle, roll);
                const bool want = hasAmmo && (roll > recycle);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldConsumeAmmo(hasAmmo, recycle, roll),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // hasAmmo = (PAmmo != nullptr),
    // recycleChance = ApplyUnlimitedShotToRecycleChance(ResolveRecycleChance(...), hasUnlimitedShot),
    // roll0to99 = xirand::GetRandomNumber(100) in [0, 100).
    const struct
    {
        bool        hasAmmo;
        int16       recycleChance;
        int         roll0to99;
        bool        wantConsume;
        const char* label;
    } composeCases[] = {
        { false, 0, 50, false, "null PAmmo: skip consume" },
        { true, 0, 0, false, "ammo present recycle 0 roll 0: no consume" },
        { true, 0, 50, true, "ammo present recycle 0 roll 50: consume" },
        { true, 25, 25, false, "roll == recycle: recycle succeeds, no consume" },
        { true, 25, 26, true, "roll > recycle: consume" },
        { true, 100, 99, false, "UnlimitedShot recycle 100: never consume in [0,100)" },
        { true, 99, 99, false, "max recycle 99 roll 99: no consume" },
        { true, 99, 0, false, "max recycle 99 roll 0: no consume" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldConsumeAmmo(c.hasAmmo, c.recycleChance, c.roll0to99);
        ok             = expect(got == c.wantConsume, c.label) && ok;
        ok             = expect(got == (c.hasAmmo && (c.roll0to99 > c.recycleChance)),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineShouldConsumeAmmo(c.hasAmmo, c.recycleChance, c.roll0to99),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
