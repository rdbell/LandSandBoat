#include "test_zone_skip_level_restriction_3042.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldSkipLevelRestrictionUpdate 3042 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline updateCharLevelRestriction skip formula for dual-wire
// cross-check (slice 3042):
//   if (!hasRestriction) return false;
//   return statusNull || powerMatches;
auto inlineShouldSkipLevelRestrictionUpdate(
    const bool hasRestriction,
    const bool statusNull,
    const bool powerMatches) -> bool
{
    if (!hasRestriction)
    {
        return false;
    }
    return statusNull || powerMatches;
}

auto pinShouldSkipLevelRestrictionUpdate(
    const bool hasRestriction,
    const bool statusNull,
    const bool powerMatches) -> bool
{
    return hasRestriction && (statusNull || powerMatches);
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldSkipLevelRestrictionUpdate
// (!hasRestriction → false; else statusNull || powerMatches on
// updateCharLevelRestriction; slice 3042). Dense 2³ boolean space.
auto runZoneSkipLevelRestriction3042SelfTests() -> bool
{
    using zonehelpers::ShouldApplyZoneLevelRestriction;
    using zonehelpers::ShouldDeleteExistingLevelRestriction;
    using zonehelpers::ShouldSkipLevelRestrictionUpdate;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, false, true), "residual has + power match → skip") && ok;
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, true, false), "residual has + null status → skip") && ok;
    ok = expect(!ShouldSkipLevelRestrictionUpdate(false, false, false), "residual no restriction → no skip") && ok;

    const struct
    {
        bool        hasRestriction;
        bool        statusNull;
        bool        powerMatches;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2³ boolean space.
        { false, false, false, false, "no has, no null, no power → no skip" },
        { false, false, true, false, "no has, power only → no skip" },
        { false, true, false, false, "no has, null only → no skip" },
        { false, true, true, false, "no has, null+power → no skip" },
        { true, false, false, false, "has, neither null nor power → no skip" },
        { true, false, true, true, "has + power match → skip" },
        { true, true, false, true, "has + null status → skip" },
        { true, true, true, true, "has + null + power → skip" },

        // Residual 1363 pins.
        { true, false, true, true, "residual skip same" },
        { true, true, false, true, "residual skip null status" },
        { false, false, false, false, "residual no skip" },

        // Additional poles for dual-wire stability.
        { true, false, false, false, "has mismatch power → continue to delete" },
        { false, true, true, false, "status flags ignored without has" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipLevelRestrictionUpdate(c.hasRestriction, c.statusNull, c.powerMatches);
        const bool inlineF = inlineShouldSkipLevelRestrictionUpdate(c.hasRestriction, c.statusNull, c.powerMatches);
        const bool wantPin = pinShouldSkipLevelRestrictionUpdate(c.hasRestriction, c.statusNull, c.powerMatches);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipLevelRestrictionUpdate dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSkipLevelRestrictionUpdate == pin formula has&&(null||power)") && ok;
    }

    // Pin composition: skip iff hasRestriction && (statusNull || powerMatches).
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, false, true), "has + powerMatches must skip") && ok;
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, true, false), "has + statusNull must skip") && ok;
    ok = expect(!ShouldSkipLevelRestrictionUpdate(true, false, false), "has without null/power must not skip") && ok;
    ok = expect(!ShouldSkipLevelRestrictionUpdate(false, true, true), "!hasRestriction must not skip even with null+power") && ok;

    // Explicit polarity over dense 2³ inject space.
    for (const bool has : { false, true })
    {
        for (const bool null : { false, true })
        {
            for (const bool power : { false, true })
            {
                const bool got  = ShouldSkipLevelRestrictionUpdate(has, null, power);
                const bool want = has && (null || power);
                ok              = expect(got == want, "polarity: skip == has&&(null||power)") && ok;
                ok              = expect(!(got && !has), "polarity: never skip when !hasRestriction") && ok;
                ok              = expect(!(got && has && !null && !power), "polarity: never skip when has but neither null nor power") && ok;
                ok              = expect(!(!got && has && (null || power)), "polarity: always skip when has && (null||power)") && ok;
            }
        }
    }

    // Host-style inject poles: has=true after HasStatusEffect; statusNull and
    // powerMatches from GetStatusEffect / GetPower vs m_levelRestriction.
    for (const bool null : { false, true })
    {
        for (const bool power : { false, true })
        {
            // Production path only reaches the gate when hasRestriction is true.
            const bool skip = ShouldSkipLevelRestrictionUpdate(true, null, power);
            const bool want = null || power;
            ok              = expect(skip == want, "host inject dual-wire identity") && ok;
            ok              = expect(skip == inlineShouldSkipLevelRestrictionUpdate(true, null, power),
                        "host inject free == inline") &&
                 ok;
            // skip when null status quirk or same power as zone.
            ok = expect(!(skip && !null && !power), "never skip when status present and power mismatches") && ok;
            ok = expect(!(!skip && (null || power)), "null or power match must skip") && ok;
        }
    }

    // Production updateCharLevelRestriction path semantics (when hasRestriction):
    // statusNull||powerMatches → return early (no delete/apply);
    // else → may ShouldDeleteExistingLevelRestriction then apply (3032).
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, false, true), "updateCharLevelRestriction same power → skip path") && ok;
    ok = expect(ShouldSkipLevelRestrictionUpdate(true, true, false), "updateCharLevelRestriction null status → skip path") && ok;
    ok = expect(!ShouldSkipLevelRestrictionUpdate(true, false, false), "updateCharLevelRestriction mismatch → continue path") && ok;
    ok = expect(!ShouldSkipLevelRestrictionUpdate(false, false, false), "updateCharLevelRestriction no has → no skip gate branch") && ok;

    // Sibling residual delete gate still composes with skip polarity.
    ok = expect(ShouldDeleteExistingLevelRestriction(true, false), "residual: has + !skip → delete") && ok;
    ok = expect(!ShouldDeleteExistingLevelRestriction(true, true), "residual: has + skip → no delete") && ok;
    // Sibling 3032 apply residual still holds under dual-wire package surface.
    ok = expect(ShouldApplyZoneLevelRestriction(50), "sibling 3032 apply residual non-zero") && ok;
    ok = expect(!ShouldApplyZoneLevelRestriction(0), "sibling 3032 apply residual zero") && ok;

    // Dense edge compose: free == inline == pin for full 2³.
    for (const bool has : { false, true })
    {
        for (const bool null : { false, true })
        {
            for (const bool power : { false, true })
            {
                const bool got  = ShouldSkipLevelRestrictionUpdate(has, null, power);
                const bool want = pinShouldSkipLevelRestrictionUpdate(has, null, power);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldSkipLevelRestrictionUpdate(has, null, power),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
