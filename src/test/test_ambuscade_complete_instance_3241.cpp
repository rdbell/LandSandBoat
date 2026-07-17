#include "test_ambuscade_complete_instance_3241.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade complete-instance 3241 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceTimeUpdate formula for dual-wire checks (slice 3241):
//   if not mobsStillAlive then instance:complete() end
// anyMobAlive is the host inject for the per-mob isAlive loop (mobsStillAlive).
// Direct return only — same formula as capacity body.
auto inlineShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

// Compact dual-wire pin matching C++ capacity formula / Go pinShouldCompleteInstance3241:
//   return !anyMobAlive
auto pinShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldCompleteInstance
// (Lua ambuscade instance onInstanceTimeUpdate complete gate; OmegaXI
// internal/ambuscade; dedicated slice 3241 expand residual 2875; prior
// dedicated 3062). Formula unchanged:
//
//   ShouldCompleteInstance(anyMobAlive) = !anyMobAlive
//
// Coverage:
//   - free == inline == pin == (!anyMobAlive)
//   - residual 2875 / 3062 / 1089 pins still hold
//   - residual poles true/false
//   - dense 2¹ full anyMobAlive domain
//   - host isAlive OR compose poles
auto runAmbuscadeCompleteInstance3241SelfTests() -> bool
{
    using ambuscadehelpers::ShouldCompleteInstance;

    bool ok = true;

    // Residual 1089 / 2875 / prior dedicated 3062 pins still hold under dual-wire.
    ok = expect(ShouldCompleteInstance(false), "residual: no mobs alive → complete") && ok;
    ok = expect(!ShouldCompleteInstance(true), "residual: any mob alive → do not complete") && ok;

    // Residual poles true/false: free == inline == pin (direct return).
    const struct
    {
        bool        anyMobAlive;
        bool        want;
        const char* label;
    } poles[] = {
        { false, true, "residual no mobs alive → complete" },
        { true, false, "residual any mob alive → do not complete" },
        { false, true, "pole anyMobAlive=false → complete" },
        { true, false, "pole anyMobAlive=true → do not complete" },
        { false, true, "host empty/all-dead → complete path" },
        { true, false, "host any alive → leave running" },
    };

    for (const auto& p : poles)
    {
        const bool got       = ShouldCompleteInstance(p.anyMobAlive);
        const bool inlineGot = inlineShouldCompleteInstance(p.anyMobAlive);
        const bool pinGot    = pinShouldCompleteInstance(p.anyMobAlive);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineGot && got == pinGot, "dual-wire free == inline == pin") && ok;
        ok = expect(got == !p.anyMobAlive, "dual-wire free == !anyMobAlive") && ok;
    }

    // Dense 2¹: full bool domain for anyMobAlive; free == inline == pin.
    for (const bool anyMobAlive : { false, true })
    {
        const bool got       = ShouldCompleteInstance(anyMobAlive);
        const bool inlineGot = inlineShouldCompleteInstance(anyMobAlive);
        const bool pinGot    = pinShouldCompleteInstance(anyMobAlive);

        ok = expect(got == !anyMobAlive, "dense free == !anyMobAlive") && ok;
        ok = expect(got == inlineGot && got == pinGot, "dense free == inline == pin") && ok;
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
        ok = expect(got == inlineGot && got == pinGot, "compose free == inline == pin") && ok;
        ok = expect(got == !anyAlive, "compose free == !anyAlive") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(ShouldCompleteInstance(false) == true, "compose no mobs") && ok;
    ok = expect(ShouldCompleteInstance(true) == false, "compose any alive") && ok;
    ok = expect(ShouldCompleteInstance(false) == pinShouldCompleteInstance(false), "free == pin no mobs") && ok;
    ok = expect(ShouldCompleteInstance(true) == pinShouldCompleteInstance(true), "free == pin any alive") && ok;

    // Production path semantics (host inject model for onInstanceTimeUpdate).
    ok = expect(ShouldCompleteInstance(false) == inlineShouldCompleteInstance(false), "host complete path free == inline") && ok;
    ok = expect(ShouldCompleteInstance(true) == inlineShouldCompleteInstance(true), "host leave-running free == inline") && ok;

    return ok;
}
