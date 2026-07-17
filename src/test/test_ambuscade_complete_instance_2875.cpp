#include "test_ambuscade_complete_instance_2875.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade complete-instance 2875 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceTimeUpdate formula for dual-wire checks:
//   if not mobsStillAlive then instance:complete() end
// anyMobAlive is the host inject for the per-mob isAlive loop (mobsStillAlive).
auto inlineShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldCompleteInstance
// (Lua ambuscade instance onInstanceTimeUpdate complete gate).
auto runAmbuscadeCompleteInstance2875SelfTests() -> bool
{
    using ambuscadehelpers::ShouldCompleteInstance;

    bool ok = true;

    // Truth table for ShouldCompleteInstance (!anyMobAlive).
    ok = expect(ShouldCompleteInstance(false), "no mobs alive → complete") && ok;
    ok = expect(!ShouldCompleteInstance(true), "any mob alive → do not complete") && ok;

    // Dual-wire matches inline formula across a small table.
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

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
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

        ok = expect(got == c.wantComplete, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
