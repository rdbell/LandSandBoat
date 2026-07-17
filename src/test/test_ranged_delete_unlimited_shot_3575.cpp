#include "test_ranged_delete_unlimited_shot_3575.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldDeleteUnlimitedShot 3575 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack UnlimitedShot delete gate for dual-wire cross-check
// (slice 3575 dedicated expand residual 3000; prior 3500 retained):
//   if !hasUnlimitedShot → false
//   else → hitOccured || retainUnlimitedShotMod <= 0
auto inlineShouldDeleteUnlimitedShot3575(
    const bool hasUnlimitedShot,
    const bool hitOccured,
    const int16 retainUnlimitedShotMod) -> bool
{
    if (!hasUnlimitedShot)
    {
        return false;
    }
    return hitOccured || retainUnlimitedShotMod <= 0;
}

// Dual-wire pin matching production free function / capacity body
// (dedicated slice 3575). Early-return form only — same formula.
//   if !hasUnlimitedShot → false
//   else → hitOccured || retainUnlimitedShotMod <= 0
auto pinShouldDeleteUnlimitedShot3575(
    const bool hasUnlimitedShot,
    const bool hitOccured,
    const int16 retainUnlimitedShotMod) -> bool
{
    if (!hasUnlimitedShot)
    {
        return false;
    }
    return hitOccured || retainUnlimitedShotMod <= 0;
}

// Prior 3500 pin retained for independence cross-check (same formula).
// free == inline == pin == pin3500.
auto pinShouldDeleteUnlimitedShot3500(
    const bool hasUnlimitedShot,
    const bool hitOccured,
    const int16 retainUnlimitedShotMod) -> bool
{
    if (!hasUnlimitedShot)
    {
        return false;
    }
    return hitOccured || retainUnlimitedShotMod <= 0;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldDeleteUnlimitedShot
// (OnRangedAttack UnlimitedShot status-delete gate; OmegaXI internal/attackutils;
// slice 3575 dedicated expand residual 3000; prior dedicated 3500 retained).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3500 (early-return formula)
//   - residual poles: no US; hit always delete; miss retain <=0 / >0; residual 1390/3000/3399/3451/3500
//   - dense hasUS × hit × retain compose
//   - prior 3500 independence (suite retained)
//   - residual dual-wire 3000 suite retained (test_ranged_delete_unlimited_shot_3000)
//   - prior dedicated dual-wire 3399 suite retained (test_ranged_delete_unlimited_shot_3399)
//   - prior dedicated dual-wire 3451 suite retained (test_ranged_delete_unlimited_shot_3451)
//   - prior dedicated dual-wire 3500 suite retained (test_ranged_delete_unlimited_shot_3500)
// Not registered in CMake/main (parent registers if/when LSB self-tests land).
auto runRangedDeleteUnlimitedShot3575SelfTests() -> bool
{
    using rangedammohelpers::ShouldDeleteUnlimitedShot;

    bool ok = true;

    // Residual 1390 / residual dual-wire 3000 / dedicated 3399 / 3451 / 3500 pins still hold under dual-wire.
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5), "residual: hit with UnlimitedShot must delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 0), "residual: miss with retain 0 must delete") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(true, false, 1), "residual: miss with retain 1 must keep") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(false, true, 0), "residual: no UnlimitedShot must not delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, -1), "residual: miss with retain -1 must delete") && ok;

    const struct
    {
        bool        hasUnlimitedShot;
        bool        hitOccured;
        int16       retainUnlimitedShotMod;
        bool        want;
        const char* label;
    } cases[] = {
        // No UnlimitedShot: never delete.
        { false, false, 0, false, "no US miss retain 0 → false" },
        { false, true, 0, false, "no US hit retain 0 → false" },
        { false, true, 5, false, "no US hit retain 5 → false" },
        { false, false, 1, false, "no US miss retain 1 → false" },
        { false, false, -1, false, "no US miss retain -1 → false" },

        // UnlimitedShot present + hit: always delete regardless of retain.
        { true, true, 5, true, "US hit retain 5 → delete" },
        { true, true, 1, true, "US hit retain 1 → delete" },
        { true, true, 0, true, "US hit retain 0 → delete" },
        { true, true, -1, true, "US hit retain -1 → delete" },
        { true, true, 100, true, "US hit retain 100 → delete" },

        // UnlimitedShot present + miss: delete only when retain <= 0.
        { true, false, 0, true, "US miss retain 0 → delete" },
        { true, false, -1, true, "US miss retain -1 → delete" },
        { true, false, -5, true, "US miss retain -5 → delete" },
        { true, false, 1, false, "US miss retain 1 → keep" },
        { true, false, 5, false, "US miss retain 5 → keep" },
        { true, false, 100, false, "US miss retain 100 → keep" },

        // Residual 1390 / 3000 / 3399 / 3451 / 3500 re-pins.
        { true, true, 5, true, "residual del hit" },
        { true, false, 0, true, "residual del retain 0" },
        { true, false, 1, false, "residual retain" },
        { false, true, 0, false, "residual no us" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool inlineF = inlineShouldDeleteUnlimitedShot3575(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool pinF    = pinShouldDeleteUnlimitedShot3575(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool pin3500 = pinShouldDeleteUnlimitedShot3500(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool wantPin = c.hasUnlimitedShot && (c.hitOccured || c.retainUnlimitedShotMod <= 0);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteUnlimitedShot dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldDeleteUnlimitedShot dual-wire == pin formula") && ok;
        ok = expect(got == pin3500, "ShouldDeleteUnlimitedShot dual-wire == pin3500 formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteUnlimitedShot == pin formula hasUS && (hit || retain<=0)") && ok;
        ok = expect(inlineF == pinF, "inline == pin") && ok;
        ok = expect(pinF == pin3500, "pin == pin3500") && ok;
    }

    // Pin composition: no-US short-circuit and hit/retain poles.
    ok = expect(!ShouldDeleteUnlimitedShot(false, true, 0), "no UnlimitedShot must not delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5), "hit with UnlimitedShot must delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 0), "miss with retain 0 must delete") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(true, false, 1), "miss with retain 1 must keep") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, -1), "miss with retain -1 must delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(false, true, 0) == pinShouldDeleteUnlimitedShot3575(false, true, 0),
                "free == pin residual no US") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5) == pinShouldDeleteUnlimitedShot3575(true, true, 5),
                "free == pin residual hit delete") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 1) == pinShouldDeleteUnlimitedShot3575(true, false, 1),
                "free == pin residual retain keep") &&
         ok;

    // Prior 3500 independence: free == prior 3500 pin poles (suite retained).
    // free == inline == pin == pin3500.
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5) == pinShouldDeleteUnlimitedShot3500(true, true, 5),
                "prior 3500 independence: free == 3500 pin del hit") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(false, true, 0) == pinShouldDeleteUnlimitedShot3500(false, true, 0),
                "prior 3500 independence: free == 3500 pin no us") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 1) == pinShouldDeleteUnlimitedShot3500(true, false, 1),
                "prior 3500 independence: free == 3500 pin retain keep") &&
         ok;
    ok = expect(pinShouldDeleteUnlimitedShot3575(true, false, 0) == pinShouldDeleteUnlimitedShot3500(true, false, 0),
                "prior 3500 independence: 3575 pin == 3500 pin miss retain 0") &&
         ok;
    ok = expect(inlineShouldDeleteUnlimitedShot3575(true, true, 5) == pinShouldDeleteUnlimitedShot3500(true, true, 5),
                "prior 3500 independence: 3575 inline == 3500 pin del hit") &&
         ok;

    // Dense compose: hasUnlimitedShot × hitOccured × retain poles — free == inline == pin == pin3500.
    for (const bool hasUS : { false, true })
    {
        for (const bool hit : { false, true })
        {
            for (const int16 retain : { static_cast<int16>(-5), static_cast<int16>(-1), static_cast<int16>(0),
                                        static_cast<int16>(1), static_cast<int16>(5), static_cast<int16>(100) })
            {
                const bool got  = ShouldDeleteUnlimitedShot(hasUS, hit, retain);
                const bool want = hasUS && (hit || retain <= 0);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldDeleteUnlimitedShot3575(hasUS, hit, retain),
                            "compose free == inline") &&
                     ok;
                ok = expect(got == pinShouldDeleteUnlimitedShot3575(hasUS, hit, retain), "compose free == pin") && ok;
                ok = expect(got == pinShouldDeleteUnlimitedShot3500(hasUS, hit, retain), "compose free == pin3500") && ok;
            }
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // hasUnlimitedShot = StatusEffectContainer->HasStatusEffect(UnlimitedShot),
    // hitOccured = any ranged shot hit this action,
    // retainUnlimitedShotMod = getMod(Mod::RETAIN_UNLIMITED_SHOT).
    const struct
    {
        bool        hasUnlimitedShot;
        bool        hitOccured;
        int16       retainUnlimitedShotMod;
        bool        wantDelete;
        const char* label;
    } composeCases[] = {
        { false, false, 0, false, "no US miss: skip delete" },
        { false, true, 0, false, "no US hit: skip delete" },
        { true, true, 5, true, "US hit: always delete" },
        { true, true, 0, true, "US hit retain 0: delete" },
        { true, false, 0, true, "US miss retain 0: delete" },
        { true, false, 1, false, "US miss retain 1: keep (RETAIN_UNLIMITED_SHOT)" },
        { true, false, -1, true, "US miss retain -1: delete" },
        { true, false, 100, false, "US miss high retain: keep" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = ShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool inlineF = inlineShouldDeleteUnlimitedShot3575(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool pinF    = pinShouldDeleteUnlimitedShot3575(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool pin3500 = pinShouldDeleteUnlimitedShot3500(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        ok                 = expect(got == c.wantDelete, c.label) && ok;
        ok                 = expect(got == (c.hasUnlimitedShot && (c.hitOccured || c.retainUnlimitedShotMod <= 0)),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineF, "compose free == inline") && ok;
        ok = expect(got == pinF, "compose free == pin") && ok;
        ok = expect(got == pin3500, "compose free == pin3500") && ok;
    }

    // Residual independence: free == pin residual poles.
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5) == pinShouldDeleteUnlimitedShot3575(true, true, 5),
                "residual free == pin del hit") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(false, true, 0) == pinShouldDeleteUnlimitedShot3575(false, true, 0),
                "residual free == pin no us") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 1) == pinShouldDeleteUnlimitedShot3575(true, false, 1),
                "residual free == pin retain keep") &&
         ok;

    // Prior 3500 independence under compose: free == 3500 pin residual poles.
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5) == pinShouldDeleteUnlimitedShot3500(true, true, 5),
                "prior 3500 independence compose: free == 3500 pin del hit") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(false, true, 0) == pinShouldDeleteUnlimitedShot3500(false, true, 0),
                "prior 3500 independence compose: free == 3500 pin no us") &&
         ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 1) == pinShouldDeleteUnlimitedShot3500(true, false, 1),
                "prior 3500 independence compose: free == 3500 pin retain keep") &&
         ok;

    return ok;
}
