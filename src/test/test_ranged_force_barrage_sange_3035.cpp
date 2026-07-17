#include "test_ranged_force_barrage_sange_3035.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldForceBarrageSangeHitResolution 3035 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack Barrage/Sange force-hit resolution gate for dual-wire
// cross-check (slice 3035):
//   return hitOccured && !resolutionIsHit && (isBarrage || isSange)
auto inlineShouldForceBarrageSangeHitResolution(
    const bool hitOccured,
    const bool resolutionIsHit,
    const bool isBarrage,
    const bool isSange) -> bool
{
    return hitOccured && !resolutionIsHit && (isBarrage || isSange);
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldForceBarrageSangeHitResolution
// (OnRangedAttack Barrage/Sange force-hit resolution gate; slice 3035).
auto runRangedForceBarrageSange3035SelfTests() -> bool
{
    using rangedammohelpers::ShouldForceBarrageSangeHitResolution;

    bool ok = true;

    const struct
    {
        bool        hitOccured;
        bool        resolutionIsHit;
        bool        isBarrage;
        bool        isSange;
        bool        want;
        const char* label;
    } cases[] = {
        // Force when hit + non-Hit resolution + Barrage and/or Sange.
        { true, false, true, false, true, "hit non-Hit Barrage → force Hit" },
        { true, false, false, true, true, "hit non-Hit Sange → force Hit" },
        { true, false, true, true, true, "hit non-Hit Barrage+Sange → force Hit" },

        // Already Hit: never force (resolution already correct).
        { true, true, true, false, false, "hit already Hit Barrage → skip" },
        { true, true, false, true, false, "hit already Hit Sange → skip" },
        { true, true, true, true, false, "hit already Hit both → skip" },
        { true, true, false, false, false, "hit already Hit neither → skip" },

        // No hit: never force.
        { false, false, true, false, false, "miss non-Hit Barrage → skip" },
        { false, false, false, true, false, "miss non-Hit Sange → skip" },
        { false, false, true, true, false, "miss non-Hit both → skip" },
        { false, true, true, false, false, "miss already Hit Barrage → skip" },
        { false, false, false, false, false, "miss non-Hit neither → skip" },

        // Hit + non-Hit but neither Barrage nor Sange: skip.
        { true, false, false, false, false, "hit non-Hit neither → skip" },

        // Residual 1390 pins.
        { true, false, true, false, true, "residual force hit" },
        { true, true, true, false, false, "residual already hit" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldForceBarrageSangeHitResolution(c.hitOccured, c.resolutionIsHit, c.isBarrage, c.isSange);
        const bool inlineF = inlineShouldForceBarrageSangeHitResolution(c.hitOccured, c.resolutionIsHit, c.isBarrage, c.isSange);
        const bool wantPin = c.hitOccured && !c.resolutionIsHit && (c.isBarrage || c.isSange);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldForceBarrageSangeHitResolution dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldForceBarrageSangeHitResolution == pin formula hit && !resHit && (barrage||sange)") && ok;
    }

    // Pin composition: force / skip poles.
    ok = expect(ShouldForceBarrageSangeHitResolution(true, false, true, false), "hit non-Hit Barrage must force Hit resolution") && ok;
    ok = expect(ShouldForceBarrageSangeHitResolution(true, false, false, true), "hit non-Hit Sange must force Hit resolution") && ok;
    ok = expect(!ShouldForceBarrageSangeHitResolution(true, true, true, false), "already Hit must not re-force") && ok;
    ok = expect(!ShouldForceBarrageSangeHitResolution(false, false, true, false), "no hit must not force") && ok;
    ok = expect(!ShouldForceBarrageSangeHitResolution(true, false, false, false), "neither Barrage nor Sange must not force") && ok;

    // Dense compose: hitOccured × resolutionIsHit × isBarrage × isSange poles.
    for (const bool hitOccured : { false, true })
    {
        for (const bool resolutionIsHit : { false, true })
        {
            for (const bool isBarrage : { false, true })
            {
                for (const bool isSange : { false, true })
                {
                    const bool got  = ShouldForceBarrageSangeHitResolution(hitOccured, resolutionIsHit, isBarrage, isSange);
                    const bool want = hitOccured && !resolutionIsHit && (isBarrage || isSange);
                    ok              = expect(got == want, "compose free == pin formula") && ok;
                    ok              = expect(got == inlineShouldForceBarrageSangeHitResolution(hitOccured, resolutionIsHit, isBarrage, isSange),
                                "compose free == inline") &&
                         ok;
                }
            }
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // hitOccured = true (literal on successful-hit path ~3343),
    // resolutionIsHit = actionResult.resolution == ActionResolution::Hit,
    // isBarrage / isSange captured at shot start via HasStatusEffect.
    // On true, host sets actionResult.resolution = ActionResolution::Hit.
    const struct
    {
        bool        hitOccured;
        bool        resolutionIsHit;
        bool        isBarrage;
        bool        isSange;
        bool        wantForce;
        const char* label;
    } composeCases[] = {
        // Production inject: hitOccured always true on this path.
        { true, false, true, false, true, "prod hit non-Hit Barrage: force Hit" },
        { true, false, false, true, true, "prod hit non-Hit Sange: force Hit" },
        { true, false, true, true, true, "prod hit non-Hit both: force Hit" },
        { true, true, true, false, false, "prod hit already Hit Barrage: skip" },
        { true, true, false, true, false, "prod hit already Hit Sange: skip" },
        { true, false, false, false, false, "prod hit non-Hit neither: skip" },
        // Pure-function poles not exercised by production literal true.
        { false, false, true, false, false, "no hit Barrage: skip" },
        { false, false, false, true, false, "no hit Sange: skip" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldForceBarrageSangeHitResolution(c.hitOccured, c.resolutionIsHit, c.isBarrage, c.isSange);
        ok             = expect(got == c.wantForce, c.label) && ok;
        ok             = expect(got == (c.hitOccured && !c.resolutionIsHit && (c.isBarrage || c.isSange)),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineShouldForceBarrageSangeHitResolution(c.hitOccured, c.resolutionIsHit, c.isBarrage, c.isSange),
                    "compose free == inline") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3343) passes true for hitOccured
    // and resolution == ActionResolution::Hit for resolutionIsHit.
    ok = expect(ShouldForceBarrageSangeHitResolution(true, false, true, false), "production inject Barrage non-Hit must force") && ok;
    ok = expect(ShouldForceBarrageSangeHitResolution(true, false, false, true), "production inject Sange non-Hit must force") && ok;
    ok = expect(!ShouldForceBarrageSangeHitResolution(true, true, true, true), "production inject already Hit must skip") && ok;

    return ok;
}
