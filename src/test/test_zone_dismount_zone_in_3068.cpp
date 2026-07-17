#include "test_zone_dismount_zone_in_3068.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldDismountOnZoneIn 3068 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CharZoneIn mount dismount formula for dual-wire cross-check (slice 3068):
//   mounted && !canUseMount
auto inlineShouldDismountOnZoneIn(const bool mounted, const bool canUseMount) -> bool
{
    return mounted && !canUseMount;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldDismountOnZoneIn
// (isMounted && !CanUseMisc(MISC_MOUNT) on CharZoneIn; slice 3068).
// Dense 2² boolean space.
auto runZoneDismountZoneIn3068SelfTests() -> bool
{
    using zonehelpers::ShouldClearCostumeOnZoneIn;
    using zonehelpers::ShouldDismountOnZoneIn;

    bool ok = true;

    // Residual 2673 pins still hold under dual-wire.
    ok = expect(ShouldDismountOnZoneIn(true, false), "residual mounted + !canUseMount → dismount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(true, true), "residual mounted + canUseMount → no dismount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, false), "residual not mounted → no dismount") && ok;

    const struct
    {
        bool        mounted;
        bool        canUseMount;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2² boolean space.
        { true, false, true, "mounted + !canUseMount → dismount" },
        { true, true, false, "mounted + canUseMount → keep mount" },
        { false, false, false, "not mounted + !canUseMount → no dismount" },
        { false, true, false, "not mounted + canUseMount → no dismount" },

        // Residual 2673 pins.
        { true, false, true, "residual ShouldDismountOnZoneIn(true, false)" },
        { true, true, false, "residual ShouldDismountOnZoneIn(true, true)" },
        { false, false, false, "residual ShouldDismountOnZoneIn(false, false)" },

        // Identity / polarity repeats for dual-wire stability.
        { true, false, true, "identity dismount pole" },
        { true, true, false, "identity mount-allowed pole" },
        { false, false, false, "identity unmounted pole" },
        { false, true, false, "identity both-false-effect pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDismountOnZoneIn(c.mounted, c.canUseMount);
        const bool inlineF = inlineShouldDismountOnZoneIn(c.mounted, c.canUseMount);
        const bool wantPin = c.mounted && !c.canUseMount;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDismountOnZoneIn dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDismountOnZoneIn == pin formula mounted && !canUseMount") && ok;
    }

    // Pin composition: only (true, false) dismounts.
    ok = expect(ShouldDismountOnZoneIn(true, false), "mounted + !canUseMount must dismount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(true, true), "mounted + canUseMount must not dismount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, false), "not mounted + !canUseMount must not dismount") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, true), "not mounted + canUseMount must not dismount") && ok;

    // Dense compose: full 2² boolean space.
    for (const bool mounted : { false, true })
    {
        for (const bool canUse : { false, true })
        {
            const bool got  = ShouldDismountOnZoneIn(mounted, canUse);
            const bool want = mounted && !canUse;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldDismountOnZoneIn(mounted, canUse),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Explicit polarity: dismount requires mounted && !canUseMount.
    for (const bool mounted : { false, true })
    {
        for (const bool canUse : { false, true })
        {
            const bool got  = ShouldDismountOnZoneIn(mounted, canUse);
            const bool want = mounted && !canUse;
            ok              = expect(got == want, "polarity: dismount == mounted && !canUse") && ok;
            ok              = expect(!(got && (!mounted || canUse)),
                        "polarity: dismount only when mounted && !canUse") &&
                 ok;
            ok = expect(!(!got && mounted && !canUse),
                        "polarity: must dismount when mounted && !canUseMount") &&
                 ok;
        }
    }

    // Host-style inject poles: isMounted() + CanUseMisc(MISC_MOUNT) on CharZoneIn.
    for (const bool mounted : { false, true })
    {
        for (const bool canUse : { false, true })
        {
            const bool dismount = ShouldDismountOnZoneIn(mounted, canUse);
            const bool want     = mounted && !canUse;
            ok                  = expect(dismount == want, "host inject dual-wire identity") && ok;
            ok                  = expect(dismount == inlineShouldDismountOnZoneIn(mounted, canUse),
                        "host inject free == inline") &&
                 ok;
            ok = expect(!(dismount && !mounted), "never dismount when not mounted") && ok;
            ok = expect(!(dismount && canUse), "never dismount when zone allows mounts") && ok;
            ok = expect(!(!dismount && mounted && !canUse), "mounted + !canUse must dismount") && ok;
        }
    }

    // Production CharZoneIn path semantics:
    // mounted + !MISC_MOUNT → animation NONE + DelStatusEffectSilent(Mounted).
    ok = expect(ShouldDismountOnZoneIn(true, false), "CharZoneIn mounted + !canUseMount → dismount path") && ok;
    ok = expect(!ShouldDismountOnZoneIn(true, true), "CharZoneIn mounted + canUseMount → keep-mount path") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, false), "CharZoneIn not mounted → no-dismount path") && ok;
    ok = expect(!ShouldDismountOnZoneIn(false, true), "CharZoneIn not mounted + canUse → no-dismount path") && ok;

    // Sibling zone-in costume clear gate (3048) is orthogonal: runs just
    // after mount dismount; free dismount formula does not depend on costume.
    // ShouldClearCostumeOnZoneIn(hasCostume) = hasCostume.
    ok = expect(ShouldClearCostumeOnZoneIn(true), "residual costume clear when hasCostume") && ok;
    ok = expect(!ShouldClearCostumeOnZoneIn(false), "residual no costume clear when !hasCostume") && ok;
    for (const bool hasCostume : { false, true })
    {
        for (const bool mounted : { false, true })
        {
            for (const bool canUse : { false, true })
            {
                // Orthogonal: costume clear iff hasCostume; dismount iff
                // mounted && !canUse.
                ok = expect(ShouldClearCostumeOnZoneIn(hasCostume) == hasCostume,
                            "dismount vs costume compose hasCostume") &&
                     ok;
                const bool wantDismount = mounted && !canUse;
                ok                      = expect(ShouldDismountOnZoneIn(mounted, canUse) == wantDismount,
                            "dismount vs costume compose dismount") &&
                     ok;
            }
        }
    }

    // Explicit dual-wire poles: free == pin formula for full 2².
    for (const bool mounted : { false, true })
    {
        for (const bool canUse : { false, true })
        {
            const bool got  = ShouldDismountOnZoneIn(mounted, canUse);
            const bool want = mounted && !canUse;
            ok              = expect(got == want, "host inject dual-wire pin poles") && ok;
            ok              = expect(got == inlineShouldDismountOnZoneIn(mounted, canUse),
                        "host inject free == inline poles") &&
                 ok;
        }
    }

    return ok;
}
