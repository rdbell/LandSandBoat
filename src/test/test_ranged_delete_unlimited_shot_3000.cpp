#include "test_ranged_delete_unlimited_shot_3000.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldDeleteUnlimitedShot 3000 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack UnlimitedShot delete gate for dual-wire cross-check (slice 3000):
//   if !hasUnlimitedShot → false
//   else → hitOccured || retainUnlimitedShotMod <= 0
auto inlineShouldDeleteUnlimitedShot(
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
// (OnRangedAttack UnlimitedShot status-delete gate; slice 3000).
auto runRangedDeleteUnlimitedShot3000SelfTests() -> bool
{
    using rangedammohelpers::ShouldDeleteUnlimitedShot;

    bool ok = true;

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

        // Residual 1390 pins.
        { true, true, 5, true, "residual del hit" },
        { true, false, 0, true, "residual del retain 0" },
        { true, false, 1, false, "residual retain" },
        { false, true, 0, false, "residual no us" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool inlineF = inlineShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        const bool wantPin = c.hasUnlimitedShot && (c.hitOccured || c.retainUnlimitedShotMod <= 0);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteUnlimitedShot dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteUnlimitedShot == pin formula hasUS && (hit || retain<=0)") && ok;
    }

    // Pin composition: no-US short-circuit and hit/retain poles.
    ok = expect(!ShouldDeleteUnlimitedShot(false, true, 0), "no UnlimitedShot must not delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5), "hit with UnlimitedShot must delete") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 0), "miss with retain 0 must delete") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(true, false, 1), "miss with retain 1 must keep") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, -1), "miss with retain -1 must delete") && ok;

    // Dense compose: hasUnlimitedShot × hitOccured × retain poles.
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
                ok              = expect(got == inlineShouldDeleteUnlimitedShot(hasUS, hit, retain),
                            "compose free == inline") &&
                     ok;
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
        const bool got = ShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod);
        ok             = expect(got == c.wantDelete, c.label) && ok;
        ok             = expect(got == (c.hasUnlimitedShot && (c.hitOccured || c.retainUnlimitedShotMod <= 0)),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineShouldDeleteUnlimitedShot(c.hasUnlimitedShot, c.hitOccured, c.retainUnlimitedShotMod),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
