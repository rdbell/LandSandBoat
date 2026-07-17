#include "test_latent_reject_process_2961.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent ShouldRejectProcessLatent 2961 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ProcessLatentEffect owner/zone early-return gate for dual-wire
// cross-check (slice 2961):
//   ownerNull || zoneIsZero
auto inlineShouldRejectProcessLatent(const bool ownerNull, const bool zoneIsZero) -> bool
{
    return ownerNull || zoneIsZero;
}

} // namespace

// Pure dual-wire expansion for latenthelpers::ShouldRejectProcessLatent
// (owner null or zone id 0 early return; slice 2961).
auto runLatentRejectProcess2961SelfTests() -> bool
{
    using latenthelpers::ShouldRejectProcessLatent;

    bool ok = true;

    const struct
    {
        bool        ownerNull;
        bool        zoneIsZero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, false, true, "owner null rejected" },
        { false, true, true, "zone zero rejected" },
        { false, false, false, "owner present zone non-zero accepted" },
        { true, true, true, "both reject poles" },

        // Residual 1359 pins.
        { true, false, true, "residual reject owner null" },
        { false, true, true, "residual reject zone zero" },
        { false, false, false, "residual accept process" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool inlineF = inlineShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool wantPin = c.ownerNull || c.zoneIsZero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectProcessLatent dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectProcessLatent == pin formula ownerNull || zoneIsZero") && ok;
    }

    // Pin composition: OR identity only.
    ok = expect(ShouldRejectProcessLatent(true, false), "ownerNull must reject") && ok;
    ok = expect(ShouldRejectProcessLatent(false, true), "zoneIsZero must reject") && ok;
    ok = expect(!ShouldRejectProcessLatent(false, false), "neither pole must accept") && ok;
    ok = expect(ShouldRejectProcessLatent(true, true), "both poles must reject") && ok;

    // Dense compose: full 2×2 bool domain.
    for (const bool ownerNull : { false, true })
    {
        for (const bool zoneIsZero : { false, true })
        {
            const bool got  = ShouldRejectProcessLatent(ownerNull, zoneIsZero);
            const bool want = ownerNull || zoneIsZero;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectProcessLatent(ownerNull, zoneIsZero),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style compose poles: ProcessLatentEffect injects
    // ownerNull = (m_POwner == nullptr),
    // zoneIsZero = (playerZoneID == 0) where
    // playerZoneID = m_POwner != nullptr ? m_POwner->getZone() : 0.
    const struct
    {
        bool        ownerNull;
        bool        zoneIsZero;
        bool        wantReject;
        const char* label;
    } composeCases[] = {
        { true, true, true, "null owner (playerZoneID forced 0): reject early return" },
        { true, false, true, "null owner inject alone: reject" },
        { false, true, true, "present owner zone 0: reject early return" },
        { false, false, false, "present owner zone non-zero: continue evaluation" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        ok             = expect(got == c.wantReject, c.label) && ok;
        ok             = expect(got == (c.ownerNull || c.zoneIsZero), "compose free == ownerNull || zoneIsZero pin") && ok;
        ok             = expect(got == inlineShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero), "compose free == inline") && ok;
    }

    return ok;
}
