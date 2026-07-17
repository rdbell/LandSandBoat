#include "test_nyzul_claim_rune_2902.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul claim rune 2902 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua Rune of Transfer onEventUpdate first-claimer gate for dual-wire
// checks: instance:getLocalVar('runeHandler') == 0
auto inlineCanClaimRuneHandler(const int32 runeHandler) -> bool
{
    return runeHandler == 0;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::CanClaimRuneHandler
// (Lua Rune of Transfer onEventUpdate first-claimer gate).
auto runNyzulClaimRune2902SelfTests() -> bool
{
    using nyzulhelpers::CanClaimRuneHandler;

    bool ok = true;

    const struct
    {
        int32       runeHandler;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "unset runeHandler claims" },
        { 1, false, "player id 1 already claimed" },
        { 42, false, "player id 42 already claimed" },
        { -1, false, "defensive negative non-zero" },
        { 100, false, "large non-zero claimed" },
        { 0x7FFFFFFF, false, "max int claimed" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanClaimRuneHandler(c.runeHandler);
        const bool inlineF = inlineCanClaimRuneHandler(c.runeHandler);
        const bool pure    = c.runeHandler == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == runeHandler==0") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1088 pins.
    ok = expect(CanClaimRuneHandler(0), "residual unset claims") && ok;
    ok = expect(!CanClaimRuneHandler(1), "residual claimed blocks") && ok;

    // Dense compose range identity: runeHandler -2..4.
    for (int32 rh = -2; rh <= 4; ++rh)
    {
        const bool got  = CanClaimRuneHandler(rh);
        const bool want = rh == 0;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineCanClaimRuneHandler(rh),
                    "compose range free == inline") &&
             ok;
    }

    return ok;
}
