#include "test_enmity_reject_non_mob_holder_3182.h"

#include "map/enmity_container.h"
#include "map/enmity_reject_non_mob_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldRejectNonMobHolder 3182 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity non-mob holder early-return gate for dual-wire cross-check
// (residual 2956 / dedicated 3182):
//   !holderIsMob
auto inlineShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

// Compact dual-wire pin matching Go pinShouldRejectNonMobHolder3182:
//   !holderIsMob
// Simple identity-not (negation of the host-injected holderIsMob bool).
auto pinShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

} // namespace

// Pure dual-wire expansion for enmityrejecthelpers::ShouldRejectNonMobHolder
// (holder objtype != TYPE_MOB early return; OmegaXI internal/enmity; slice 3182).
//
// Coverage:
//   - holderIsMob false → reject / true → accept
//   - free == inline == pin == !holderIsMob (identity-not)
//   - residual 1357 / 2956 pins still hold
auto runEnmityRejectNonMobHolder3182SelfTests() -> bool
{
    using enmityrejecthelpers::ShouldRejectNonMobHolder;

    bool ok = true;

    // Residual 1357 / 2956 pins still hold under dual-wire.
    ok = expect(ShouldRejectNonMobHolder(false), "residual !holderIsMob must reject") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "residual holderIsMob must accept") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldRejectNonMobHolder(false), "non-mob holder rejected") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "mob holder accepted") && ok;

    const struct
    {
        bool        holderIsMob;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "non-mob holder rejected" },
        { true, false, "mob holder accepted" },

        // Residual 2956 re-pins.
        { false, true, "residual 2956 reject non-mob" },
        { true, false, "residual 2956 accept mob" },

        // Residual 1357 re-pins.
        { false, true, "residual 1357 reject non-mob" },
        { true, false, "residual 1357 accept mob" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNonMobHolder(c.holderIsMob);
        const bool inlineF = inlineShouldRejectNonMobHolder(c.holderIsMob);
        const bool pinGot  = pinShouldRejectNonMobHolder(c.holderIsMob);
        const bool wantPin = !c.holderIsMob;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNonMobHolder dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldRejectNonMobHolder dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldRejectNonMobHolder == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on holderIsMob only.
    ok = expect(ShouldRejectNonMobHolder(false), "!holderIsMob must reject") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "holderIsMob must accept") && ok;
    ok = expect(ShouldRejectNonMobHolder(false) == pinShouldRejectNonMobHolder(false), "free==pin false") && ok;
    ok = expect(ShouldRejectNonMobHolder(true) == pinShouldRejectNonMobHolder(true), "free==pin true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool holderIsMob : { false, true })
    {
        const bool got  = ShouldRejectNonMobHolder(holderIsMob);
        const bool want = !holderIsMob;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNonMobHolder(holderIsMob), "compose free == inline") && ok;
        ok              = expect(got == pinShouldRejectNonMobHolder(holderIsMob), "compose free == pin") && ok;
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
        ok             = expect(got == pinShouldRejectNonMobHolder(c.holderIsMob), "compose free == pin") && ok;
    }

    // Residual 2956 / 1357 still hold under dedicated suite.
    ok = expect(ShouldRejectNonMobHolder(false) && !ShouldRejectNonMobHolder(true),
                "residual 2956/1357 pins failed under 3182 suite") &&
         ok;

    return ok;
}
