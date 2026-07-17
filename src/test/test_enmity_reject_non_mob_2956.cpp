#include "test_enmity_reject_non_mob_2956.h"

#include "map/enmity_container.h"
#include "map/enmity_reject_non_mob_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldRejectNonMobHolder 2956 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity non-mob holder early-return gate for dual-wire
// cross-check (slice 2956):
//   !holderIsMob
auto inlineShouldRejectNonMobHolder(const bool holderIsMob) -> bool
{
    return !holderIsMob;
}

} // namespace

// Pure dual-wire expansion for enmityrejecthelpers::ShouldRejectNonMobHolder
// (holder objtype != TYPE_MOB early return; slice 2956).
auto runEnmityRejectNonMob2956SelfTests() -> bool
{
    using enmityrejecthelpers::ShouldRejectNonMobHolder;

    bool ok = true;

    const struct
    {
        bool        holderIsMob;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "non-mob holder rejected" },
        { true, false, "mob holder accepted" },

        // Residual 1357 pins.
        { false, true, "residual reject non-mob" },
        { true, false, "residual accept mob" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNonMobHolder(c.holderIsMob);
        const bool inlineF = inlineShouldRejectNonMobHolder(c.holderIsMob);
        const bool wantPin = !c.holderIsMob;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNonMobHolder dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNonMobHolder == pin formula !holderIsMob") && ok;
    }

    // Pin composition: negation identity only.
    ok = expect(ShouldRejectNonMobHolder(false), "!holderIsMob must reject") && ok;
    ok = expect(!ShouldRejectNonMobHolder(true), "holderIsMob must accept") && ok;

    // Dense compose: both bool inputs (full domain).
    for (const bool holderIsMob : { false, true })
    {
        const bool got  = ShouldRejectNonMobHolder(holderIsMob);
        const bool want = !holderIsMob;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNonMobHolder(holderIsMob), "compose free == inline") && ok;
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
    }

    return ok;
}
