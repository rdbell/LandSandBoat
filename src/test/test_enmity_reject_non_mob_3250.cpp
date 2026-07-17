#include "test_enmity_reject_non_mob_3250.h"

#include "map/enmity_container.h"
#include "map/enmity_reject_non_mob_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldRejectNonMobHolder 3250 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity non-mob holder early-return gate for dual-wire cross-check
// (slice 3250 dedicated expand residual 2956; prior dedicated 3182):
//   !holderIsMob
// Direct return form matching production free function / capacity.
auto inlineShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

// Compact dual-wire pin matching free function / capacity body (slice 3250).
// Direct return only — same formula as production ShouldRejectNonMobHolder.
// Simple identity-not (negation of the host-injected holderIsMob bool).
auto pinShouldRejectNonMobHolder3250(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

// Compact dual-wire pin matching prior dedicated 3182 residual pin.
auto pinShouldRejectNonMobHolder3182(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

} // namespace

// Pure dual-wire expansion for enmityrejecthelpers::ShouldRejectNonMobHolder
// (holder objtype != TYPE_MOB early return; OmegaXI internal/enmity;
// slice 3250 dedicated expand residual 2956; prior dedicated 3182).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return !holderIsMob identity-not)
//   - residual poles: non-mob reject / mob accept
//   - dense full 2^1 boolean domain
//   - residual 1357 / 2956 / prior dedicated 3182 pins still hold
auto runEnmityRejectNonMob3250SelfTests() -> bool
{
    using enmityrejecthelpers::ShouldRejectNonMobHolder;

    bool ok = true;

    // Residual 1357 / 2956 / prior dedicated 3182 pins still hold under dual-wire.
    ok = expect(ShouldRejectNonMobHolder(false), "residual !holderIsMob must reject") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "residual holderIsMob must accept") && ok;

    // Residual poles: free == inline == pin (direct return).
    const struct
    {
        bool        holderIsMob;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2956 classic dual poles.
        { false, true, "residual non-mob holder rejected" },
        { true, false, "residual mob holder accepted" },

        // Residual 2956 re-pins.
        { false, true, "residual 2956 reject non-mob" },
        { true, false, "residual 2956 accept mob" },

        // Residual 1357 re-pins.
        { false, true, "residual 1357 reject non-mob" },
        { true, false, "residual 1357 accept mob" },

        // Prior dedicated 3182 poles.
        { false, true, "prior 3182 non-mob rejected" },
        { true, false, "prior 3182 mob accepted" },
        { false, true, "prior 3182 residual 2956 reject non-mob" },
        { true, false, "prior 3182 residual 2956 accept mob" },

        // Host inject path poles (UpdateEnmity non-mob holder gate).
        { true, false, "host TYPE_MOB holder: accept (continue UpdateEnmity)" },
        { false, true, "host non-MOB holder (pet/trust/PC): reject early return" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldRejectNonMobHolder(p.holderIsMob);
        const bool inlineF = inlineShouldRejectNonMobHolder(p.holderIsMob);
        const bool pin     = pinShouldRejectNonMobHolder3250(p.holderIsMob);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNonMobHolder dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldRejectNonMobHolder == pin formula") && ok;
        ok = expect(got == pinShouldRejectNonMobHolder3182(p.holderIsMob),
                    "ShouldRejectNonMobHolder == prior dedicated pin") &&
             ok;
        ok = expect(got == !p.holderIsMob, "ShouldRejectNonMobHolder == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on holderIsMob only.
    ok = expect(ShouldRejectNonMobHolder(false), "!holderIsMob must reject") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "holderIsMob must accept") && ok;
    ok = expect(ShouldRejectNonMobHolder(false) == pinShouldRejectNonMobHolder3250(false), "free==pin false") && ok;
    ok = expect(ShouldRejectNonMobHolder(true) == pinShouldRejectNonMobHolder3250(true), "free==pin true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool holderIsMob : { false, true })
    {
        const bool got     = ShouldRejectNonMobHolder(holderIsMob);
        const bool inlineF = inlineShouldRejectNonMobHolder(holderIsMob);
        const bool pin     = pinShouldRejectNonMobHolder3250(holderIsMob);
        const bool want    = !holderIsMob;
        ok                 = expect(got == want, "dense free == pin formula") && ok;
        ok                 = expect(got == inlineF, "dense free == inline") && ok;
        ok                 = expect(got == pin, "dense free == pin") && ok;
        ok                 = expect(got == pinShouldRejectNonMobHolder3182(holderIsMob), "dense free == residual pin") && ok;
    }

    // Production enmitymath residual form matches capacity dual-wire.
    for (const bool holderIsMob : { false, true })
    {
        ok = expect(enmitymath::ShouldRejectNonMobHolder(holderIsMob) ==
                        ShouldRejectNonMobHolder(holderIsMob),
                    "capacity dual-wire == enmitymath residual") &&
             ok;
    }

    // Host-style compose poles: holder object-type injects.
    // holderIsMob = (objtype == TYPE_MOB); only true for mob holders.
    const struct
    {
        bool        holderIsMob;
        bool        wantReject;
        const char* label;
    } composeCases[] = {
        { true, false, "TYPE_MOB holder: accept (continue UpdateEnmity)" },
        { false, true, "non-MOB holder (pet/trust/PC): reject early return" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldRejectNonMobHolder(c.holderIsMob);
        ok             = expect(got == c.wantReject, c.label) && ok;
        ok             = expect(got == !c.holderIsMob, "compose free == !holderIsMob pin") && ok;
        ok             = expect(got == inlineShouldRejectNonMobHolder(c.holderIsMob), "compose free == inline") && ok;
        ok             = expect(got == pinShouldRejectNonMobHolder3250(c.holderIsMob), "compose free == pin") && ok;
    }

    // Residual 2956 / prior dedicated 3182 / pure 1357 still hold under expand suite.
    ok = expect(ShouldRejectNonMobHolder(false) && !ShouldRejectNonMobHolder(true),
                "residual 2956/3182/1357 pins failed under 3250 suite") &&
         ok;

    return ok;
}
