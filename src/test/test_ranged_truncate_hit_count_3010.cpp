#include "test_ranged_truncate_hit_count_3010.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldTruncateHitCountOnAmmoDeplete 3010 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack hitCount-truncate-on-ammo-deplete gate for dual-wire
// cross-check (slice 3010):
//   if !consumingThisShot → false
//   else → ammoQuantityBeforeConsume == shotIndexI
auto inlineShouldTruncateHitCountOnAmmoDeplete(
    const bool consumingThisShot,
    const uint8 ammoQuantityBeforeConsume,
    const uint8 shotIndexI) -> bool
{
    if (!consumingThisShot)
    {
        return false;
    }
    return ammoQuantityBeforeConsume == shotIndexI;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldTruncateHitCountOnAmmoDeplete
// (OnRangedAttack hitCount truncate on ammo deplete; slice 3010).
auto runRangedTruncateHitCount3010SelfTests() -> bool
{
    using rangedammohelpers::ShouldTruncateHitCountOnAmmoDeplete;

    bool ok = true;

    const struct
    {
        bool        consuming;
        uint8       ammoQuantityBeforeConsume;
        uint8       shotIndexI;
        bool        want;
        const char* label;
    } cases[] = {
        // Not consuming always skips truncate.
        { false, 3, 3, false, "not consuming equal qty/index → false" },
        { false, 5, 3, false, "not consuming qty>index → false" },
        { false, 1, 0, false, "not consuming qty>index zero-i → false" },
        { false, 0, 0, false, "not consuming zero poles → false" },

        // Consuming: equality truncates.
        { true, 3, 3, true, "qty == index 3 → truncate" },
        { true, 1, 1, true, "qty == index 1 → truncate" },
        { true, 0, 0, true, "qty == index 0 → truncate" },
        { true, 5, 5, true, "qty == index 5 → truncate" },
        { true, 255, 255, true, "qty == index uint8 max → truncate" },

        // Consuming: inequality continues.
        { true, 5, 3, false, "qty 5 > index 3 → no truncate" },
        { true, 3, 5, false, "qty 3 < index 5 → no truncate" },
        { true, 2, 1, false, "qty 2 > index 1 → no truncate" },
        { true, 1, 0, false, "qty 1 > index 0 → no truncate" },
        { true, 0, 1, false, "qty 0 < index 1 → no truncate" },
        { true, 4, 2, false, "qty 4 > index 2 → no truncate" },

        // Residual 1390 pins.
        { true, 3, 3, true, "residual truncate qty==i" },
        { true, 5, 3, false, "residual no truncate qty>i" },
        { false, 3, 3, false, "residual not consuming" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldTruncateHitCountOnAmmoDeplete(c.consuming, c.ammoQuantityBeforeConsume, c.shotIndexI);
        const bool inlineF = inlineShouldTruncateHitCountOnAmmoDeplete(c.consuming, c.ammoQuantityBeforeConsume, c.shotIndexI);
        const bool wantPin = c.consuming && (c.ammoQuantityBeforeConsume == c.shotIndexI);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldTruncateHitCountOnAmmoDeplete dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldTruncateHitCountOnAmmoDeplete == pin formula consuming && qty==i") && ok;
    }

    // Pin composition: not-consuming short-circuit and equality.
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(false, 3, 3), "not consuming must not truncate") && ok;
    ok = expect(ShouldTruncateHitCountOnAmmoDeplete(true, 3, 3), "qty == index must truncate") && ok;
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(true, 5, 3), "qty != index must not truncate") && ok;
    ok = expect(ShouldTruncateHitCountOnAmmoDeplete(true, 0, 0), "qty == index 0 must truncate") && ok;
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(true, 1, 0), "qty 1 != index 0 must not truncate") && ok;

    // Dense compose: consuming × qty poles × index poles.
    for (const bool consuming : { false, true })
    {
        for (const uint8 qty : { static_cast<uint8>(0), static_cast<uint8>(1), static_cast<uint8>(2),
                                 static_cast<uint8>(3), static_cast<uint8>(5), static_cast<uint8>(10),
                                 static_cast<uint8>(255) })
        {
            for (const uint8 i : { static_cast<uint8>(0), static_cast<uint8>(1), static_cast<uint8>(2),
                                   static_cast<uint8>(3), static_cast<uint8>(5), static_cast<uint8>(10),
                                   static_cast<uint8>(255) })
            {
                const bool got  = ShouldTruncateHitCountOnAmmoDeplete(consuming, qty, i);
                const bool want = consuming && (qty == i);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldTruncateHitCountOnAmmoDeplete(consuming, qty, i),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // consumingThisShot = true (literal; already inside ShouldConsumeAmmo true branch),
    // ammoQuantityBeforeConsume = static_cast<uint8>(PAmmo->getQuantity()),
    // shotIndexI = i (multi-shot loop index).
    // On true, host sets hitCount = i.
    const struct
    {
        bool        consuming;
        uint8       ammoQuantityBeforeConsume;
        uint8       shotIndexI;
        bool        wantTruncate;
        const char* label;
    } composeCases[] = {
        { false, 3, 3, false, "not consuming: skip truncate" },
        { true, 3, 3, true, "last ammo at shot i=3: truncate hitCount" },
        { true, 5, 3, false, "ammo remains after shot i=3: continue" },
        { true, 1, 0, false, "first shot with qty 1: qty!=i continue" },
        { true, 1, 1, true, "second shot (i=1) with qty 1: truncate" },
        { true, 0, 0, true, "zero qty at i=0: truncate" },
        { true, 10, 9, false, "qty 10 at i=9: continue" },
        { true, 10, 10, true, "qty 10 at i=10: truncate" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldTruncateHitCountOnAmmoDeplete(c.consuming, c.ammoQuantityBeforeConsume, c.shotIndexI);
        ok             = expect(got == c.wantTruncate, c.label) && ok;
        const bool wantPin = c.consuming && (c.ammoQuantityBeforeConsume == c.shotIndexI);
        ok                 = expect(got == wantPin, "compose free == pin formula consuming && qty==i") && ok;
        ok                 = expect(got == inlineShouldTruncateHitCountOnAmmoDeplete(c.consuming, c.ammoQuantityBeforeConsume, c.shotIndexI),
                    "compose free == inline") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3295) calls
    // ShouldTruncateHitCountOnAmmoDeplete(true, static_cast<uint8>(PAmmo->getQuantity()), i)
    // inside the ShouldConsumeAmmo true branch (consuming always true at call site).
    ok = expect(ShouldTruncateHitCountOnAmmoDeplete(true, 3, 3), "production inject true + qty==i must truncate") && ok;
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(true, 5, 3), "production inject true + qty!=i must not truncate") && ok;

    return ok;
}
