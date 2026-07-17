#include "test_aman_can_thumbs_up_3150.h"

#include "map/aman_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aman CanThumbsUp 3150 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAMANContainer::canThumbsUp pure half for dual-wire cross-check
// (residual 2967 / dedicated 3150):
//   cooldownExpired
// Production resolves cooldown via getCharVar(thumbsUpCooldownVar) == 0.
auto inlineCanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

// Compact dual-wire pin matching Go pinCanThumbsUp3150:
//   cooldownExpired
auto pinCanThumbsUp(const bool cooldownExpired) -> bool
{
    return cooldownExpired;
}

} // namespace

// Pure dual-wire expansion for amanhelpers::CanThumbsUp
// (thumbs-up cooldown-expired identity gate; OmegaXI internal/aman;
// slice 3150).
//
// Coverage:
//   - cooldownExpired true → permit / false → reject
//   - free == inline == pin == cooldownExpired
//   - residual 0698 / 2967 pins still hold
auto runAmanCanThumbsUp3150SelfTests() -> bool
{
    using amanhelpers::CanThumbsUp;

    bool ok = true;

    // Residual 0698 / 2967 pins still hold under dual-wire.
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
        // Classic dual poles.
        { true, true, "cooldown expired permits thumbs-up" },
        { false, false, "cooldown active rejects thumbs-up" },

        // Residual 2967 re-pins.
        { true, true, "residual 2967 true" },
        { false, false, "residual 2967 false" },

        // Residual 0698 re-pins.
        { true, true, "residual 0698 true" },
        { false, false, "residual 0698 false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanThumbsUp(c.cooldownExpired);
        const bool inlineF = inlineCanThumbsUp(c.cooldownExpired);
        const bool pinGot  = pinCanThumbsUp(c.cooldownExpired);
        const bool wantPin = c.cooldownExpired;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanThumbsUp dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "CanThumbsUp dual-wire == pin") && ok;
        ok = expect(got == wantPin, "CanThumbsUp == pin formula (identity)") && ok;
    }

    // Pin composition: free function is identity on cooldownExpired only.
    ok = expect(CanThumbsUp(true), "true must permit") && ok;
    ok = expect(!CanThumbsUp(false), "false must reject") && ok;
    ok = expect(CanThumbsUp(true) == pinCanThumbsUp(true), "free==pin true") && ok;
    ok = expect(CanThumbsUp(false) == pinCanThumbsUp(false), "free==pin false") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool cooldownExpired : { false, true })
    {
        const bool got  = CanThumbsUp(cooldownExpired);
        const bool want = cooldownExpired;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineCanThumbsUp(cooldownExpired), "compose free == inline") && ok;
        ok              = expect(got == pinCanThumbsUp(cooldownExpired), "compose free == pin") && ok;
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
