#include "test_artisan_can_claim_scroll_3396.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanClaimScroll 3396 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua moogleOnFinish option 99 pure gate for dual-wire
// cross-check (slice 3396 dedicated expand residual 2916; prior dedicated 3132):
//   nextScroll < JstMidnight()
// Direct return form matching production free function / capacity.
auto inlineCanClaimScroll(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

// Compact dual-wire pin matching free function / capacity body (slice 3396).
// Direct return only — same formula as production CanClaimScroll.
auto pinCanClaimScroll3396(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanClaimScroll
// (Lua moogleOnFinish option 99 Get Scroll gate; OmegaXI internal/artisan;
// slice 3396 dedicated expand residual 2916; prior dedicated 3132).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual poles: nextScroll before / at / after jstMidnight
//   - dense timestamp grid
//   - residual 0948 / 2916 / prior dedicated 3132 pins still hold
auto runArtisanCanClaimScroll3396SelfTests() -> bool
{
    using artisanhelpers::CanClaimScroll;

    bool ok = true;

    // Residual 0948 / 2916 / prior dedicated 3132 pins still hold under dual-wire.
    ok = expect(CanClaimScroll(100, 200), "residual: nextScroll < midnight may claim") && ok;
    ok = expect(!CanClaimScroll(200, 200), "residual: equal timestamps must not claim") && ok;
    ok = expect(!CanClaimScroll(300, 200), "residual: nextScroll > midnight must not claim") && ok;
    ok = expect(CanClaimScroll(0, 1), "residual: never claimed must claim") && ok;

    // Residual poles: free == inline == pin (direct return).
    const struct
    {
        int64       nextScroll;
        int64       jstMidnight;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 0948 / 2916 classic dual poles.
        { 100, 200, true, "residual nextScroll < midnight" },
        { 200, 200, false, "residual equal timestamps" },
        { 300, 200, false, "residual nextScroll > midnight" },
        { 0, 1, true, "residual never claimed" },
        { 0, 0, false, "residual both zero (equal)" },
        { -1, 0, true, "residual negative nextScroll before midnight" },
        { 1, 0, false, "residual nextScroll after zero midnight" },
        { 0, -1, false, "residual midnight already past zero nextScroll" },
        { 100, 101, true, "residual one second before midnight" },
        { 101, 100, false, "residual one second after midnight" },
        { 1783090799, 1783090800, true, "residual JST midnight-1" },
        { 1783090800, 1783090800, false, "residual exact JST midnight" },
        { 1783090801, 1783090800, false, "residual JST midnight+1" },
        { -100, -50, true, "residual both negative, nextScroll earlier" },
        { -50, -100, false, "residual both negative, nextScroll later" },
        { -50, -50, false, "residual both negative equal" },

        // Prior dedicated 3132 dense poles.
        { 100, 200, true, "prior nextScroll < midnight" },
        { 200, 200, false, "prior equal timestamps" },
        { 300, 200, false, "prior nextScroll > midnight" },
        { 0, 1, true, "prior never claimed" },
        { 1783090799, 1783090800, true, "prior JST midnight-1" },
        { 1783090800, 1783090800, false, "prior exact JST midnight" },
        { 1783090801, 1783090800, false, "prior JST midnight+1" },

        // Host inject path poles (moogleOnFinish option 99).
        { 100, 200, true, "host nextScroll before → claim" },
        { 200, 200, false, "host equal → skip claim" },
        { 300, 200, false, "host next after midnight → skip claim" },
        { 0, 1, true, "host never claimed → claim" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanClaimScroll(p.nextScroll, p.jstMidnight);
        const bool inlineF = inlineCanClaimScroll(p.nextScroll, p.jstMidnight);
        const bool pin     = pinCanClaimScroll3396(p.nextScroll, p.jstMidnight);
        const bool wantF   = p.nextScroll < p.jstMidnight;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanClaimScroll dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanClaimScroll == pin formula") && ok;
        ok = expect(got == wantF, "formula free==nextScroll<jstMidnight") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(CanClaimScroll(100, 200) == pinCanClaimScroll3396(100, 200), "free==pin nextScroll < midnight") && ok;
    ok = expect(CanClaimScroll(200, 200) == pinCanClaimScroll3396(200, 200), "free==pin equal timestamps") && ok;
    ok = expect(CanClaimScroll(300, 200) == pinCanClaimScroll3396(300, 200), "free==pin nextScroll > midnight") && ok;
    ok = expect(CanClaimScroll(0, 1) == pinCanClaimScroll3396(0, 1), "free==pin never claimed") && ok;

    // Dense relative offsets around midnight: free == inline == pin == formula.
    constexpr int64 midnight = 1000000;
    for (int64 d = -50; d <= 50; ++d)
    {
        const int64 next   = midnight + d;
        const bool  got    = CanClaimScroll(next, midnight);
        const bool  want   = next < midnight;
        const bool  inlineF = inlineCanClaimScroll(next, midnight);
        const bool  pin    = pinCanClaimScroll3396(next, midnight);
        ok = expect(got == want, "dense CanClaimScroll formula") && ok;
        ok = expect(got == inlineF, "dense free==inline") && ok;
        ok = expect(got == pin, "dense free==pin") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanClaimScroll(100, 200), "nextScroll under must apply") && ok;
    ok = expect(!CanClaimScroll(200, 200), "equal timestamps must reject") && ok;
    ok = expect(!CanClaimScroll(300, 200), "nextScroll over must reject") && ok;
    ok = expect(CanClaimScroll(0, 1), "never claimed must apply") && ok;

    // Production moogleOnFinish path semantics:
    // free → may giveItem / setCharVar; fail → skip claim.
    ok = expect(CanClaimScroll(100, 200), "moogleOnFinish free → claim path") && ok;
    ok = expect(!CanClaimScroll(200, 200), "moogleOnFinish equal → skip claim") && ok;
    ok = expect(!CanClaimScroll(300, 200), "moogleOnFinish next after midnight → skip claim") && ok;
    ok = expect(CanClaimScroll(0, 1), "moogleOnFinish never claimed → claim path") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    const struct
    {
        int64       nextScroll;
        int64       jstMidnight;
        const char* label;
    } hostPoles[] = {
        { 100, 200, "eligible claim path" },
        { 200, 200, "equal skip" },
        { 300, 200, "next after midnight skip" },
        { 0, 1, "never claimed claim" },
        { 0, 0, "both zero skip" },
        { 1783090799, 1783090800, "JST midnight-1 claim" },
        { 1783090800, 1783090800, "exact JST midnight skip" },
        { 1783090801, 1783090800, "JST midnight+1 skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = CanClaimScroll(p.nextScroll, p.jstMidnight);
        const bool inlineF = inlineCanClaimScroll(p.nextScroll, p.jstMidnight);
        const bool pin     = pinCanClaimScroll3396(p.nextScroll, p.jstMidnight);
        ok                 = expect(got == pin, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    return ok;
}
