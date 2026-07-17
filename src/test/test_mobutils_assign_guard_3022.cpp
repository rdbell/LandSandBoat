#include "test_mobutils_assign_guard_3022.h"

#include "map/mob_setup_capacity.h"
#include "map/mobutils_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils ShouldAssignGuardSkill 3022 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateMobStats / dual-wire formula for cross-check (slice 3022):
//   (mJob == 2 /*MNK*/ || mJob == 18 /*PUP*/) && cannotGuardMod == 0
auto inlineShouldAssignGuardSkill(const uint8 mJob, const int16 cannotGuardMod) -> bool
{
    return (mJob == 2 || mJob == 18) && cannotGuardMod == 0;
}

} // namespace

// Pure dual-wire expansion for mobutilshelpers::ShouldAssignGuardSkill and
// residual mobsetuphelpers::ShouldAssignGuardSkill (MNK/PUP +
// MOBMOD_CANNOT_GUARD == 0 assignment gate; slice 3022).
auto runMobutilsAssignGuard3022SelfTests() -> bool
{
    using mobutilshelpers::GuardSkillRank;
    using mobutilshelpers::ShouldAssignGuardSkill;

    bool ok = true;

    // Residual 1623 pins still hold under dual-wire free function.
    ok = expect(ShouldAssignGuardSkill(2 /*MNK*/, 0), "residual MNK 0 permits") && ok;
    ok = expect(ShouldAssignGuardSkill(18 /*PUP*/, 0), "residual PUP 0 permits") && ok;
    ok = expect(!ShouldAssignGuardSkill(2 /*MNK*/, 1), "residual MNK 1 rejects") && ok;
    ok = expect(!ShouldAssignGuardSkill(1 /*WAR*/, 0), "residual WAR 0 rejects") && ok;

    const struct
    {
        uint8       mJob;
        int16       cannotGuardMod;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles: MNK/PUP with cannotGuard 0/1.
        { 2, 0, true, "MNK cannotGuard 0 permits" },
        { 18, 0, true, "PUP cannotGuard 0 permits" },
        { 2, 1, false, "MNK cannotGuard 1 rejects" },
        { 18, 1, false, "PUP cannotGuard 1 rejects" },

        // WAR / job 0 with cannotGuard 0 still reject.
        { 1, 0, false, "WAR cannotGuard 0 rejects" },
        { 0, 0, false, "job 0 cannotGuard 0 rejects" },
        { 3, 0, false, "WHM cannotGuard 0 rejects" },

        // Non-guard jobs with cannotGuard 1 still reject.
        { 1, 1, false, "WAR cannotGuard 1 rejects" },
        { 0, 1, false, "job 0 cannotGuard 1 rejects" },

        // cannotGuard negative: MNK/PUP require exactly 0.
        { 2, -1, false, "MNK cannotGuard -1 rejects" },
        { 18, -1, false, "PUP cannotGuard -1 rejects" },
        { 2, -32768, false, "MNK cannotGuard int16 min rejects" },
        { 18, 32767, false, "PUP cannotGuard int16 max rejects" },

        // Residual 1623 re-pins.
        { 2, 0, true, "residual MNK 0" },
        { 18, 0, true, "residual PUP 0" },
        { 2, 1, false, "residual MNK 1" },
        { 1, 0, false, "residual WAR 0" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAssignGuardSkill(c.mJob, c.cannotGuardMod);
        const bool residual = mobsetuphelpers::ShouldAssignGuardSkill(c.mJob, c.cannotGuardMod);
        const bool inlineF = inlineShouldAssignGuardSkill(c.mJob, c.cannotGuardMod);
        const bool wantPin = (c.mJob == 2 || c.mJob == 18) && c.cannotGuardMod == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == residual, "mobutilshelpers dual-wire == mobsetuphelpers residual") && ok;
        ok = expect(got == inlineF, "ShouldAssignGuardSkill dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAssignGuardSkill == pin formula") && ok;
        ok = expect(residual == inlineF, "residual == inline LSB formula") && ok;
    }

    // GuardSkillRank is fixed rank C (3); dual-wire and residual agree.
    ok = expect(GuardSkillRank == 3, "mobutilshelpers::GuardSkillRank == 3") && ok;
    ok = expect(mobsetuphelpers::GuardSkillRank == 3, "mobsetuphelpers::GuardSkillRank == 3") && ok;
    ok = expect(GuardSkillRank == mobsetuphelpers::GuardSkillRank, "dual-wire GuardSkillRank == residual") && ok;

    // Pin composition: free function is the (MNK|PUP) && cannotGuard==0 gate only.
    ok = expect(ShouldAssignGuardSkill(2, 0), "MNK 0 must permit") && ok;
    ok = expect(ShouldAssignGuardSkill(18, 0), "PUP 0 must permit") && ok;
    ok = expect(!ShouldAssignGuardSkill(2, 1), "MNK 1 must reject") && ok;
    ok = expect(!ShouldAssignGuardSkill(1, 0), "WAR 0 must reject") && ok;
    ok = expect(!ShouldAssignGuardSkill(0, 0), "job 0 must reject") && ok;
    ok = expect(!ShouldAssignGuardSkill(2, -1), "negative cannotGuard must reject") && ok;

    // Dense compose over representative job × cannotGuard poles:
    // dual-wire free == residual free == pin == inline.
    const uint8 jobs[] = { 0, 1, 2, 3, 18, 19, 22, 255 };
    const int16 mods[] = { -32768, -1, 0, 1, 2, 32767 };
    for (const uint8 mJob : jobs)
    {
        for (const int16 cannotGuardMod : mods)
        {
            const bool got      = ShouldAssignGuardSkill(mJob, cannotGuardMod);
            const bool residual = mobsetuphelpers::ShouldAssignGuardSkill(mJob, cannotGuardMod);
            const bool want     = (mJob == 2 || mJob == 18) && cannotGuardMod == 0;
            ok                  = expect(got == want, "compose free == pin formula") && ok;
            ok                  = expect(got == residual, "compose free == residual") && ok;
            ok                  = expect(got == inlineShouldAssignGuardSkill(mJob, cannotGuardMod), "compose free == inline") && ok;
            ok                  = expect(residual == want, "compose residual == pin formula") && ok;
        }
    }

    // Host-style inject poles: GetMJob() as uint8 + getMobMod(MOBMOD_CANNOT_GUARD) as int16.
    // (Live CalculateMobStats assign path is residual 1623 / mobutils.cpp.)
    const struct
    {
        uint8 mJob;
        int16 cannotGuardMod;
        bool  want;
    } hostPoles[] = {
        { 2, 0, true },
        { 18, 0, true },
        { 2, 1, false },
        { 18, 1, false },
        { 1, 0, false },
        { 0, 0, false },
        { 2, -1, false },
        { 18, -32768, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool inject = (h.mJob == 2 || h.mJob == 18) && h.cannotGuardMod == 0;
        ok                = expect(ShouldAssignGuardSkill(h.mJob, h.cannotGuardMod) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldAssignGuardSkill(h.mJob, h.cannotGuardMod) ==
                        mobsetuphelpers::ShouldAssignGuardSkill(h.mJob, h.cannotGuardMod),
                    "host inject dual-wire == residual") &&
             ok;
        ok = expect(ShouldAssignGuardSkill(h.mJob, h.cannotGuardMod) ==
                        inlineShouldAssignGuardSkill(h.mJob, h.cannotGuardMod),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldAssignGuardSkill(h.mJob, h.cannotGuardMod) == h.want, "host inject want pin") && ok;
    }

    // Production path semantics:
    //   if ShouldAssignGuardSkill(GetMJob(), getMobMod(MOBMOD_CANNOT_GUARD)):
    //       WorkingSkills.skill[SKILL_GUARD] = GetBaseSkill(PMob, GuardSkillRank)
    // when true  → assign guard skill at fixed rank C (3)
    // when false → leave guard skill unassigned by this gate
    ok = expect(ShouldAssignGuardSkill(2, 0), "MNK + cannotGuard 0 → assign path") && ok;
    ok = expect(ShouldAssignGuardSkill(18, 0), "PUP + cannotGuard 0 → assign path") && ok;
    ok = expect(!ShouldAssignGuardSkill(2, 1), "MNK + cannotGuard 1 → no assign path") && ok;
    ok = expect(!ShouldAssignGuardSkill(1, 0), "WAR + cannotGuard 0 → no assign path") && ok;
    ok = expect(!ShouldAssignGuardSkill(0, 0), "job 0 + cannotGuard 0 → no assign path") && ok;
    ok = expect(!ShouldAssignGuardSkill(2, -1), "MNK + cannotGuard negative → no assign path") && ok;

    return ok;
}
