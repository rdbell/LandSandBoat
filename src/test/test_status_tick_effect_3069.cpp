#include "test_status_tick_effect_3069.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldTickEffect 3069 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline TickEffects formula for dual-wire cross-check (slice 3069):
//   tickPeriodNonzero && elapsedTickCount < elapsedThreshold
auto inlineShouldTickEffect(const bool tickPeriodNonzero, const uint32 elapsedTickCount, const uint32 elapsedThreshold) -> bool
{
    return tickPeriodNonzero && elapsedTickCount < elapsedThreshold;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldTickEffect
// (tickPeriod != 0 && elapsedTicks < threshold; slice 3069). Dense
// tickPeriodNonzero × elapsed vs threshold (equal → false, less → true).
auto runStatusTickEffect3069SelfTests() -> bool
{
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1366 pins still hold under dual-wire.
    ok = expect(ShouldTickEffect(true, 0, 1), "residual tick due (elapsed < threshold)") && ok;
    ok = expect(!ShouldTickEffect(true, 1, 1), "residual tick not due (elapsed == threshold)") && ok;
    ok = expect(!ShouldTickEffect(false, 0, 5), "residual no period does not tick") && ok;

    const struct
    {
        bool        tickPeriodNonzero;
        uint32      elapsedTickCount;
        uint32      elapsedThreshold;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual 1366 poles.
        { true, 0, 1, true, "tick due elapsed < threshold" },
        { true, 1, 1, false, "tick not due elapsed == threshold" },
        { false, 0, 5, false, "no period does not tick" },

        // Equal → false; less → true; greater → false (period nonzero).
        { true, 0, 0, false, "zero equal not due" },
        { true, 5, 5, false, "equal not due" },
        { true, 4, 5, true, "less due" },
        { true, 6, 5, false, "greater not due" },

        // tickPeriodNonzero false × equal / less / greater.
        { false, 0, 0, false, "zero period equal no tick" },
        { false, 0, 1, false, "zero period less no tick" },
        { false, 2, 1, false, "zero period greater no tick" },
        { false, 1, 1, false, "zero period equal mid no tick" },

        // First tick and multi-period progression.
        { true, 0, 1, true, "first period due" },
        { true, 1, 2, true, "second period due" },
        { true, 2, 2, false, "second period boundary not due" },
        { true, 2, 3, true, "third period due" },
        { true, 3, 3, false, "third period boundary not due" },

        // Large host-normalized counters.
        { true, 0, 1u << 20, true, "large threshold first due" },
        { true, (1u << 20) - 1, 1u << 20, true, "large just under due" },
        { true, 1u << 20, 1u << 20, false, "large equal not due" },
        { true, (1u << 20) + 1, 1u << 20, false, "large past not due" },
        { false, 1u << 20, 1u << 20, false, "large equal zero period no tick" },

        // Host zero-period injects elapsedThreshold as 0 (TickEffects path).
        { false, 0, 0, false, "host zero period threshold 0" },
        { false, 7, 0, false, "host zero period elapsed ignore" },
        { true, 0, 0, false, "nonzero period but zero threshold not due" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldTickEffect(c.tickPeriodNonzero, c.elapsedTickCount, c.elapsedThreshold);
        const bool inlineF = inlineShouldTickEffect(c.tickPeriodNonzero, c.elapsedTickCount, c.elapsedThreshold);
        const bool wantPin = c.tickPeriodNonzero && c.elapsedTickCount < c.elapsedThreshold;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldTickEffect dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldTickEffect == pin formula tickPeriodNonzero && elapsedTickCount < elapsedThreshold") && ok;
    }

    // Pin composition: tick due iff period nonzero and elapsed < threshold.
    ok = expect(ShouldTickEffect(true, 0, 1), "period nonzero elapsed < threshold must tick") && ok;
    ok = expect(!ShouldTickEffect(true, 1, 1), "period nonzero elapsed == threshold must not tick") && ok;
    ok = expect(!ShouldTickEffect(true, 2, 1), "period nonzero elapsed > threshold must not tick") && ok;
    ok = expect(!ShouldTickEffect(false, 0, 1), "period zero must not tick") && ok;

    // Dense compose: full 2¹ × equality-class space (equal / less / greater).
    const struct
    {
        uint32      elapsed;
        uint32      threshold;
        const char* label;
    } pairs[] = {
        { 1, 1, "equal" },
        { 0, 1, "less" },
        { 2, 1, "greater" },
        { 0, 0, "zero-equal" },
        { 0, 5, "zero-less" },
        { 5, 0, "pos-greater-zero-thresh" },
    };
    for (const bool p : { false, true })
    {
        for (const auto& pair : pairs)
        {
            const bool got  = ShouldTickEffect(p, pair.elapsed, pair.threshold);
            const bool want = p && pair.elapsed < pair.threshold;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldTickEffect(p, pair.elapsed, pair.threshold),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Explicit polarity: never tick when period zero; when nonzero tick
    // iff elapsedTickCount < elapsedThreshold.
    for (const bool p : { false, true })
    {
        for (const auto& pair : pairs)
        {
            const bool got = ShouldTickEffect(p, pair.elapsed, pair.threshold);
            ok             = expect(!(!p && got), "polarity: period zero must not tick") && ok;
            if (p)
            {
                ok = expect(got == (pair.elapsed < pair.threshold),
                            "polarity: period nonzero tick == elapsed < threshold") &&
                     ok;
            }
        }
    }

    // Host-style inject: TickEffects path semantics.
    const struct
    {
        bool        tickPeriodNonzero;
        uint32      elapsedTickCount;
        uint32      elapsedThreshold;
        bool        wantDue;
        const char* label;
    } composeCases[] = {
        { true, 0, 1, true, "first period due" },
        { true, 1, 2, true, "second period due" },
        { true, 2, 2, false, "at threshold not due" },
        { true, 3, 2, false, "past threshold not due" },
        { false, 0, 0, false, "zero period host threshold 0" },
        { false, 0, 5, false, "zero period never due" },
        { true, 0, 0, false, "nonzero period zero threshold not due" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldTickEffect(c.tickPeriodNonzero, c.elapsedTickCount, c.elapsedThreshold);
        ok             = expect(got == c.wantDue, c.label) && ok;
        ok             = expect(got == (c.tickPeriodNonzero && c.elapsedTickCount < c.elapsedThreshold),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineShouldTickEffect(c.tickPeriodNonzero, c.elapsedTickCount, c.elapsedThreshold),
                    "compose free == inline") &&
             ok;
    }

    // Production TickEffects path semantics.
    ok = expect(ShouldTickEffect(true, 0, 1), "TickEffects first period → tick path") && ok;
    ok = expect(!ShouldTickEffect(true, 1, 1), "TickEffects at threshold → skip path") && ok;
    ok = expect(!ShouldTickEffect(false, 0, 5), "TickEffects zero period → skip path") && ok;

    // Sibling dual-wire ShouldExpireEffect is orthogonal.
    ok = expect(ShouldExpireEffect(true, 100, 100), "residual expire at equality still holds") && ok;
    for (const bool p : { false, true })
    {
        for (const uint32 elapsed : { uint32{ 0 }, uint32{ 1 }, uint32{ 2 } })
        {
            const uint32 threshold = 1;
            const bool   got       = ShouldTickEffect(p, elapsed, threshold);
            const bool   want      = p && elapsed < threshold;
            ok                     = expect(got == want, "tick vs expire orthogonal") && ok;
            ok                     = expect(ShouldExpireEffect(true, 100, 100),
                        "expire residual under tick compose") &&
                 ok;
        }
    }

    // Explicit dual-wire poles: free == inline == pin across dense grid.
    for (const bool p : { false, true })
    {
        for (const uint32 elapsed : { uint32{ 0 }, uint32{ 1 }, uint32{ 2 }, uint32{ 5 }, uint32{ 100 } })
        {
            for (const uint32 threshold : { uint32{ 0 }, uint32{ 1 }, uint32{ 5 }, uint32{ 100 } })
            {
                const bool got  = ShouldTickEffect(p, elapsed, threshold);
                const bool want = p && elapsed < threshold;
                ok              = expect(got == want, "host inject dual-wire pin") && ok;
                ok              = expect(got == inlineShouldTickEffect(p, elapsed, threshold),
                            "host inject free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
