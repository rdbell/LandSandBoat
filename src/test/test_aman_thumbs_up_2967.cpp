#include "test_aman_thumbs_up_2967.h"

#include "map/aman_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aman CanThumbsUp 2967 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAMANContainer::canThumbsUp pure half for dual-wire cross-check
// (slice 2967):
//   cooldownExpired
// Production resolves cooldown via getCharVar(thumbsUpCooldownVar) == 0.
auto inlineCanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

} // namespace

// Pure dual-wire expansion for amanhelpers::CanThumbsUp
// (thumbs-up cooldown-expired identity gate; slice 2967).
auto runAmanThumbsUp2967SelfTests() -> bool
{
    using amanhelpers::CanThumbsUp;

    bool ok = true;

    // Residual 0698 pins still hold under dual-wire.
    ok = expect(CanThumbsUp(true), "residual cooldownExpired true permits") && ok;
    ok = expect(!CanThumbsUp(false), "residual cooldownExpired false rejects") && ok;

    const struct
    {
        bool        cooldownExpired;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "cooldown expired permits thumbs-up" },
        { false, false, "cooldown active rejects thumbs-up" },

        // Residual 0698 re-pins.
        { true, true, "residual true" },
        { false, false, "residual false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanThumbsUp(c.cooldownExpired);
        const bool inlineF = inlineCanThumbsUp(c.cooldownExpired);
        const bool wantPin = c.cooldownExpired;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanThumbsUp dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanThumbsUp == pin formula (identity)") && ok;
    }

    // Pin composition: free function is identity on cooldownExpired only.
    ok = expect(CanThumbsUp(true), "true must permit") && ok;
    ok = expect(!CanThumbsUp(false), "false must reject") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool cooldownExpired : { false, true })
    {
        const bool got  = CanThumbsUp(cooldownExpired);
        const bool want = cooldownExpired;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineCanThumbsUp(cooldownExpired), "compose free == inline") && ok;
    }

    // Host-style inject poles: getCharVar("[ASSIST][ThumbsUp]Cooldown") == 0.
    // (Live CAMANContainer::canThumbsUp / 0x0b7 assist / 0x01b job_info residual.)
    const struct
    {
        bool charvarZero; // getCharVar(thumbsUpCooldownVar) == 0
        bool want;
    } hostPoles[] = {
        { true, true },
        { false, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool cooldownExpired = h.charvarZero;
        const bool inject          = cooldownExpired;
        ok                         = expect(CanThumbsUp(cooldownExpired) == inject, "host inject dual-wire identity") && ok;
        ok                         = expect(CanThumbsUp(cooldownExpired) == inlineCanThumbsUp(cooldownExpired),
                    "host inject free == inline") &&
             ok;
        ok = expect(CanThumbsUp(cooldownExpired) == h.want, "host inject want pin") && ok;
    }

    // Production path semantics:
    //   CanThumbsUp(getCharVar(thumbsUpCooldownVar) == 0)
    // when true  → GiveThumbsUp may forward / job_info can_thumbs_up_mentor set
    // when false → ThumbsUpCooldown message path
    ok = expect(CanThumbsUp(true), "charvar == 0 → permit path") && ok;
    ok = expect(!CanThumbsUp(false), "charvar != 0 → reject path") && ok;

    // Assist-channel SelectAction style residual: mute gate is host-owned
    // (!muted && CanThumbsUp(...)); pure half is identity only.
    ok = expect((!false && CanThumbsUp(true)), "unmuted + expired opens path") && ok;
    ok = expect(!(!true && CanThumbsUp(true)), "muted closes path even when expired") && ok;
    ok = expect(!(!false && CanThumbsUp(false)), "active cooldown closes path when unmuted") && ok;

    return ok;
}
