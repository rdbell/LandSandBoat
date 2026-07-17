#include "test_latent_reject_process_3343.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent ShouldRejectProcessLatent 3343 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ProcessLatentEffect owner/zone early-return gate for dual-wire
// cross-check (dedicated slice 3343 expand residual 2961). Positive OR form:
//   ownerNull || zoneIsZero
auto inlineShouldRejectProcessLatent(const bool ownerNull, const bool zoneIsZero) -> bool
{
    return ownerNull || zoneIsZero;
}

// Compact dual-wire pin matching Go pinShouldRejectProcessLatent3343 /
// C++ capacity. Positive OR form only (not De Morgan / if-else):
//   ownerNull || zoneIsZero
auto pinShouldRejectProcessLatent(const bool ownerNull, const bool zoneIsZero) -> bool
{
    return ownerNull || zoneIsZero;
}

} // namespace

// Pure dual-wire expansion for latenthelpers::ShouldRejectProcessLatent
// (owner null or zone id 0 early return; OmegaXI internal/latenteffect;
// dedicated slice 3343 expand residual 2961 / pure 1359).
//
// Coverage:
//   - free == inline == pin (positive OR form)
//   - residual 1359 / 2961 poles still hold
//   - dense full 2² bool domain
//   - host-style ProcessLatentEffect inject poles
auto runLatentRejectProcess3343SelfTests() -> bool
{
    using latenthelpers::ShouldRejectProcessLatent;

    bool ok = true;

    // Residual 1359 / 2961 pins still hold under dual-wire.
    ok = expect(ShouldRejectProcessLatent(true, false), "residual 2961/1359: ownerNull must reject") && ok;
    ok = expect(ShouldRejectProcessLatent(false, true), "residual 2961/1359: zoneIsZero must reject") && ok;
    ok = expect(!ShouldRejectProcessLatent(false, false), "residual 2961/1359: neither pole must accept") && ok;
    ok = expect(ShouldRejectProcessLatent(true, true), "residual 2961/1359: both poles must reject") && ok;

    const struct
    {
        bool        ownerNull;
        bool        zoneIsZero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, false, true, "pole owner null rejected" },
        { false, true, true, "pole zone zero rejected" },
        { false, false, false, "pole owner present zone non-zero accepted" },
        { true, true, true, "pole both reject" },

        // Residual 2961 / 1359 re-pins.
        { true, false, true, "residual reject owner null" },
        { false, true, true, "residual reject zone zero" },
        { false, false, false, "residual accept process" },
        { true, true, true, "residual both reject poles" },

        // Host-style ProcessLatentEffect inject poles.
        { true, true, true, "null owner (playerZoneID forced 0): reject early return" },
        { true, false, true, "null owner inject alone: reject" },
        { false, true, true, "present owner zone 0: reject early return" },
        { false, false, false, "present owner zone non-zero: continue evaluation" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool inlineF = inlineShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool pinF    = pinShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool wantPin = c.ownerNull || c.zoneIsZero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectProcessLatent dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldRejectProcessLatent dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectProcessLatent == pin formula ownerNull || zoneIsZero") && ok;
    }

    // Pin composition: positive OR identity only.
    ok = expect(ShouldRejectProcessLatent(true, false), "ownerNull must reject") && ok;
    ok = expect(ShouldRejectProcessLatent(false, true), "zoneIsZero must reject") && ok;
    ok = expect(!ShouldRejectProcessLatent(false, false), "neither pole must accept") && ok;
    ok = expect(ShouldRejectProcessLatent(true, true), "both poles must reject") && ok;

    // Explicit polarity: reject is positive OR of injects (never De Morgan).
    for (const bool ownerNull : { false, true })
    {
        for (const bool zoneIsZero : { false, true })
        {
            const bool got  = ShouldRejectProcessLatent(ownerNull, zoneIsZero);
            const bool want = ownerNull || zoneIsZero;
            ok              = expect(got == want, "polarity: free == ownerNull || zoneIsZero") && ok;
            ok              = expect(!(got && !(ownerNull || zoneIsZero)), "polarity: never reject when neither pole") && ok;
            ok              = expect(!(!got && (ownerNull || zoneIsZero)), "polarity: always reject when either pole") && ok;
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
    } hostCases[] = {
        { true, true, true, "null owner (playerZoneID forced 0): reject early return" },
        { true, false, true, "null owner inject alone: reject" },
        { false, true, true, "present owner zone 0: reject early return" },
        { false, false, false, "present owner zone non-zero: continue evaluation" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool inlineF = inlineShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);
        const bool pinF    = pinShouldRejectProcessLatent(c.ownerNull, c.zoneIsZero);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == (c.ownerNull || c.zoneIsZero), "host compose free == ownerNull || zoneIsZero (positive OR)") && ok;
    }

    // Dense compose: free == inline == pin positive OR for full 2².
    for (const bool ownerNull : { false, true })
    {
        for (const bool zoneIsZero : { false, true })
        {
            const bool got  = ShouldRejectProcessLatent(ownerNull, zoneIsZero);
            const bool want = ownerNull || zoneIsZero;
            ok              = expect(got == want, "compose free == pin formula ownerNull || zoneIsZero") && ok;
            ok              = expect(got == inlineShouldRejectProcessLatent(ownerNull, zoneIsZero),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldRejectProcessLatent(ownerNull, zoneIsZero), "compose free == pin") && ok;
        }
    }

    // Residual 2961 suite still holds under dedicated expand.
    ok = expect(ShouldRejectProcessLatent(true, false), "residual 2961 owner-null pin under 3343") && ok;
    ok = expect(ShouldRejectProcessLatent(false, true), "residual 2961 zone-zero pin under 3343") && ok;
    ok = expect(!ShouldRejectProcessLatent(false, false), "residual 2961 accept pin under 3343") && ok;
    ok = expect(ShouldRejectProcessLatent(true, true), "residual 2961 both-reject pin under 3343") && ok;

    return ok;
}
