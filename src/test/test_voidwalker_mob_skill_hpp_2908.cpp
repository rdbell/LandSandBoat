#include "test_voidwalker_mob_skill_hpp_2908.h"

#include "map/voidwalker_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "voidwalker ShouldDoMobSkillEveryHPP 2908 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline voidwalker.lua local doMobSkillEveryHPP pure gate for dual-wire
// cross-check (slice 2908):
//   if every <= 0: false  // Go/C++ guard; Lua % 0 errors
//   if not (mobhpp <= start and condition): false
//   if (start % every) != (mobhpp % every): false
//   return getLocalVar('MOB_SKILL_' .. mobhpp) == 0  // !localVarSet
auto inlineShouldDoMobSkillEveryHPP(const int32 mobHPP,
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

} // namespace

// Pure dual-wire expansion for voidwalkerhelpers::ShouldDoMobSkillEveryHPP
// (Lua local doMobSkillEveryHPP HPP-modulo fire gate; slice 2908).
auto runVoidwalkerMobSkillHPP2908SelfTests() -> bool
{
    using voidwalkerhelpers::MobSkillLocalVar;
    using voidwalkerhelpers::ShouldDoMobSkillEveryHPP;

    bool ok = true;

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

        // local-var already set at matching threshold
        { 60, 20, 80, true, true, false, "60 already set" },
        { 0, 20, 80, true, true, false, "0 already set" },

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
    };

    for (const auto& c : cases)
    {
        const bool got = ShouldDoMobSkillEveryHPP(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        const bool inlineF =
            inlineShouldDoMobSkillEveryHPP(c.mobHPP, c.every, c.start, c.condition, c.localVarSet);
        bool wantPin = false;
        if (c.every > 0 && c.mobHPP <= c.start && c.condition && (c.start % c.every) == (c.mobHPP % c.every))
        {
            wantPin = !c.localVarSet;
        }

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDoMobSkillEveryHPP dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "ShouldDoMobSkillEveryHPP == pin formula") && ok;
    }

    // Pin composition: modulo alignment at Capricornus thresholds.
    for (const int32 hpp : { int32{ 80 }, int32{ 60 }, int32{ 40 }, int32{ 20 }, int32{ 0 } })
    {
        ok = expect(ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, false), "Capricornus threshold fires") && ok;
        ok = expect(!ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, true), "Capricornus threshold local set blocked") &&
             ok;
    }
    for (const int32 hpp : { int32{ 79 }, int32{ 70 }, int32{ 50 }, int32{ 30 }, int32{ 10 }, int32{ 1 } })
    {
        ok = expect(!ShouldDoMobSkillEveryHPP(hpp, 20, 80, true, false), "Capricornus non-threshold blocked") && ok;
    }

    // MobSkillLocalVar pin composition.
    ok = expect(MobSkillLocalVar(80) == "MOB_SKILL_80", "MobSkillLocalVar(80)") && ok;
    ok = expect(MobSkillLocalVar(0) == "MOB_SKILL_0", "MobSkillLocalVar(0)") && ok;
    ok = expect(MobSkillLocalVar(82) == "MOB_SKILL_82", "MobSkillLocalVar(82)") && ok;

    // Dense compose: every/start/hpp/condition/localVarSet grid.
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
                        const bool want =
                            inlineShouldDoMobSkillEveryHPP(hpp, every, start, condition, localVarSet);
                        ok = expect(got == want, "compose free == pin formula") && ok;
                    }
                }
            }
        }
    }

    return ok;
}
