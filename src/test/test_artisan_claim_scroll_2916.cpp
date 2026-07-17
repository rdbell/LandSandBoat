#include "test_artisan_claim_scroll_2916.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanClaimScroll 2916 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua moogleOnFinish option 99 pure gate for dual-wire
// cross-check:
//   nextScroll < JstMidnight()
auto inlineCanClaimScroll(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanClaimScroll
// (artisan.lua moogleOnFinish option 99 Get Scroll gate).
auto runArtisanClaimScroll2916SelfTests() -> bool
{
    using artisanhelpers::CanClaimScroll;

    bool ok = true;

    const struct
    {
        int64       nextScroll;
        int64       jstMidnight;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 0948 pins.
        { 100, 200, true, "residual nextScroll < midnight" },
        { 200, 200, false, "residual equal timestamps" },
        { 300, 200, false, "residual nextScroll > midnight" },
        { 0, 1, true, "residual never claimed" },
        // Boundary matrix.
        { 0, 0, false, "both zero (equal)" },
        { -1, 0, true, "negative nextScroll before midnight" },
        { 1, 0, false, "nextScroll after zero midnight" },
        { 0, -1, false, "midnight already past zero nextScroll" },
        { 100, 101, true, "one second before midnight" },
        { 101, 100, false, "one second after midnight" },
        { 1783090799, 1783090800, true, "JST midnight-1" },
        { 1783090800, 1783090800, false, "exact JST midnight" },
        { 1783090801, 1783090800, false, "JST midnight+1" },
        { -100, -50, true, "both negative, nextScroll earlier" },
        { -50, -100, false, "both negative, nextScroll later" },
        { -50, -50, false, "both negative equal" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanClaimScroll(c.nextScroll, c.jstMidnight);
        const bool inlineF = inlineCanClaimScroll(c.nextScroll, c.jstMidnight);
        const bool wantF   = c.nextScroll < c.jstMidnight;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline nextScroll<jstMidnight") && ok;
        ok = expect(got == wantF, "dual-wire free == formula") && ok;
    }

    // Residual 0948 explicit pins.
    ok = expect(CanClaimScroll(100, 200), "residual CanClaimScroll(100, 200)") && ok;
    ok = expect(!CanClaimScroll(200, 200), "residual CanClaimScroll(200, 200)") && ok;
    ok = expect(!CanClaimScroll(300, 200), "residual CanClaimScroll(300, 200)") && ok;
    ok = expect(CanClaimScroll(0, 1), "residual CanClaimScroll(0, 1)") && ok;

    // Dense relative offsets around midnight.
    constexpr int64 midnight = 1000000;
    for (int64 d = -50; d <= 50; ++d)
    {
        const int64 next  = midnight + d;
        const bool  got   = CanClaimScroll(next, midnight);
        const bool  want  = next < midnight;
        ok = expect(got == want, "dense CanClaimScroll formula") && ok;
        ok = expect(got == inlineCanClaimScroll(next, midnight), "dense dual-wire == inline") && ok;
    }

    return ok;
}
