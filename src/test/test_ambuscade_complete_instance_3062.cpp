#include "test_ambuscade_complete_instance_3062.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade complete-instance 3062 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceTimeUpdate formula for dual-wire checks (slice 3062):
//   if not mobsStillAlive then instance:complete() end
// anyMobAlive is the host inject for the per-mob isAlive loop (mobsStillAlive).
auto inlineShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

// Compact dual-wire pin matching C++ capacity formula:
//   !anyMobAlive
auto pinShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldCompleteInstance
// (Lua ambuscade instance onInstanceTimeUpdate complete gate; slice 3062).
auto runAmbuscadeCompleteInstance3062SelfTests() -> bool
{
    using ambuscadehelpers::ShouldCompleteInstance;

    bool ok = true;

    // Residual 1089 / 2875 pins still hold under dual-wire.
    ok = expect(ShouldCompleteInstance(false), "residual: no mobs alive → complete") && ok;
    ok = expect(!ShouldCompleteInstance(true), "residual: any mob alive → do not complete") && ok;

    // Truth table for ShouldCompleteInstance (!anyMobAlive) — dense 2¹.
    ok = expect(ShouldCompleteInstance(false), "no mobs alive → complete") && ok;
    ok = expect(!ShouldCompleteInstance(true), "any mob alive → do not complete") && ok;

    // Dual-wire matches inline + pin formulas across the full bool domain.
    const struct
    {
        bool        anyMobAlive;
        bool        want;
        const char* label;
    } cases[] = {
        { false, true, "table no mobs alive" },
        { true, false, "table any mob alive" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldCompleteInstance(c.anyMobAlive);
        const bool inlineGot = inlineShouldCompleteInstance(c.anyMobAlive);
        const bool pinGot    = pinShouldCompleteInstance(c.anyMobAlive);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == !c.anyMobAlive, "dual-wire free == !anyMobAlive") && ok;
    }

    // Host compose: anyMobAlive is OR of per-mob isAlive (mobsStillAlive).
    // Empty mob list → anyMobAlive false → complete.
    // At least one alive → do not complete.
    const struct
    {
        bool        mob0Alive;
        bool        mob1Alive;
        bool        wantComplete;
        const char* label;
    } composeCases[] = {
        { false, false, true, "compose no mobs alive" },
        { true, false, false, "compose first alive" },
        { false, true, false, "compose second alive" },
        { true, true, false, "compose both alive" },
    };

    for (const auto& c : composeCases)
    {
        const bool anyAlive  = c.mob0Alive || c.mob1Alive;
        const bool got       = ShouldCompleteInstance(anyAlive);
        const bool inlineGot = inlineShouldCompleteInstance(anyAlive);
        const bool pinGot    = pinShouldCompleteInstance(anyAlive);

        ok = expect(got == c.wantComplete, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == !anyAlive, "compose dual-wire free == !anyAlive") && ok;
    }

    // Dense 2¹ host-style poles: free == inline == pin for full anyMobAlive space.
    for (const bool anyMobAlive : { false, true })
    {
        const bool got = ShouldCompleteInstance(anyMobAlive);
        ok             = expect(got == pinShouldCompleteInstance(anyMobAlive), "dense free == pin") && ok;
        ok             = expect(got == inlineShouldCompleteInstance(anyMobAlive), "dense free == inline") && ok;
        ok             = expect(got == !anyMobAlive, "dense free == !anyMobAlive") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(ShouldCompleteInstance(false) == true, "compose no mobs") && ok;
    ok = expect(ShouldCompleteInstance(true) == false, "compose any alive") && ok;

    return ok;
}
