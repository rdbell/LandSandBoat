#include "test_aman_thumbs_up_3675.h"

#include "map/aman_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aman CanThumbsUp 3675 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAMANContainer::canThumbsUp pure half for dual-wire cross-check
// (dedicated 3675 expand residual 2967):
//   cooldownExpired
// Production resolves cooldown via getCharVar(thumbsUpCooldownVar) == 0.
auto inlineCanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

// Compact dual-wire pin matching Go pinCanThumbsUp3675 / C++ capacity
// (direct return):
//   cooldownExpired
auto pinCanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

// Prior dedicated 3630 pin for independence cross-check.
auto pinCanThumbsUp3630(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

auto inlineCanThumbsUp3630(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

} // namespace

// Pure dual-wire expansion for amanhelpers::CanThumbsUp
// (thumbs-up cooldown-expired identity gate; OmegaXI internal/aman;
// dedicated slice 3675 expand residual 2967; prior dedicated 3630 / 3585 / 3541 / 3473 / 3419 / 3308).
//
// Coverage:
//   - cooldownExpired true → permit / false → reject
//   - free == inline == pin == pin3630 == cooldownExpired (direct return)
//   - residual 0698 / 2967 / prior dedicated 3150 / 3229 / 3278 / 3308 / 3419 / 3473 / 3541 / 3585 / 3630 pins still hold
//   - prior 3630 independence (free == 3630 inline/pin)
//   - dense 2¹ boolean space
auto runAmanThumbsUp3675SelfTests() -> bool
{
    using amanhelpers::CanThumbsUp;

    bool ok = true;

    // Residual 0698 / 2967 / 3150 / 3229 / 3278 / 3308 / 3419 / 3473 / 3541 / 3585 / 3630 pins still hold under dual-wire.
    ok = expect(CanThumbsUp(true), "residual cooldownExpired true permits") && ok;
    ok = expect(!CanThumbsUp(false), "residual cooldownExpired false rejects") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(CanThumbsUp(true), "eligible expired permits") && ok;
    ok = expect(!CanThumbsUp(false), "blocked active rejects") && ok;

    const struct
    {
        bool        cooldownExpired;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual poles true/false.
        { true, true, "cooldown expired permits thumbs-up" },
        { false, false, "cooldown active rejects thumbs-up" },

        // Residual 2967 re-pins.
        { true, true, "residual 2967 true" },
        { false, false, "residual 2967 false" },

        // Prior dedicated 3150 re-pins.
        { true, true, "prior 3150 true" },
        { false, false, "prior 3150 false" },

        // Prior dedicated 3229 re-pins.
        { true, true, "prior 3229 true" },
        { false, false, "prior 3229 false" },

        // Prior dedicated 3278 re-pins.
        { true, true, "prior 3278 true" },
        { false, false, "prior 3278 false" },

        // Prior dedicated 3308 re-pins.
        { true, true, "prior 3308 true" },
        { false, false, "prior 3308 false" },

        // Prior dedicated 3419 re-pins.
        { true, true, "prior 3419 true" },
        { false, false, "prior 3419 false" },

        // Prior dedicated 3473 re-pins.
        { true, true, "prior 3473 true" },
        { false, false, "prior 3473 false" },

        // Prior dedicated 3541 re-pins.
        { true, true, "prior 3541 true" },
        { false, false, "prior 3541 false" },

        // Prior dedicated 3585 re-pins.
        { true, true, "prior 3585 true" },
        { false, false, "prior 3585 false" },

        // Prior dedicated 3630 re-pins (independence).
        { true, true, "prior 3630 true" },
        { false, false, "prior 3630 false" },

        // Residual 0698 re-pins.
        { true, true, "residual 0698 true" },
        { false, false, "residual 0698 false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanThumbsUp(c.cooldownExpired);
        const bool inlineF = inlineCanThumbsUp(c.cooldownExpired);
        const bool pinGot  = pinCanThumbsUp(c.cooldownExpired);
        const bool pin3630 = pinCanThumbsUp3630(c.cooldownExpired);
        const bool wantPin = c.cooldownExpired;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanThumbsUp dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "CanThumbsUp dual-wire == pin") && ok;
        ok = expect(got == pin3630, "CanThumbsUp dual-wire == pin3630") && ok;
        ok = expect(got == wantPin, "CanThumbsUp == pin formula (identity)") && ok;
    }

    // Pin composition: free function is identity on cooldownExpired only.
    ok = expect(CanThumbsUp(true), "true must permit") && ok;
    ok = expect(!CanThumbsUp(false), "false must reject") && ok;
    ok = expect(CanThumbsUp(true) == pinCanThumbsUp(true), "free==pin true") && ok;
    ok = expect(CanThumbsUp(false) == pinCanThumbsUp(false), "free==pin false") && ok;
    ok = expect(CanThumbsUp(true) == inlineCanThumbsUp(true), "free==inline true") && ok;
    ok = expect(CanThumbsUp(false) == inlineCanThumbsUp(false), "free==inline false") && ok;
    ok = expect(CanThumbsUp(true) == pinCanThumbsUp3630(true), "free==pin3630 true") && ok;
    ok = expect(CanThumbsUp(false) == pinCanThumbsUp3630(false), "free==pin3630 false") && ok;

    // Explicit residual poles free == inline == pin == pin3630 for true / false.
    for (const bool pole : { false, true })
    {
        const bool got     = CanThumbsUp(pole);
        const bool inlineF = inlineCanThumbsUp(pole);
        const bool pinGot  = pinCanThumbsUp(pole);
        const bool pin3630 = pinCanThumbsUp3630(pole);
        ok                 = expect(got == pole, "pole free == identity") && ok;
        ok                 = expect(got == inlineF, "pole free == inline") && ok;
        ok                 = expect(got == pinGot, "pole free == pin") && ok;
        ok                 = expect(got == pin3630, "pole free == pin3630") && ok;
    }

    // Dense compose: full 2^1 boolean space — free == pin == inline == pin3630.
    for (const bool cooldownExpired : { false, true })
    {
        const bool got  = CanThumbsUp(cooldownExpired);
        const bool want = cooldownExpired;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineCanThumbsUp(cooldownExpired), "compose free == inline") && ok;
        ok              = expect(got == pinCanThumbsUp(cooldownExpired), "compose free == pin") && ok;
        ok              = expect(got == pinCanThumbsUp3630(cooldownExpired), "compose free == pin3630") && ok;
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
        ok = expect(CanThumbsUp(cooldownExpired) == pinCanThumbsUp(cooldownExpired),
                    "host inject free == pin") &&
             ok;
        ok = expect(CanThumbsUp(cooldownExpired) == pinCanThumbsUp3630(cooldownExpired),
                    "host inject free == pin3630") &&
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

    // Prior dedicated 3630 independence: free == prior 3630 inline/pin.
    ok = expect(CanThumbsUp(true) == inlineCanThumbsUp3630(true), "prior 3630 independence free==inline true") && ok;
    ok = expect(CanThumbsUp(false) == inlineCanThumbsUp3630(false), "prior 3630 independence free==inline false") && ok;
    ok = expect(CanThumbsUp(true) == pinCanThumbsUp3630(true), "prior 3630 independence free==pin true") && ok;
    ok = expect(CanThumbsUp(false) == pinCanThumbsUp3630(false), "prior 3630 independence free==pin false") && ok;

    // Residual 2967 / 3150 / 3229 / 3278 / 3308 / 3419 / 3473 / 3541 / 3585 / 3630 / 0698 still hold under dedicated 3675.
    ok = expect(CanThumbsUp(true) && !CanThumbsUp(false), "residual 2967/3150/3229/3278/3308/3419/3473/3541/3585/3630/0698 under 3675") && ok;

    return ok;
}
