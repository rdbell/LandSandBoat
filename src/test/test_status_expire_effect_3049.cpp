#include "test_status_expire_effect_3049.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldExpireEffect 3049 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CheckEffectsExpiry formula for dual-wire cross-check (slice 3049):
//   durationNonzero && expiryTime <= tickTime
auto inlineShouldExpireEffect(const bool durationNonzero, const int64 expiryTime, const int64 tickTime) -> bool
{
    return durationNonzero && expiryTime <= tickTime;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldExpireEffect
// (duration != 0 && start+duration <= tick; slice 3049). Dense durationNonzero
// × expiry vs tick (equal / before / after).
auto runStatusExpireEffect3049SelfTests() -> bool
{
    using statuseffecthelpers::ShouldExpireEffect;
    using statuseffecthelpers::ShouldTickEffect;

    bool ok = true;

    // Residual 1366 pins still hold under dual-wire.
    ok = expect(ShouldExpireEffect(true, 100, 100), "residual expire at equality") && ok;
    ok = expect(ShouldExpireEffect(true, 99, 100), "residual expire past") && ok;
    ok = expect(!ShouldExpireEffect(true, 101, 100), "residual not expire future") && ok;
    ok = expect(!ShouldExpireEffect(false, 0, 100), "residual permanent no expire") && ok;

    const struct
    {
        bool        durationNonzero;
        int64       expiryTime;
        int64       tickTime;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual 1366 poles.
        { true, 100, 100, true, "expire at equality" },
        { true, 99, 100, true, "expire past (expiry < tick)" },
        { true, 101, 100, false, "not expire future (expiry > tick)" },
        { false, 0, 100, false, "permanent duration zero no expire" },

        // durationNonzero false × equal / before / after.
        { false, 100, 100, false, "zero duration equal times no expire" },
        { false, 99, 100, false, "zero duration past no expire" },
        { false, 101, 100, false, "zero duration future no expire" },

        // Zero tick anchors.
        { true, 0, 0, true, "zero equal expires when duration nonzero" },
        { true, -1, 0, true, "negative expiry past zero tick" },
        { true, 1, 0, false, "positive expiry after zero tick" },
        { false, 0, 0, false, "zero duration zero times no expire" },

        // Large / extreme host-normalized units.
        { true, 1LL << 40, 1LL << 40, true, "large equal expires" },
        { true, (1LL << 40) - 1, 1LL << 40, true, "large past expires" },
        { true, (1LL << 40) + 1, 1LL << 40, false, "large future keeps" },
        { false, 1LL << 40, 1LL << 40, false, "large equal zero duration keeps" },

        // Negative both sides.
        { true, -100, -50, true, "both negative past expires" },
        { true, -50, -100, false, "both negative future keeps" },
        { true, -50, -50, true, "both negative equal expires" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        const bool inlineF = inlineShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        const bool wantPin = c.durationNonzero && c.expiryTime <= c.tickTime;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldExpireEffect dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldExpireEffect == pin formula durationNonzero && expiryTime <= tickTime") && ok;
    }

    // Pin composition: expire iff duration nonzero and expiry <= tick.
    ok = expect(ShouldExpireEffect(true, 100, 100), "duration nonzero equal must expire") && ok;
    ok = expect(ShouldExpireEffect(true, 50, 100), "duration nonzero past must expire") && ok;
    ok = expect(!ShouldExpireEffect(true, 150, 100), "duration nonzero future must not expire") && ok;
    ok = expect(!ShouldExpireEffect(false, 50, 100), "duration zero must not expire") && ok;

    // Dense compose: full 2¹ × equality-class space (equal / before / after).
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
    };
    for (const bool d : { false, true })
    {
        for (const auto& p : pairs)
        {
            const bool got  = ShouldExpireEffect(d, p.expiry, p.tick);
            const bool want = d && p.expiry <= p.tick;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldExpireEffect(d, p.expiry, p.tick),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Explicit polarity: never expire when duration zero; when nonzero expire
    // iff expiryTime <= tickTime.
    for (const bool d : { false, true })
    {
        for (const auto& p : pairs)
        {
            const bool got = ShouldExpireEffect(d, p.expiry, p.tick);
            ok             = expect(!(!d && got), "polarity: duration zero must not expire") && ok;
            if (d)
            {
                ok = expect(got == (p.expiry <= p.tick),
                            "polarity: duration nonzero expire == expiry <= tick") &&
                     ok;
            }
        }
    }

    // Host-style inject: CheckEffectsExpiry path semantics.
    const struct
    {
        bool        durationNonzero;
        int64       expiryTime;
        int64       tickTime;
        bool        wantExpire;
        const char* label;
    } composeCases[] = {
        { true, 1000, 1000, true, "timed effect at exact expiry" },
        { true, 999, 1000, true, "timed effect past expiry" },
        { true, 1001, 1000, false, "timed effect not yet expired" },
        { false, 1000, 1000, false, "permanent effect never expires" },
        { false, 0, 1000, false, "permanent zero expiry never expires" },
        { true, 0, 1, true, "timed zero start+duration already due" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime);
        ok             = expect(got == c.wantExpire, c.label) && ok;
        ok             = expect(got == (c.durationNonzero && c.expiryTime <= c.tickTime),
                    "compose free == pin formula") &&
             ok;
        ok = expect(got == inlineShouldExpireEffect(c.durationNonzero, c.expiryTime, c.tickTime),
                    "compose free == inline") &&
             ok;
    }

    // Production CheckEffectsExpiry path semantics.
    ok = expect(ShouldExpireEffect(true, 50, 100), "CheckEffectsExpiry timed past → remove path") && ok;
    ok = expect(!ShouldExpireEffect(true, 150, 100), "CheckEffectsExpiry timed future → keep path") && ok;
    ok = expect(!ShouldExpireEffect(false, 50, 100), "CheckEffectsExpiry permanent → keep path") && ok;

    // Sibling residual ShouldTickEffect is orthogonal.
    ok = expect(ShouldTickEffect(true, 0, 1), "residual tick due still holds") && ok;
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

    // Explicit dual-wire poles: free == inline == pin across dense grid.
    for (const bool d : { false, true })
    {
        for (const int64 expiry : { int64{ -10 }, int64{ 0 }, int64{ 50 }, int64{ 100 }, int64{ 150 } })
        {
            for (const int64 tick : { int64{ 0 }, int64{ 100 } })
            {
                const bool got  = ShouldExpireEffect(d, expiry, tick);
                const bool want = d && expiry <= tick;
                ok              = expect(got == want, "host inject dual-wire pin") && ok;
                ok              = expect(got == inlineShouldExpireEffect(d, expiry, tick),
                            "host inject free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
