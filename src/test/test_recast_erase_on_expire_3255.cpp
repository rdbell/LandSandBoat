#include "test_recast_erase_on_expire_3255.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldEraseOnExpire 3255 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Check erase-vs-retain gate for dual-wire cross-check (slice 3255):
//   !isAbility
auto inlineShouldEraseOnExpire(const bool isAbility) -> bool
{
    return !isAbility;
}

// Pin form of the same formula (free == inline == pin).
auto pinShouldEraseOnExpire(const bool isAbility) -> bool
{
    return !isAbility;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldEraseOnExpire
// (magic erase vs ability zero-retain Check gate; slice 3255 of residual
// 3070 / pure 1370). Residual suite: test_recast_erase_on_expire_3070.
auto runRecastEraseOnExpire3255SelfTests() -> bool
{
    using recasthelpers::ShouldEraseOnExpire;

    bool ok = true;

    // Residual 3070 / 1370 pins still hold under dual-wire.
    ok = expect(ShouldEraseOnExpire(false), "residual magic erase") && ok;
    ok = expect(!ShouldEraseOnExpire(true), "residual ability retain (no erase)") && ok;

    const struct
    {
        bool        isAbility;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "magic/item/loot erase" },
        { true, false, "ability retain zero" },

        // Residual 3070 / 1370 pins restated.
        { false, true, "residual 3070/1370 erase non-ability" },
        { true, false, "residual 3070/1370 retain ability" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldEraseOnExpire(c.isAbility);
        const bool pinF    = pinShouldEraseOnExpire(c.isAbility);
        const bool inlineF = inlineShouldEraseOnExpire(c.isAbility);
        const bool wantPin = !c.isAbility;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pinF, "ShouldEraseOnExpire dual-wire == pin formula") && ok;
        ok = expect(got == inlineF, "ShouldEraseOnExpire dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldEraseOnExpire == pin formula !isAbility") && ok;
    }

    // Pin composition: invert only.
    ok = expect(ShouldEraseOnExpire(false), "non-ability must erase") && ok;
    ok = expect(!ShouldEraseOnExpire(true), "ability must not erase") && ok;

    // Dense compose: both bool inputs (full domain 2^1); free == inline == pin.
    for (const bool isAbility : { false, true })
    {
        const bool got  = ShouldEraseOnExpire(isAbility);
        const bool want = !isAbility;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldEraseOnExpire(isAbility), "compose free == inline") && ok;
        ok              = expect(got == pinShouldEraseOnExpire(isAbility), "compose free == pin") && ok;
    }

    // Host-style inject poles: type == RECAST_ABILITY / not as bools.
    // (Live container Check erase/retain is residual 1370 / recast_container tests.)
    for (const bool isAbility : { true, false })
    {
        const bool inject = isAbility; // mirrors type == RECAST_ABILITY
        ok                = expect(ShouldEraseOnExpire(inject) == !inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldEraseOnExpire(inject) == inlineShouldEraseOnExpire(inject),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldEraseOnExpire(inject) == pinShouldEraseOnExpire(inject),
                    "host inject free == pin") &&
             ok;
    }

    return ok;
}
