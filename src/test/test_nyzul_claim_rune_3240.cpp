#include "test_nyzul_claim_rune_3240.h"

#include "map/nyzul_capacity.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul claim rune 3240 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua Rune of Transfer onEventUpdate first-claimer gate for dual-wire
// cross-check (slice 3240 dedicated expand residual 2902):
//   instance:getLocalVar('runeHandler') == 0
// Direct return form matching production free function / capacity.
auto inlineCanClaimRuneHandler(const int32 runeHandler) -> bool
{
    return runeHandler == 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3240).
// Direct return only — same formula as production CanClaimRuneHandler.
auto pinCanClaimRuneHandler3240(const int32 runeHandler) -> bool
{
    return runeHandler == 0;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::CanClaimRuneHandler
// (Lua Rune of Transfer onEventUpdate first-claimer gate; slice 3240
// dedicated expand residual 2902). Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 2902 / 1088 poles still hold (0 true, non-zero false)
//   - residual poles + dense edges (neg, 1, max)
auto runNyzulClaimRune3240SelfTests() -> bool
{
    using nyzulhelpers::CanClaimRuneHandler;

    bool ok = true;

    // Residual 2902 / 1088 pins still hold.
    ok = expect(CanClaimRuneHandler(0), "residual unset claims") && ok;
    ok = expect(!CanClaimRuneHandler(1), "residual player id 1 blocks") && ok;
    ok = expect(!CanClaimRuneHandler(42), "residual player id 42 blocks") && ok;
    ok = expect(!CanClaimRuneHandler(-1), "residual defensive negative blocks") && ok;
    ok = expect(!CanClaimRuneHandler(0x7FFFFFFF), "residual max int blocks") && ok;

    const struct
    {
        int32       runeHandler;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2902 / 1088 classic dual poles.
        { 0, true, "residual unset runeHandler claims" },
        { 1, false, "residual player id 1 already claimed" },
        { 42, false, "residual player id 42 already claimed" },
        { -1, false, "residual defensive negative non-zero" },
        { 100, false, "residual large non-zero claimed" },
        { 0x7FFFFFFF, false, "residual max int claimed" },

        // Host inject path poles (Rune_of_Transfer onEventUpdate).
        { 0, true, "host unset continue claim path" },
        { 12345, false, "host player id already claimer early skip" },
        { 2, false, "host player id 2 already claimer" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanClaimRuneHandler(p.runeHandler);
        const bool inlineF = inlineCanClaimRuneHandler(p.runeHandler);
        const bool pin     = pinCanClaimRuneHandler3240(p.runeHandler);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanClaimRuneHandler dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanClaimRuneHandler == pin formula") && ok;
        ok = expect(got == (p.runeHandler == 0), "CanClaimRuneHandler == formula") && ok;
    }

    // Dense edges: negative, 0, 1, small non-zero, large, max int.
    // free == inline == pin == formula.
    const struct
    {
        int32       runeHandler;
        const char* label;
    } edges[] = {
        { -2, "edge neg -2" },
        { -1, "edge neg -1" },
        { 0, "edge zero claims" },
        { 1, "edge one claimed" },
        { 2, "edge two claimed" },
        { 42, "edge mid claimed" },
        { 100, "edge hundred claimed" },
        { 0x7FFFFFFF, "edge max int claimed" },
        { std::numeric_limits<int32>::min(), "edge min int claimed" },
    };

    for (const auto& e : edges)
    {
        const bool got     = CanClaimRuneHandler(e.runeHandler);
        const bool inlineF = inlineCanClaimRuneHandler(e.runeHandler);
        const bool pin     = pinCanClaimRuneHandler3240(e.runeHandler);
        const bool want    = e.runeHandler == 0;

        ok = expect(got == want, e.label) && ok;
        ok = expect(got == inlineF, "dense edge free == inline") && ok;
        ok = expect(got == pin, "dense edge free == pin") && ok;
    }

    // Dense compose range identity: runeHandler -4..4.
    // free == inline == pin == formula.
    for (int32 rh = -4; rh <= 4; ++rh)
    {
        const bool got     = CanClaimRuneHandler(rh);
        const bool inlineF = inlineCanClaimRuneHandler(rh);
        const bool pin     = pinCanClaimRuneHandler3240(rh);
        const bool want    = rh == 0;

        ok = expect(got == want, "dense compose free == formula") && ok;
        ok = expect(got == inlineF, "dense compose free == inline") && ok;
        ok = expect(got == pin, "dense compose free == pin") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanClaimRuneHandler(0), "runeHandler 0 must claim") && ok;
    ok = expect(!CanClaimRuneHandler(1), "runeHandler 1 must block") && ok;
    ok = expect(!CanClaimRuneHandler(-1), "runeHandler -1 must block") && ok;
    ok = expect(!CanClaimRuneHandler(0x7FFFFFFF), "runeHandler max int must block") && ok;

    // Production path semantics (host inject model for onEventUpdate).
    const struct
    {
        int32       runeHandler;
        const char* label;
    } hostPoles[] = {
        { 0, "eligible claim path" },
        { 1, "player 1 already claimer skip" },
        { 999, "large player id already claimer skip" },
        { -1, "defensive negative skip" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got     = CanClaimRuneHandler(p.runeHandler);
        const bool inlineF = inlineCanClaimRuneHandler(p.runeHandler);
        const bool pin     = pinCanClaimRuneHandler3240(p.runeHandler);

        ok = expect(got == pin, p.label) && ok;
        ok = expect(got == inlineF, "host pole free == inline") && ok;
    }

    return ok;
}
