#include "test_voidwalker_should_randomly_2903.h"

#include "map/voidwalker_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldRandomly 2903 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua local randomly for dual-wire cross-check:
//   math.random(0, 100) <= chance
//     and not hasStatusEffect(effect)
//     and GetSystemTime() > (MOBSKILL_TIME + between)
auto inlineShouldRandomly(const int32 roll,
                          const int32 chance,
                          const bool  hasEffect,
                          const int64 now,
                          const int64 lastSkillTime,
                          const int64 between) -> bool
{
    return roll <= chance && !hasEffect && now > (lastSkillTime + between);
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldRandomly
// (Lua local randomly chance / effect / cooldown gate; slice 2903).
auto runVoidwalkerShouldRandomly2903SelfTests() -> bool
{
    using voidwalkerhelpers::RandomlyRollMax;
    using voidwalkerhelpers::ShouldRandomly;

    bool ok = true;

    ok = expect(RandomlyRollMax == 100, "RandomlyRollMax pin 100") && ok;

    const struct
    {
        int32       roll;
        int32       chance;
        bool        hasEffect;
        int64       now;
        int64       lastSkillTime;
        int64       between;
        bool        want;
        const char* label;
    } cases[] = {
        // chance boundary (roll <= chance)
        { 10, 10, false, 100, 0, 60, true, "roll==chance ready fires" },
        { 11, 10, false, 100, 0, 60, false, "roll > chance blocked" },
        { 0, 0, false, 1, 0, 0, true, "chance 0 roll 0 fires" },
        { 1, 0, false, 1, 0, 0, false, "chance 0 roll 1 blocked" },
        { 0, 30, false, 100, 0, 60, true, "roll 0 always <= non-neg chance" },
        { 100, 100, false, 100, 0, 60, true, "max roll == max chance" },
        { 100, 99, false, 100, 0, 60, false, "max roll > chance-1" },

        // status effect gate
        { 0, 30, true, 100, 0, 60, false, "has effect blocked" },
        { 0, 30, false, 100, 0, 60, true, "no effect ready fires" },

        // cooldown: now > lastSkillTime+between (strict greater)
        { 0, 30, false, 50, 0, 60, false, "cooldown now < last+between" },
        { 0, 30, false, 60, 0, 60, false, "cooldown now == last+between" },
        { 0, 30, false, 61, 0, 60, true, "cooldown now > last+between" },
        { 0, 30, false, 100, 50, 50, false, "now == last+between interior" },
        { 0, 30, false, 101, 50, 50, true, "now > last+between interior" },

        // production-ish call sites (Lamprey_Lord chance=10, between=60)
        { 10, 10, false, 1000, 900, 60, true, "Lamprey_Lord edge ready" },
        { 11, 10, false, 1000, 900, 60, false, "Lamprey_Lord chance miss" },
        { 5, 10, true, 1000, 900, 60, false, "Lamprey_Lord has Blood Weapon" },
        { 5, 10, false, 1000, 950, 60, false, "Lamprey_Lord still cooling" },

        // Feuerunke / Erebus style chance=30 between=60
        { 30, 30, false, 200, 100, 60, true, "Feuerunke edge ready" },
        { 31, 30, false, 200, 100, 60, false, "Feuerunke chance miss" },

        // all three fail / all three pass compose
        { 50, 10, true, 10, 0, 60, false, "all three gates fail" },
        { 5, 10, false, 100, 0, 60, true, "all three gates pass" },

        // negative injects (defensive; hosts inject non-neg)
        { -1, 10, false, 100, 0, 60, true, "negative roll still <= chance" },
        { 0, -1, false, 100, 0, 60, false, "negative chance fails positive roll" },
    };

    for (const auto& c : cases)
    {
        const bool got = ShouldRandomly(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool inlineF =
            inlineShouldRandomly(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool wantPin =
            c.roll <= c.chance && !c.hasEffect && c.now > (c.lastSkillTime + c.between);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRandomly dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "ShouldRandomly == pin formula") && ok;
    }

    // Pin composition: free function uses strict > on cooldown.
    ok = expect(!ShouldRandomly(0, 100, false, 60, 0, 60), "now == last+between must not fire") && ok;
    ok = expect(ShouldRandomly(0, 100, false, 61, 0, 60), "now == last+between+1 must fire") && ok;

    // Dense compose: chance boundary x cooldown boundary x hasEffect.
    for (const bool hasEffect : { false, true })
    {
        for (int32 chance = 0; chance <= 5; ++chance)
        {
            for (int32 roll = -1; roll <= 6; ++roll)
            {
                for (const int64 now : { int64{ 59 }, int64{ 60 }, int64{ 61 } })
                {
                    constexpr int64 last = 0;
                    constexpr int64 betw = 60;
                    const bool      got  = ShouldRandomly(roll, chance, hasEffect, now, last, betw);
                    const bool      want =
                        roll <= chance && !hasEffect && now > (last + betw);
                    ok = expect(got == want, "compose free == pin formula") && ok;
                    ok = expect(got == inlineShouldRandomly(roll, chance, hasEffect, now, last, betw),
                                "compose free == inline") &&
                         ok;
                }
            }
        }
    }

    return ok;
}
