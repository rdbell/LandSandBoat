#include "test_attack_block_counter_state_3265.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldBlockCounterForState 3265 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAttack::CheckCounter engaged / prevent-action gate for dual-wire
// cross-check (dedicated slice 3265):
//   !isEngaged || hasPreventActionIgnoringCharm
auto inlineShouldBlockCounterForState(const bool isEngaged, const bool hasPreventActionIgnoringCharm) -> bool
{
    return !isEngaged || hasPreventActionIgnoringCharm;
}

// Compact dual-wire pin matching Go pinShouldBlockCounterForState3265 / C++ capacity:
//   !isEngaged || hasPreventActionIgnoringCharm
auto pinShouldBlockCounterForState(const bool isEngaged, const bool hasPreventActionIgnoringCharm) -> bool
{
    return !isEngaged || hasPreventActionIgnoringCharm;
}

} // namespace

// Pure dual-wire expansion for attackhelpers::ShouldBlockCounterForState
// (!isEngaged || hasPreventActionIgnoringCharm; dedicated slice 3265;
// residual pure 1376).
//
// Coverage:
//   - free == inline == pin == (!isEngaged || hasPreventActionIgnoringCharm)
//   - residual 1376 pins still hold
//   - residual poles + dense 2² over both bool inputs
//   - CheckCounter path sibling residual 3003 / 3204 left residual
auto runAttackBlockCounterState3265SelfTests() -> bool
{
    using namespace attackhelpers;

    bool ok = true;

    // Residual 1376 pins still hold under dual-wire.
    ok = expect(ShouldBlockCounterForState(false, false), "residual: not engaged blocks counter") && ok;
    ok = expect(ShouldBlockCounterForState(true, true), "residual: prevent action blocks counter") && ok;
    ok = expect(!ShouldBlockCounterForState(true, false), "residual: engaged without prevent can counter") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        isEngaged;
        bool        hasPreventActionIgnoringCharm;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1376 poles.
        { false, false, true, "residual not engaged blocks" },
        { true, true, true, "residual prevent action blocks" },
        { true, false, false, "residual engaged can counter" },

        // Classic dual poles (dense 2²).
        { false, false, true, "not engaged, no prevent → block" },
        { false, true, true, "not engaged, prevent → block" },
        { true, false, false, "engaged, no prevent → can counter" },
        { true, true, true, "engaged, prevent → block" },

        // Boundary re-pins of the only allow cell and block neighbors.
        { true, false, false, "only allow cell: engaged && !prevent" },
        { false, false, true, "block neighbor: !engaged && !prevent" },
        { true, true, true, "block neighbor: engaged && prevent" },
        { false, true, true, "block neighbor: !engaged && prevent" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBlockCounterForState(c.isEngaged, c.hasPreventActionIgnoringCharm);
        const bool inlineF = inlineShouldBlockCounterForState(c.isEngaged, c.hasPreventActionIgnoringCharm);
        const bool pin     = pinShouldBlockCounterForState(c.isEngaged, c.hasPreventActionIgnoringCharm);
        const bool wantPin = !c.isEngaged || c.hasPreventActionIgnoringCharm;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin (direct return; QF1001-safe).
        ok = expect(got == inlineF && got == pin, "ShouldBlockCounterForState dual-wire free == inline == pin") &&
             ok;
        ok = expect(got == wantPin, "free == pin formula !engaged || prevent") && ok;
    }

    // Explicit residual poles free == inline == pin for dense 2² cells.
    for (const bool engaged : { false, true })
    {
        for (const bool prevent : { false, true })
        {
            const bool got     = ShouldBlockCounterForState(engaged, prevent);
            const bool inlineF = inlineShouldBlockCounterForState(engaged, prevent);
            const bool pin     = pinShouldBlockCounterForState(engaged, prevent);
            const bool want    = !engaged || prevent;
            ok                 = expect(got == want, "pole free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
        }
    }

    // Dense 2² compose over both bool domains — free == inline == pin.
    for (const bool engaged : { false, true })
    {
        for (const bool prevent : { false, true })
        {
            const bool got     = ShouldBlockCounterForState(engaged, prevent);
            const bool inlineF = inlineShouldBlockCounterForState(engaged, prevent);
            const bool pin     = pinShouldBlockCounterForState(engaged, prevent);
            const bool want    = !engaged || prevent;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host path: CAttack::CheckCounter injects IsEngaged +
    // HasPreventActionEffect(true); when block is true sets m_isCountered=false
    // and returns (no merit/rate/facing evaluation).
    const struct
    {
        bool        isEngaged;
        bool        prevent;
        bool        wantBlock;
        const char* label;
    } hostCases[] = {
        { false, false, true, "not engaged → CheckCounter blocks" },
        { false, true, true, "not engaged + prevent → CheckCounter blocks" },
        { true, true, true, "engaged + prevent → CheckCounter blocks" },
        { true, false, false, "engaged no prevent → host continues counter path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldBlockCounterForState(c.isEngaged, c.prevent);
        const bool inlineF = inlineShouldBlockCounterForState(c.isEngaged, c.prevent);
        const bool pin     = pinShouldBlockCounterForState(c.isEngaged, c.prevent);

        ok = expect(got == c.wantBlock, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host compose dual-wire free == inline == pin") && ok;

        // Host compose: if (block) { m_isCountered=false; return; } else continue.
        const bool counteredAttemptAllowed = !got;
        if (c.wantBlock)
        {
            ok = expect(!counteredAttemptAllowed, "block must force early return (no counter attempt)") && ok;
        }
        else
        {
            ok = expect(counteredAttemptAllowed, "non-block must allow counter path") && ok;
        }
    }

    // CheckCounter path sibling residual 3003 / 3204 still hold
    // (ShouldSkipCounterForDaken not re-expanded under 3265).
    ok = expect(ShouldSkipCounterForDaken(AttackTypeDaken),
                "sibling residual: Daken skips counter (3003/3204)") &&
         ok;
    ok = expect(!ShouldSkipCounterForDaken(AttackTypeNormal),
                "sibling residual: Normal does not skip counter (3003/3204)") &&
         ok;

    return ok;
}
