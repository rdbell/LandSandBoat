#include "test_zone_delete_level_restriction_3043.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldDeleteExistingLevelRestriction 3043 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline updateCharLevelRestriction delete formula for dual-wire
// cross-check (slice 3043):
//   hasRestriction && !shouldSkip
auto inlineShouldDeleteExistingLevelRestriction(
    const bool hasRestriction,
    const bool shouldSkip) -> bool
{
    return hasRestriction && !shouldSkip;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldDeleteExistingLevelRestriction
// (hasRestriction && !shouldSkip on updateCharLevelRestriction; slice 3043).
// Dense 2² boolean space. Composes with ShouldSkipLevelRestrictionUpdate (3042).
auto runZoneDeleteLevelRestriction3043SelfTests() -> bool
{
    using zonehelpers::ShouldDeleteExistingLevelRestriction;
    using zonehelpers::ShouldSkipLevelRestrictionUpdate;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldDeleteExistingLevelRestriction(true, false), "residual has + not skip → delete") && ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(true, true), "residual has + skip → no delete") && ok;

    const struct
    {
        bool        hasRestriction;
        bool        shouldSkip;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2² boolean space.
        { false, false, false, "no restriction + not skip → no delete" },
        { false, true, false, "no restriction + skip → no delete" },
        { true, false, true, "has restriction + not skip → delete" },
        { true, true, false, "has restriction + skip → no delete" },

        // Residual 1363 pins.
        { true, false, true, "residual delete existing" },
        { true, true, false, "residual no delete skip" },

        // Identity / polarity repeats for dual-wire stability.
        { true, false, true, "identity delete pole" },
        { false, false, false, "identity no-has pole" },
        { true, true, false, "identity skip pole" },
        { false, true, false, "identity both-false-delete pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDeleteExistingLevelRestriction(c.hasRestriction, c.shouldSkip);
        const bool inlineF = inlineShouldDeleteExistingLevelRestriction(c.hasRestriction, c.shouldSkip);
        const bool wantPin = c.hasRestriction && !c.shouldSkip;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDeleteExistingLevelRestriction dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDeleteExistingLevelRestriction == pin formula has && !skip") && ok;
    }

    // Pin composition: only (true, false) deletes.
    ok = expect(ShouldDeleteExistingLevelRestriction(true, false), "has + not skip must delete") && ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(false, false), "no has + not skip must not delete") && ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(true, true), "has + skip must not delete") && ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(false, true), "no has + skip must not delete") && ok;

    // Explicit polarity: delete requires hasRestriction && !shouldSkip.
    for (const bool has : { false, true })
    {
        for (const bool skip : { false, true })
        {
            const bool got  = ShouldDeleteExistingLevelRestriction(has, skip);
            const bool want = has && !skip;
            ok              = expect(got == want, "polarity: delete == (has && !skip)") && ok;
            ok              = expect(!(got && (!has || skip)), "polarity: delete only when has && !skip") && ok;
            ok              = expect(!(!got && has && !skip), "polarity: must delete when has && !skip") && ok;
        }
    }

    // Host-style compose with skip gate (sibling 3042):
    // shouldSkip := ShouldSkipLevelRestrictionUpdate(has, statusNull, powerMatches)
    // if shouldSkip → return early (no delete / no apply)
    // if ShouldDeleteExistingLevelRestriction(has, shouldSkip) → DelStatusEffect
    // Production after skip early-return injects (true, false).
    for (const bool has : { false, true })
    {
        for (const bool skip : { false, true })
        {
            const bool del  = ShouldDeleteExistingLevelRestriction(has, skip);
            const bool want = has && !skip;
            ok              = expect(del == want, "host inject dual-wire identity") && ok;
            ok              = expect(del == inlineShouldDeleteExistingLevelRestriction(has, skip),
                        "host inject free == inline") &&
                 ok;
            // Delete and skip are exclusive when has is true.
            ok = expect(!(has && del && skip), "delete and skip must not both fire when has") && ok;
            ok = expect(!(has && !del && !skip), "has && !skip must delete") && ok;
        }
    }

    // Production updateCharLevelRestriction path semantics (after skip gate):
    // production injects (true, false) inside hasRestriction branch after skip
    // return → always delete when that branch is reached.
    ok = expect(ShouldDeleteExistingLevelRestriction(true, false),
                "updateCharLevelRestriction production inject (true,false) → delete path") &&
         ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(true, true),
                "updateCharLevelRestriction skip → no-delete path") &&
         ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(false, false),
                "updateCharLevelRestriction no has → no-delete path") &&
         ok;

    // Compose with skip free function (sibling 3042 surface):
    // when skip is true, delete must be false for the same hasRestriction.
    for (const bool has : { false, true })
    {
        for (const bool statusNull : { false, true })
        {
            for (const bool powerMatches : { false, true })
            {
                const bool skip = ShouldSkipLevelRestrictionUpdate(has, statusNull, powerMatches);
                const bool del  = ShouldDeleteExistingLevelRestriction(has, skip);
                // Identity: delete == has && !skip for any skip-gate inputs.
                ok = expect(del == (has && !skip), "skip-compose free identity") && ok;
                // When skip fires with has, no delete; when has and not skip, delete.
                ok = expect(!(has && skip && del), "skip-compose: skip true must suppress delete") && ok;
                ok = expect(!(has && !skip && !del), "skip-compose: has && !skip must delete") && ok;
                ok = expect(!(!has && del), "skip-compose: no has never deletes") && ok;
            }
        }
    }

    // Dense compose: full 2² free == inline == pin.
    for (const bool has : { false, true })
    {
        for (const bool skip : { false, true })
        {
            const bool got  = ShouldDeleteExistingLevelRestriction(has, skip);
            const bool want = has && !skip;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldDeleteExistingLevelRestriction(has, skip),
                        "compose free == inline") &&
                 ok;
        }
    }

    return ok;
}
