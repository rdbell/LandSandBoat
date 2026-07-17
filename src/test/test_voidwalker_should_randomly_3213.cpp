#include "test_voidwalker_should_randomly_3213.h"

#include "map/voidwalker_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldRandomly 3213 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua local randomly for dual-wire cross-check
// (dedicated 3213):
//   math.random(0, 100) <= chance
//     and not hasStatusEffect(effect)
//     and GetSystemTime() > (MOBSKILL_TIME + between)
auto inlineShouldRandomly3213(const int32 roll,
                              const int32 chance,
                              const bool  hasEffect,
                              const int64 now,
                              const int64 lastSkillTime,
                              const int64 between) -> bool
{
    return roll <= chance && !hasEffect && now > (lastSkillTime + between);
}

// Compact dual-wire pin matching Go pinShouldRandomly3213 / C++ capacity
// free-function form (formula unchanged from 0987 / 2903). Positive if/else
// form only — avoid QF1001 De Morgan rewrites of !hasEffect:
//   if hasEffect { return false }
//   if roll <= chance {
//     if now > (lastSkillTime + between) { return true }
//   }
//   return false
auto pinShouldRandomly3213(const int32 roll,
                           const int32 chance,
                           const bool  hasEffect,
                           const int64 now,
                           const int64 lastSkillTime,
                           const int64 between) -> bool
{
    if (hasEffect)
    {
        return false;
    }
    if (roll <= chance)
    {
        if (now > (lastSkillTime + between))
        {
            return true;
        }
    }
    return false;
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldRandomly
// (Lua local randomly chance / effect / cooldown gate;
// OmegaXI internal/voidwalker; dedicated slice 3213; residual expand 2903 / pure 0987).
//
// Coverage:
//   - free == inline == pin (positive if/else form)
//   - residual 2903 / 0987 pins still hold
//   - poles chance / effect / cooldown
auto runVoidwalkerShouldRandomly3213SelfTests() -> bool
{
    using voidwalkerhelpers::RandomlyRollMax;
    using voidwalkerhelpers::ShouldRandomly;

    bool ok = true;

    // RandomlyRollMax pin (inject range upper bound).
    ok = expect(RandomlyRollMax == 100, "RandomlyRollMax pin 100") && ok;

    // Residual 0987 / 2903 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRandomly(10, 10, false, 100, 0, 60), "residual 0987: roll==chance ready should fire") && ok;
    ok = expect(!ShouldRandomly(11, 10, false, 100, 0, 60), "residual 0987: roll > chance should block") && ok;
    ok = expect(!ShouldRandomly(0, 30, true, 100, 0, 60), "residual 0987: has effect should block") && ok;
    ok = expect(!ShouldRandomly(0, 30, false, 50, 0, 60), "residual 0987: cooldown not ready should block") && ok;
    ok = expect(!ShouldRandomly(0, 30, false, 60, 0, 60), "residual 0987: now == last+between should block") && ok;
    ok = expect(ShouldRandomly(0, 30, false, 61, 0, 60), "residual 0987: now > last+between should fire") && ok;
    ok = expect(ShouldRandomly(0, 0, false, 1, 0, 0), "residual 0987: chance 0 roll 0 should fire") && ok;
    // Residual 2903 out-of-range / production-ish poles.
    ok = expect(ShouldRandomly(100, 100, false, 100, 0, 60), "residual 2903: max roll == max chance should fire") && ok;
    ok = expect(!ShouldRandomly(100, 99, false, 100, 0, 60), "residual 2903: max roll > chance-1 should block") && ok;
    ok = expect(ShouldRandomly(10, 10, false, 1000, 900, 60), "residual 2903: Lamprey_Lord edge ready should fire") && ok;
    ok = expect(!ShouldRandomly(5, 10, true, 1000, 900, 60), "residual 2903: Lamprey_Lord has Blood Weapon should block") && ok;
    ok = expect(!ShouldRandomly(5, 10, false, 1000, 950, 60), "residual 2903: Lamprey_Lord still cooling should block") && ok;
    ok = expect(ShouldRandomly(-1, 10, false, 100, 0, 60), "residual 2903: negative roll still <= chance should fire") && ok;
    ok = expect(!ShouldRandomly(0, -1, false, 100, 0, 60), "residual 2903: negative chance fails positive roll") && ok;

    // --- Core poles: free == inline == pin (chance / effect / cooldown) ---
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
        { 10, 10, true, 100, 0, 60, false, "chance edge blocked by effect" },

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
        const bool got     = ShouldRandomly(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool inlineF = inlineShouldRandomly3213(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool pin     = pinShouldRandomly3213(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        // Positive form pin composition (explicit early-return on hasEffect).
        const bool wantPin = pinShouldRandomly3213(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRandomly free == inline == pin") &&
             ok;
    }

    // Free == pin across residual poles + chance / effect / cooldown composition.
    ok = expect(!ShouldRandomly(0, 100, false, 60, 0, 60), "now == last+between must not fire") && ok;
    ok = expect(ShouldRandomly(0, 100, false, 60, 0, 60) == pinShouldRandomly3213(0, 100, false, 60, 0, 60),
                "free == pin for cooldown equality") &&
         ok;
    ok = expect(ShouldRandomly(0, 100, false, 61, 0, 60), "now == last+between+1 must fire") && ok;
    ok = expect(ShouldRandomly(0, 100, false, 61, 0, 60) == pinShouldRandomly3213(0, 100, false, 61, 0, 60),
                "free == pin for cooldown ready") &&
         ok;
    ok = expect(ShouldRandomly(0, 30, true, 100, 0, 60) == pinShouldRandomly3213(0, 30, true, 100, 0, 60),
                "free == pin for hasEffect") &&
         ok;
    ok = expect(ShouldRandomly(10, 10, false, 100, 0, 60) == pinShouldRandomly3213(10, 10, false, 100, 0, 60),
                "free == pin for chance edge") &&
         ok;
    ok = expect(ShouldRandomly(11, 10, false, 100, 0, 60) == pinShouldRandomly3213(11, 10, false, 100, 0, 60),
                "free == pin for chance miss") &&
         ok;

    // Dense compose: chance boundary x cooldown boundary x hasEffect:
    // free == inline == pin (positive if/else).
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
                    const bool      inlineF =
                        inlineShouldRandomly3213(roll, chance, hasEffect, now, last, betw);
                    const bool pin = pinShouldRandomly3213(roll, chance, hasEffect, now, last, betw);
                    const bool want =
                        roll <= chance && !hasEffect && now > (last + betw);
                    ok = expect(got == want, "compose free == pin formula") && ok;
                    ok = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
                }
            }
        }
    }

    // --- Production randomly path semantics ---
    // Host injects:
    //   roll = math.random(0, 100)
    //   has  = mob:hasStatusEffect(effect)
    //   now  = GetSystemTime()
    //   last = mob:getLocalVar('MOBSKILL_TIME')
    // when true  → setLocalVar MOBSKILL_USE/TIME + useMobAbility
    // when false → no skill this tick
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
    } hostCases[] = {
        { 0, RandomlyRollMax, false, 100, 0, 60, true, "host min roll ready" },
        { RandomlyRollMax, RandomlyRollMax, false, 100, 0, 60, true, "host max roll ready" },
        { RandomlyRollMax, RandomlyRollMax - 1, false, 100, 0, 60, false, "host max roll chance miss" },
        { 50, 30, false, 100, 0, 60, false, "host mid chance miss" },
        { 30, 30, false, 100, 0, 60, true, "host mid chance edge" },
        { 30, 30, true, 100, 0, 60, false, "host effect blocks edge" },
        { 30, 30, false, 60, 0, 60, false, "host cooldown equality blocks" },
        { 30, 30, false, 61, 0, 60, true, "host cooldown ready fires" },
    };
    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRandomly(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool inlineF = inlineShouldRandomly3213(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        const bool pin     = pinShouldRandomly3213(c.roll, c.chance, c.hasEffect, c.now, c.lastSkillTime, c.between);
        ok                 = expect(got == c.want, c.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
