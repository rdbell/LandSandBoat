#include "test_artisan_can_claim_scroll_3132.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanClaimScroll 3132 self-test failed: " << label << '\n';
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

// Compact dual-wire pin matching Go pinCanClaimScroll3132:
//   nextScroll < jstMidnight
auto pinCanClaimScroll(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanClaimScroll
// (Lua moogleOnFinish option 99 Get Scroll gate; OmegaXI internal/artisan;
// slice 3132).
//
// Coverage:
//   - nextScroll before / at / after jstMidnight
//   - free == inline == pin == nextScroll < jstMidnight
//   - residual 0948 / 2916 pins still hold
auto runArtisanCanClaimScroll3132SelfTests() -> bool
{
    using artisanhelpers::CanClaimScroll;

    bool ok = true;

    // Residual 0948 / 2916 pins still hold under dual-wire.
    ok = expect(CanClaimScroll(100, 200), "residual: nextScroll < midnight may claim") && ok;
    ok = expect(!CanClaimScroll(200, 200), "residual: equal timestamps must not claim") && ok;
    ok = expect(!CanClaimScroll(300, 200), "residual: nextScroll > midnight must not claim") && ok;
    ok = expect(CanClaimScroll(0, 1), "residual: never claimed must claim") && ok;

    // --- nextScroll before / at / after jstMidnight ---
    const struct
    {
        int64       nextScroll;
        int64       jstMidnight;
        bool        want;
        const char* label;
    } cases[] = {
        // residual 0948 poles
        { 100, 200, true, "residual nextScroll < midnight" },
        { 200, 200, false, "residual equal timestamps" },
        { 300, 200, false, "residual nextScroll > midnight" },
        { 0, 1, true, "residual never claimed" },
        // nextScroll before jstMidnight
        { 0, 0, false, "both zero (equal)" },
        { -1, 0, true, "negative nextScroll before midnight" },
        { 100, 101, true, "one second before midnight" },
        { 1783090799, 1783090800, true, "JST midnight-1" },
        { -100, -50, true, "both negative, nextScroll earlier" },
        // nextScroll at jstMidnight (equal)
        { 1783090800, 1783090800, false, "exact JST midnight" },
        { -50, -50, false, "both negative equal" },
        // nextScroll after jstMidnight
        { 1, 0, false, "nextScroll after zero midnight" },
        { 0, -1, false, "midnight already past zero nextScroll" },
        { 101, 100, false, "one second after midnight" },
        { 1783090801, 1783090800, false, "JST midnight+1" },
        { -50, -100, false, "both negative, nextScroll later" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanClaimScroll(c.nextScroll, c.jstMidnight);
        const bool inlineF = inlineCanClaimScroll(c.nextScroll, c.jstMidnight);
        const bool pinGot  = pinCanClaimScroll(c.nextScroll, c.jstMidnight);
        const bool wantF   = c.nextScroll < c.jstMidnight;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantF, "formula free==nextScroll<jstMidnight") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(CanClaimScroll(100, 200) == pinCanClaimScroll(100, 200), "free==pin nextScroll < midnight") && ok;
    ok = expect(CanClaimScroll(200, 200) == pinCanClaimScroll(200, 200), "free==pin equal timestamps") && ok;
    ok = expect(CanClaimScroll(300, 200) == pinCanClaimScroll(300, 200), "free==pin nextScroll > midnight") && ok;
    ok = expect(CanClaimScroll(0, 1) == pinCanClaimScroll(0, 1), "free==pin never claimed") && ok;

    // Dense relative offsets around midnight: free == inline == pin == formula.
    constexpr int64 midnight = 1000000;
    for (int64 d = -50; d <= 50; ++d)
    {
        const int64 next  = midnight + d;
        const bool  got   = CanClaimScroll(next, midnight);
        const bool  want  = next < midnight;
        ok = expect(got == want, "dense CanClaimScroll formula") && ok;
        ok = expect(got == inlineCanClaimScroll(next, midnight), "dense free==inline") && ok;
        ok = expect(got == pinCanClaimScroll(next, midnight), "dense free==pin") && ok;
    }

    // Production moogleOnFinish path semantics:
    // free → may giveItem / setCharVar; fail → skip claim.
    ok = expect(CanClaimScroll(100, 200), "moogleOnFinish free → claim path") && ok;
    ok = expect(!CanClaimScroll(200, 200), "moogleOnFinish equal → skip claim") && ok;
    ok = expect(!CanClaimScroll(300, 200), "moogleOnFinish next after midnight → skip claim") && ok;
    ok = expect(CanClaimScroll(0, 1), "moogleOnFinish never claimed → claim path") && ok;

    return ok;
}
