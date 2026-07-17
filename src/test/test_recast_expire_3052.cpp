#include "test_recast_expire_3052.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldExpireRecast 3052 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Check expiry gate for dual-wire cross-check (slice 3052):
//   nowUnits >= timeStampUnits + recastTimeUnits
auto inlineShouldExpireRecast(const int64 nowUnits, const int64 timeStampUnits, const int64 recastTimeUnits) -> bool
{
    return nowUnits >= timeStampUnits + recastTimeUnits;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldExpireRecast
// (now >= TimeStamp + RecastTime Check gate; slice 3052).
auto runRecastExpire3052SelfTests() -> bool
{
    using recasthelpers::ShouldExpireRecast;

    bool ok = true;

    const struct
    {
        int64       nowUnits;
        int64       timeStampUnits;
        int64       recastTimeUnits;
        bool        want;
        const char* label;
    } cases[] = {
        // Boundary: now == stamp + recast → true.
        { 50, 10, 40, true, "exactly at boundary" },
        // One unit before → false.
        { 49, 10, 40, false, "one unit before" },
        // After → true.
        { 51, 10, 40, true, "after boundary" },
        // Zero recastTime → expire when now >= stamp.
        { 10, 10, 0, true, "zero recast at stamp" },
        { 11, 10, 0, true, "zero recast after stamp" },
        { 9, 10, 0, false, "zero recast before stamp" },

        // Residual 1370 pins.
        { 50, 10, 40, true, "residual 1370 expire" },
        { 49, 10, 40, false, "residual 1370 not expire" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldExpireRecast(c.nowUnits, c.timeStampUnits, c.recastTimeUnits);
        const bool inlineF = inlineShouldExpireRecast(c.nowUnits, c.timeStampUnits, c.recastTimeUnits);
        const bool wantPin = c.nowUnits >= c.timeStampUnits + c.recastTimeUnits;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldExpireRecast dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldExpireRecast == pin formula now>=stamp+recast") && ok;
    }

    // Pin composition: residual poles.
    ok = expect(ShouldExpireRecast(50, 10, 40), "residual expire must be true") && ok;
    ok = expect(!ShouldExpireRecast(49, 10, 40), "residual not-expire must be false") && ok;

    // Dense compose: free == pin == inline across a small unit grid.
    constexpr int64 stamp = 100;
    for (const int64 recast : { int64{ 0 }, int64{ 1 }, int64{ 40 } })
    {
        for (int64 delta = -2; delta <= 2; ++delta)
        {
            const int64 now  = stamp + recast + delta;
            const bool  got  = ShouldExpireRecast(now, stamp, recast);
            const bool  want = now >= stamp + recast;
            ok               = expect(got == want, "compose free == pin formula") && ok;
            ok               = expect(got == inlineShouldExpireRecast(now, stamp, recast), "compose free == inline") && ok;
        }
    }

    // Host-style inject poles (Check injects epoch counts into free function).
    // (Live container Check erase/retain is residual 1370 / recast_container tests.)
    const struct
    {
        int64 now;
        int64 stamp;
        int64 recast;
    } poles[] = {
        { 50, 10, 40 },
        { 49, 10, 40 },
        { 10, 10, 0 },
        { 9, 10, 0 },
    };
    for (const auto& p : poles)
    {
        const bool want = p.now >= p.stamp + p.recast;
        ok              = expect(ShouldExpireRecast(p.now, p.stamp, p.recast) == want, "host inject dual-wire identity") && ok;
        ok              = expect(ShouldExpireRecast(p.now, p.stamp, p.recast) == inlineShouldExpireRecast(p.now, p.stamp, p.recast),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
