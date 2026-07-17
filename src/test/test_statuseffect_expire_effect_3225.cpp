#include "test_statuseffect_expire_effect_3225.h"

#include "map/status_effect_capacity.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "statuseffect ShouldExpireEffect 3225 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckEffectsExpiry formula for dual-wire cross-check
// (dedicated slice 3225 expand residual 3049):
//   durationNonzero && expiryTime <= tickTime
// Direct return (not if/else) to match capacity free function form.
auto inlineShouldExpireEffect(const bool durationNonzero, const int64 expiryTime, const int64 tickTime) -> bool
{
    return durationNonzero && expiryTime <= tickTime;
}

// Compact dual-wire pin matching Go pinShouldExpireEffect3225 / C++ capacity
// form (formula unchanged from 1366 / 3049). Direct return of the AND-compare:
//   durationNonzero && expiryTime <= tickTime
auto pinShouldExpireEffect(const bool durationNonzero, const int64 expiryTime, const int64 tickTime) -> bool
{
    return durationNonzero && expiryTime <= tickTime;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldExpireEffect
// (duration != 0 && start+duration <= tick; OmegaXI internal/statuseffect;
// dedicated slice 3225 expand residual 3049 / pure 1366).
//
// Coverage:
//   - free == inline == pin (direct return form)
//   - residual poles: duration zero never expires; expiry==tick; expiry<tick;
//     expiry>tick
//   - dense edges: int64 0, negative, max-ish values; duration true/false
//     crossed with equality boundaries
// Formula unchanged — not registered in CMake/main.
auto runStatuseffectExpireEffect3225SelfTests() -> bool
{
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1366 / 3049 pins still hold under dedicated dual-wire.
    ok = expect(ShouldExpireEffect(true, 100, 100), "residual: expire at equality (expiry==tick)") && ok;
    ok = expect(ShouldExpireEffect(true, 99, 100), "residual: expire past (expiry<tick)") && ok;
    ok = expect(!ShouldExpireEffect(true, 101, 100), "residual: not expire future (expiry>tick)") && ok;
    ok = expect(!ShouldExpireEffect(false, 0, 100), "residual: duration zero never expires") && ok;

    // Residual poles free == inline == pin.
    const struct
    {
        bool        durationNonzero;
        int64       expiryTime;
        int64       tickTime;
        bool        want;
        const char* label;
    } poles[] = {
        { true, 100, 100, true, "expiry==tick expires when duration nonzero" },
        { true, 99, 100, true, "expiry<tick expires when duration nonzero" },
        { true, 101, 100, false, "expiry>tick keeps when duration nonzero" },
        { false, 0, 100, false, "duration zero never expires (zero expiry)" },
        { false, 100, 100, false, "duration zero never expires (equal times)" },
        { false, 99, 100, false, "duration zero never expires (past)" },
        { false, 101, 100, false, "duration zero never expires (future)" },
    };
    for (const auto& p : poles)
    {
        const bool got     = ShouldExpireEffect(p.durationNonzero, p.expiryTime, p.tickTime);
        const bool inlineF = inlineShouldExpireEffect(p.durationNonzero, p.expiryTime, p.tickTime);
        const bool pinF    = pinShouldExpireEffect(p.durationNonzero, p.expiryTime, p.tickTime);
        const bool wantPin = p.durationNonzero && p.expiryTime <= p.tickTime;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF && got == pinF && got == wantPin, "pole free==inline==pin") && ok;
    }

    // Dense table: duration true/false × equality boundaries + edges.
    const int64 nearMax = (int64{ 1 } << 62) - 1;
    const struct
    {
        bool        durationNonzero;
        int64       expiryTime;
        int64       tickTime;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual 1366 / 3049 poles.
        { true, 100, 100, true, "expire at equality" },
        { true, 99, 100, true, "expire past (expiry < tick)" },
        { true, 101, 100, false, "not expire future (expiry > tick)" },
        { false, 0, 100, false, "permanent duration zero no expire" },

        // durationNonzero false × equal / before / after.
        { false, 100, 100, false, "zero duration equal times no expire" },
        { false, 99, 100, false, "zero duration past no expire" },
        { false, 101, 100, false, "zero duration future no expire" },

        // Zero / negative edges.
        { true, 0, 0, true, "zero equal expires when duration nonzero" },
        { true, -1, 0, true, "negative expiry past zero tick" },
        { true, 1, 0, false, "positive expiry after zero tick" },
        { false, 0, 0, false, "zero duration zero times no expire" },
        { true, -100, -50, true, "both negative past expires" },
        { true, -50, -100, false, "both negative future keeps" },
        { true, -50, -50, true, "both negative equal expires" },
        { false, -50, -50, false, "both negative equal zero duration keeps" },
        { true, 0, -1, false, "zero expiry after negative tick keeps" },
        { true, -1, -1, true, "both -1 equal expires" },

        // Large / max-ish host-normalized units.
        { true, int64{ 1 } << 40, int64{ 1 } << 40, true, "large equal expires" },
        { true, (int64{ 1 } << 40) - 1, int64{ 1 } << 40, true, "large past expires" },
        { true, (int64{ 1 } << 40) + 1, int64{ 1 } << 40, false, "large future keeps" },
        { false, int64{ 1 } << 40, int64{ 1 } << 40, false, "large equal zero duration keeps" },
        { true, nearMax, nearMax, true, "near-max equal expires" },
        { true, nearMax - 1, nearMax, true, "near-max past expires" },
        { true, nearMax, nearMax - 1, false, "near-max future keeps" },
        { false, nearMax, nearMax, false, "near-max equal zero duration keeps" },
        { true, -(int64{ 1 } << 40), -(int64{ 1 } << 40), true, "large-negative equal expires" },
        { true, (-(int64{ 1 } << 40)) - 1, -(int64{ 1 } << 40), true, "large-negative past expires" },
        { true, (-(int64{ 1 } << 40)) + 1, -(int64{ 1 } << 40), false, "large-negative future keeps" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        const bool inlineF = inlineShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        const bool pinF    = pinShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        const bool wantPin = c.durationNonzero && c.expiryTime <= c.tickTime;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dense free==inline") && ok;
        ok = expect(got == pinF, "dense free==pin") && ok;
        ok = expect(got == wantPin, "dense free==wantPin") && ok;
    }

    // Dense compose: duration true/false × equality-class space.
    const struct
    {
        int64       expiry;
        int64       tick;
        const char* label;
    } pairs[] = {
        { 100, 100, "equal" },
        { 99, 100, "before" },
        { 101, 100, "after" },
        { 0, 0, "zero-equal" },
        { -1, 0, "neg-before" },
        { 1, 0, "pos-after" },
        { -100, -50, "both-neg-before" },
        { -50, -100, "both-neg-after" },
        { -50, -50, "both-neg-equal" },
        { int64{ 1 } << 40, int64{ 1 } << 40, "large-equal" },
        { (int64{ 1 } << 40) - 1, int64{ 1 } << 40, "large-before" },
        { (int64{ 1 } << 40) + 1, int64{ 1 } << 40, "large-after" },
        { nearMax, nearMax, "near-max-equal" },
        { nearMax - 1, nearMax, "near-max-before" },
        { nearMax, nearMax - 1, "near-max-after" },
    };
    for (const bool d : { false, true })
    {
        for (const auto& p : pairs)
        {
            const bool got     = ShouldExpireEffect(d, p.expiry, p.tick);
            const bool inlineF = inlineShouldExpireEffect(d, p.expiry, p.tick);
            const bool pinF    = pinShouldExpireEffect(d, p.expiry, p.tick);
            const bool want    = d && p.expiry <= p.tick;
            ok                 = expect(got == want, "compose free==pin formula") && ok;
            ok                 = expect(got == inlineF && got == pinF, "compose free==inline==pin") && ok;
            ok                 = expect(!(!d && got), "polarity: duration zero must not expire") && ok;
            if (d)
            {
                ok = expect(got == (p.expiry <= p.tick),
                            "polarity: duration nonzero expire == expiry <= tick") &&
                     ok;
            }
        }
    }

    // Explicit dual-wire poles: free == inline == pin across dense grid.
    const int64 expiries[] = {
        -(int64{ 1 } << 40), -10, -1, 0, 50, 100, 150, int64{ 1 } << 40, nearMax,
    };
    const int64 ticks[] = {
        -(int64{ 1 } << 40), -1, 0, 100, int64{ 1 } << 40, nearMax,
    };
    for (const bool d : { false, true })
    {
        for (const int64 expiry : expiries)
        {
            for (const int64 tick : ticks)
            {
                const bool got     = ShouldExpireEffect(d, expiry, tick);
                const bool inlineF = inlineShouldExpireEffect(d, expiry, tick);
                const bool pinF    = pinShouldExpireEffect(d, expiry, tick);
                const bool want    = d && expiry <= tick;
                ok                 = expect(got == want, "dense grid free==want") && ok;
                ok                 = expect(got == inlineF && got == pinF, "dense grid free==inline==pin") && ok;
            }
        }
    }

    // Host-style CheckEffectsExpiry inject path semantics.
    ok = expect(ShouldExpireEffect(true, 50, 100) && pinShouldExpireEffect(true, 50, 100),
                "CheckEffectsExpiry timed past → remove path") &&
         ok;
    ok = expect(!ShouldExpireEffect(true, 150, 100) && !pinShouldExpireEffect(true, 150, 100),
                "CheckEffectsExpiry timed future → keep path") &&
         ok;
    ok = expect(!ShouldExpireEffect(false, 50, 100) && !pinShouldExpireEffect(false, 50, 100),
                "CheckEffectsExpiry permanent → keep path") &&
         ok;

    // Sibling residual ShouldTickEffect is orthogonal (3069 left alone).
    ok = expect(ShouldTickEffect(true, 0, 1), "residual tick due still holds under dedicated expire dual-wire") && ok;
    for (const bool d : { false, true })
    {
        for (const int64 expiry : { int64{ 99 }, int64{ 100 }, int64{ 101 } })
        {
            const int64 tick = 100;
            const bool  got  = ShouldExpireEffect(d, expiry, tick);
            const bool  want = d && expiry <= tick;
            ok               = expect(got == want, "expiry vs tick-due orthogonal") && ok;
            ok               = expect(ShouldTickEffect(true, 0, 1),
                        "tick-due residual under expiry compose") &&
                 ok;
        }
    }

    // Numeric limits edge (max-ish int64).
    const int64 intMax = std::numeric_limits<int64>::max();
    ok                 = expect(ShouldExpireEffect(true, intMax, intMax), "int64 max equal expires") && ok;
    ok                 = expect(ShouldExpireEffect(true, intMax - 1, intMax), "int64 max past expires") && ok;
    ok                 = expect(!ShouldExpireEffect(true, intMax, intMax - 1), "int64 max future keeps") && ok;
    ok                 = expect(!ShouldExpireEffect(false, intMax, intMax), "int64 max equal zero duration keeps") && ok;
    ok                 = expect(ShouldExpireEffect(true, intMax, intMax) ==
                    pinShouldExpireEffect(true, intMax, intMax),
                "int64 max free==pin") &&
         ok;

    return ok;
}
