#include "test_voidwalker_mob_skill_hpp_3334.h"

#include "map/voidwalker_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldDoMobSkillEveryHPP 3334 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua local doMobSkillEveryHPP pure gate for dual-wire
// cross-check (dedicated 3334):
//   if every <= 0: false  // Go/C++ guard; Lua % 0 errors
//   if not (mobhpp <= start and condition): false
//   if (start % every) != (mobhpp % every): false
//   return getLocalVar('MOB_SKILL_' .. mobhpp) == 0  // !localVarSet
auto inlineShouldDoMobSkillEveryHPP3334(const int32 mobHPP,
                                        const int32 every,
                                        const int32 start,
                                        const bool  condition,
                                        const bool  localVarSet) -> bool
{
    if (every <= 0 || mobHPP > start || !condition)
    {
        return false;
    }
    if ((start % every) != (mobHPP % every))
    {
        return false;
    }
    return !localVarSet;
}

// Compact dual-wire pin matching Go pinShouldDoMobSkillEveryHPP3334 / C++
// capacity free-function form (formula unchanged from 0987 / 2908). Positive
// if/else form only — avoid QF1001 De Morgan rewrites of !localVarSet /
// !condition:
//   if every <= 0 { return false }
//   if mobHPP > start { return false }
//   if !condition { return false }
//   if (start % every) != (mobHPP % every) { return false }
//   if localVarSet { return false }
//   return true
auto pinShouldDoMobSkillEveryHPP3334(const int32 mobHPP,
                                     const int32 every,
                                     const int32 start,
                                     const bool  condition,
                                     const bool  localVarSet) -> bool
{
    if (every <= 0)
    {
        return false;
    }
    if (mobHPP > start)
    {
        return false;
    }
    if (!condition)
    {
        return false;
    }
    if ((start % every) != (mobHPP % every))
    {
        return false;
    }
    if (localVarSet)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldDoMobSkillEveryHPP
// (Lua local doMobSkillEveryHPP HPP-modulo fire gate;
// OmegaXI internal/voidwalker; dedicated slice 3334; residual expand 2908 / pure 0987).
//
// Coverage:
//   - free == inline == pin (positive if/else form)
//   - residual 2908 / 0987 pins still hold
//   - poles every<=0, hpp>start, !condition, mod mismatch, localVarSet
auto runVoidwalkerMobSkillHPP3334SelfTests() -> bool
{
    using voidwalkerhelpers::MobSkillLocalVar;
    using voidwalkerhelpers::ShouldDoMobSkillEveryHPP;

    bool ok = true;

    // Residual 0987 / 2908 pins still hold under dedicated dual-wire.
    ok = expect(ShouldDoMobSkillEveryHPP(80, 20, 80, true, false), "residual 0987: Capricornus 80 first fire") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 20, 80, true, true), "residual 0987: Capricornus 80 already set") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(70, 20, 80, true, false), "residual 0987: modulo miss 70") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(60, 20, 80, true, false), "residual 0987: Capricornus 60 fire") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(90, 20, 80, true, false), "residual 0987: above start") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 20, 80, false, false), "residual 0987: condition false") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(82, 20, 82, true, false), "residual 0987: Blobdingnag 82") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(62, 20, 82, true, false), "residual 0987: Blobdingnag 62") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(81, 20, 82, true, false), "residual 0987: Blobdingnag 81 skip") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 0, 80, true, false), "residual 0987: every 0 guard") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, -1, 80, true, false), "residual 0987: every negative") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(0, 20, 80, true, false), "residual 0987: hpp 0 every 20 start 80") && ok;
    // Residual 2908 denser poles.
    ok = expect(ShouldDoMobSkillEveryHPP(40, 20, 80, true, false), "residual 2908: Capricornus 40 fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(20, 20, 80, true, false), "residual 2908: Capricornus 20 fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(2, 20, 82, true, false), "residual 2908: Blobdingnag 2 fire") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(0, 20, 82, true, false), "residual 2908: Blobdingnag 0 skip") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(60, 20, 80, true, true), "residual 2908: 60 already set") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(50, 1, 50, true, false), "residual 2908: every 1 at start") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(51, 1, 50, true, false), "residual 2908: every 1 above start") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(75, 10, 75, true, false), "residual 2908: start equals hpp") && ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(70, 10, 75, true, false), "residual 2908: misaligned 70") && ok;
    ok = expect(MobSkillLocalVar(80) == "MOB_SKILL_80", "residual 2908: MobSkillLocalVar(80)") && ok;
    ok = expect(MobSkillLocalVar(0) == "MOB_SKILL_0", "residual 2908: MobSkillLocalVar(0)") && ok;
    ok = expect(MobSkillLocalVar(82) == "MOB_SKILL_82", "residual 2908: MobSkillLocalVar(82)") && ok;

    // --- Core poles: free == inline == pin (every / start / condition / mod / localVar) ---
    const struct
    {
        int32       mobHPP;
        int32       every;
        int32       start;
        bool        condition;
        bool        localVarSet;
        bool        want;
        const char* label;
    } cases[] = {
        // Capricornus-style: every=20, start=80 → fire at 80, 60, 40, 20, 0
        { 80, 20, 80, true, false, true, "80 first fire" },
        { 80, 20, 80, true, true, false, "80 already set" },
        { 70, 20, 80, true, false, false, "70 skip (modulo miss)" },
        { 60, 20, 80, true, false, true, "60 fire" },
        { 40, 20, 80, true, false, true, "40 fire" },
        { 20, 20, 80, true, false, true, "20 fire" },
        { 0, 20, 80, true, false, true, "0 fire" },
        { 90, 20, 80, true, false, false, "above start" },
        { 80, 20, 80, false, false, false, "condition false" },

        // Blobdingnag: every=20, start=82 → fire at 82, 62, 42, 22, 2
        { 82, 20, 82, true, false, true, "blobdingnag 82" },
        { 62, 20, 82, true, false, true, "blobdingnag 62" },
        { 42, 20, 82, true, false, true, "blobdingnag 42" },
        { 81, 20, 82, true, false, false, "blobdingnag 81 skip" },
        { 80, 20, 82, true, false, false, "blobdingnag 80 skip" },
        { 2, 20, 82, true, false, true, "blobdingnag 2 fire" },
        { 0, 20, 82, true, false, false, "blobdingnag 0 skip" },

        // every guards (Lua % 0 errors; hosts never pass 0)
        { 80, 0, 80, true, false, false, "every 0 guard" },
        { 80, -1, 80, true, false, false, "every negative" },
        { 80, -20, 80, true, false, false, "every large negative" },

        // local-var already set at matching threshold
        { 60, 20, 80, true, true, false, "60 already set" },
        { 0, 20, 80, true, true, false, "0 already set" },
        { 82, 20, 82, true, true, false, "blobdingnag 82 already set" },

        // every=1: every HPP at or below start matches modulo
        { 50, 1, 50, true, false, true, "every 1 at start" },
        { 49, 1, 50, true, false, true, "every 1 below start" },
        { 51, 1, 50, true, false, false, "every 1 above start" },
        { 50, 1, 50, true, true, false, "every 1 already set" },

        // start == mobHPP edge with non-aligned every
        { 75, 10, 75, true, false, true, "start equals hpp" },
        { 65, 10, 75, true, false, true, "aligned 65" },
        { 70, 10, 75, true, false, false, "misaligned 70" },

        // production-ish: condition false even when HPP matches
        { 80, 20, 80, false, true, false, "condition false + local set" },

        // dedicated poles: every<=0, hpp>start, !condition, mod mismatch, localVarSet
        { 100, 0, 100, true, false, false, "pole every<=0" },
        { 101, 20, 80, true, false, false, "pole hpp>start" },
        { 80, 20, 80, false, false, false, "pole !condition" },
        { 79, 20, 80, true, false, false, "pole mod mismatch" },
        { 80, 20, 80, true, true, false, "pole localVarSet" },
        { 80, 20, 80, true, false, true, "pole all ready fires" },
    };

    for (const auto& c : cases)
    {
        const bool got = ShouldDoMobSkillEveryHPP(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        const bool inlineF =
            inlineShouldDoMobSkillEveryHPP3334(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        const bool pin =
            pinShouldDoMobSkillEveryHPP3334(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldDoMobSkillEveryHPP free == inline == pin") && ok;
    }

    // Free == pin across residual poles + every / start / condition / mod / localVar.
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 0, 80, true, false), "every 0 must not fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(80, 0, 80, true, false) ==
                    pinShouldDoMobSkillEveryHPP3334(80, 0, 80, true, false),
                "free == pin for every 0") &&
         ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(90, 20, 80, true, false), "hpp > start must not fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(90, 20, 80, true, false) ==
                    pinShouldDoMobSkillEveryHPP3334(90, 20, 80, true, false),
                "free == pin for hpp > start") &&
         ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 20, 80, false, false), "!condition must not fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(80, 20, 80, false, false) ==
                    pinShouldDoMobSkillEveryHPP3334(80, 20, 80, false, false),
                "free == pin for !condition") &&
         ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(70, 20, 80, true, false), "mod mismatch must not fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(70, 20, 80, true, false) ==
                    pinShouldDoMobSkillEveryHPP3334(70, 20, 80, true, false),
                "free == pin for mod mismatch") &&
         ok;
    ok = expect(!ShouldDoMobSkillEveryHPP(80, 20, 80, true, true), "localVarSet must not fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(80, 20, 80, true, true) ==
                    pinShouldDoMobSkillEveryHPP3334(80, 20, 80, true, true),
                "free == pin for localVarSet") &&
         ok;
    ok = expect(ShouldDoMobSkillEveryHPP(80, 20, 80, true, false), "all ready must fire") && ok;
    ok = expect(ShouldDoMobSkillEveryHPP(80, 20, 80, true, false) ==
                    pinShouldDoMobSkillEveryHPP3334(80, 20, 80, true, false),
                "free == pin for all ready") &&
         ok;

    // Capricornus threshold composition: free == inline == pin.
    for (const int32 hpp : { int32{ 80 }, int32{ 60 }, int32{ 40 }, int32{ 20 }, int32{ 0 } })
    {
        const bool got    = ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, false);
        const bool inlineF = inlineShouldDoMobSkillEveryHPP3334(hpp, 20, 80, true, false);
        const bool pin    = pinShouldDoMobSkillEveryHPP3334(hpp, 20, 80, true, false);
        ok = expect(got && got == inlineF && got == pin, "Capricornus threshold free == inline == pin") && ok;
        ok = expect(!ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, true), "Capricornus threshold local set blocked") &&
             ok;
    }
    for (const int32 hpp : { int32{ 79 }, int32{ 70 }, int32{ 50 }, int32{ 30 }, int32{ 10 }, int32{ 1 } })
    {
        const bool got    = ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, false);
        const bool inlineF = inlineShouldDoMobSkillEveryHPP3334(hpp, 20, 80, true, false);
        const bool pin    = pinShouldDoMobSkillEveryHPP3334(hpp, 20, 80, true, false);
        ok = expect(!got && got == inlineF && got == pin, "Capricornus non-threshold free == inline == pin") && ok;
    }

    // Dense compose: every/start/hpp/condition/localVarSet grid:
    // free == inline == pin (positive if/else).
    for (const int32 every : { int32{ 0 }, int32{ -1 }, int32{ 1 }, int32{ 10 }, int32{ 20 } })
    {
        for (const int32 start : { int32{ 0 }, int32{ 50 }, int32{ 80 }, int32{ 82 }, int32{ 100 } })
        {
            for (int32 hpp = 0; hpp <= 100; hpp += 5)
            {
                for (const bool condition : { false, true })
                {
                    for (const bool localVarSet : { false, true })
                    {
                        const bool got =
                            ShouldDoMobSkillEveryHPP(hpp, every, start, condition, localVarSet);
                        const bool inlineF =
                            inlineShouldDoMobSkillEveryHPP3334(hpp, every, start, condition, localVarSet);
                        const bool pin =
                            pinShouldDoMobSkillEveryHPP3334(hpp, every, start, condition, localVarSet);
                        ok = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
                    }
                }
            }
        }
    }

    // Host inject poles: production injects getHPP, every, start, condition,
    // getLocalVar('MOB_SKILL_'..hpp) != 0.
    // when true  → useMobAbility + setLocalVar('MOB_SKILL_'..hpp, 1)
    // when false → no skill this tick
    const struct
    {
        int32       mobHPP;
        int32       every;
        int32       start;
        bool        condition;
        bool        localVarSet;
        bool        want;
        const char* label;
    } hostCases[] = {
        { 80, 20, 80, true, false, true, "host Capricornus ready" },
        { 80, 20, 80, true, true, false, "host Capricornus local set" },
        { 82, 20, 82, true, false, true, "host Blobdingnag ready" },
        { 81, 20, 82, true, false, false, "host Blobdingnag mod miss" },
        { 90, 20, 80, true, false, false, "host above start" },
        { 80, 20, 80, false, false, false, "host condition blocks" },
        { 80, 0, 80, true, false, false, "host every 0 guard" },
        { 60, 20, 80, true, false, true, "host mid threshold fire" },
    };
    for (const auto& c : hostCases)
    {
        const bool got = ShouldDoMobSkillEveryHPP(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        const bool inlineF =
            inlineShouldDoMobSkillEveryHPP3334(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        const bool pin =
            pinShouldDoMobSkillEveryHPP3334(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
