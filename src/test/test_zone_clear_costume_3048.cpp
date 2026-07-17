#include "test_zone_clear_costume_3048.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldClearCostumeOnZoneIn 3048 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CharZoneIn Costume clear formula for dual-wire cross-check (slice 3048):
//   hasCostume
auto inlineShouldClearCostumeOnZoneIn(const bool hasCostume) -> bool
{
    return hasCostume;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldClearCostumeOnZoneIn
// (HasStatusEffect(Costume) on CharZoneIn; slice 3048). Dense 2¹ boolean space.
auto runZoneClearCostume3048SelfTests() -> bool
{
    using zonehelpers::ShouldClearCostumeOnZoneIn;
    using zonehelpers::ShouldDismountOnZoneIn;

    bool ok = true;

    // Residual 2682 pins still hold under dual-wire.
    ok = expect(ShouldClearCostumeOnZoneIn(true), "residual has costume → clear") && ok;
    ok = expect(!ShouldClearCostumeOnZoneIn(false), "residual no costume → no clear") && ok;

    const struct
    {
        bool        hasCostume;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "has costume clears on zone-in" },
        { false, false, "no costume does not clear" },

        // Residual 2682 pins.
        { true, true, "residual ShouldClearCostumeOnZoneIn(true)" },
        { false, false, "residual ShouldClearCostumeOnZoneIn(false)" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity true" },
        { false, false, "identity false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearCostumeOnZoneIn(c.hasCostume);
        const bool inlineF = inlineShouldClearCostumeOnZoneIn(c.hasCostume);
        const bool wantPin = c.hasCostume;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearCostumeOnZoneIn dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearCostumeOnZoneIn == pin formula hasCostume") && ok;
    }

    // Pin composition: identity with hasCostume only.
    ok = expect(ShouldClearCostumeOnZoneIn(true), "hasCostume true must clear") && ok;
    ok = expect(!ShouldClearCostumeOnZoneIn(false), "hasCostume false must not clear") && ok;

    // Dense compose: full 2¹ boolean space.
    for (const bool hasCostume : { false, true })
    {
        const bool got  = ShouldClearCostumeOnZoneIn(hasCostume);
        const bool want = hasCostume;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearCostumeOnZoneIn(hasCostume),
                    "compose free == inline") &&
             ok;
    }

    // Explicit polarity: clear iff hasCostume.
    for (const bool has : { false, true })
    {
        const bool got = ShouldClearCostumeOnZoneIn(has);
        ok             = expect(got == has, "polarity: clear == hasCostume") && ok;
        ok             = expect(!(got && !has), "polarity: must not clear when hasCostume false") && ok;
        ok             = expect(!(!got && has), "polarity: must clear when hasCostume true") && ok;
    }

    // Host-style inject poles: HasStatusEffect(Costume) as bool on CharZoneIn.
    for (const bool hasCostume : { false, true })
    {
        const bool clear = ShouldClearCostumeOnZoneIn(hasCostume);
        ok               = expect(clear == hasCostume, "host inject dual-wire identity") && ok;
        ok               = expect(clear == inlineShouldClearCostumeOnZoneIn(hasCostume),
                    "host inject free == inline") &&
             ok;
        ok = expect(!(clear && !hasCostume), "clear only when hasCostume") && ok;
        ok = expect(!(!clear && hasCostume), "hasCostume must clear") && ok;
    }

    // Production CharZoneIn path semantics:
    // hasCostume → DelStatusEffectSilent(Costume); else no clear.
    ok = expect(ShouldClearCostumeOnZoneIn(true), "CharZoneIn has Costume → clear path") && ok;
    ok = expect(!ShouldClearCostumeOnZoneIn(false), "CharZoneIn no Costume → no-clear path") && ok;

    // Sibling zone-in dismount gate (residual 2673) is orthogonal: runs just
    // before costume clear; free costume identity does not depend on mount.
    // ShouldDismountOnZoneIn(mounted, canUseMount) = mounted && !canUseMount.
    ok = expect(ShouldDismountOnZoneIn(true, false), "residual dismount when mounted && !canUseMount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, false), "residual no dismount when not mounted") && ok;
    for (const bool hasCostume : { false, true })
    {
        for (const bool mounted : { false, true })
        {
            for (const bool canUse : { false, true })
            {
                // Orthogonal: costume clear iff hasCostume; dismount iff
                // mounted && !canUse.
                ok = expect(ShouldClearCostumeOnZoneIn(hasCostume) == hasCostume,
                            "costume vs mount compose hasCostume") &&
                     ok;
                const bool wantDismount = mounted && !canUse;
                ok                      = expect(ShouldDismountOnZoneIn(mounted, canUse) == wantDismount,
                            "costume vs mount compose dismount") &&
                     ok;
            }
        }
    }

    // Explicit dual-wire poles: free == identity for both bools.
    for (const bool hasCostume : { false, true })
    {
        const bool got = ShouldClearCostumeOnZoneIn(hasCostume);
        ok             = expect(got == hasCostume, "host inject dual-wire identity poles") && ok;
        ok             = expect(got == inlineShouldClearCostumeOnZoneIn(hasCostume),
                    "host inject free == inline poles") &&
             ok;
    }

    return ok;
}
