#include "test_mobutils_assign_parry_3361.h"

#include "map/mobutils_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils ShouldAssignParrySkill 3361 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CalculateMobStats / mobsetuphelpers::ShouldAssignParrySkill formula
// for dual-wire cross-check (dedicated slice 3361 expand residual 2972):
//   canParryMod > 0
auto inlineShouldAssignParrySkill3361(const int16 canParryMod) -> bool
{
    return canParryMod > 0;
}

// Dual-wire pin matching production free function / capacity body
// (dedicated slice 3361). Direct return only — same formula.
//   canParryMod > 0
auto pinShouldAssignParrySkill3361(const int16 canParryMod) -> bool
{
    return canParryMod > 0;
}

} // namespace

// Pure dual-wire expansion for mobutilshelpers::ShouldAssignParrySkill
// (MOBMOD_CAN_PARRY > 0 assignment gate; dedicated slice 3361 expand residual 2972).
// Required poles:
//   - canParryMod > 0 → true
//   - canParryMod <= 0 → false
//   - free == inline == pin
//   - residual 2972 / 1623 / prior re-index 3115 pins still hold
auto runMobutilsAssignParry3361SelfTests() -> bool
{
    using mobutilshelpers::ShouldAssignParrySkill;

    bool ok = true;

    // Residual 1623 / residual dual-wire 2972 / prior re-index 3115 pins.
    ok = expect(ShouldAssignParrySkill(3), "residual positive 3 permits") && ok;
    ok = expect(!ShouldAssignParrySkill(0), "residual zero rejects") && ok;

    const struct
    {
        int16       canParryMod;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 0, false, "zero CAN_PARRY rejects assignment" },
        { 1, true, "rank 1 permits assignment" },
        { 3, true, "rank 3 (typical Dynamis / trust) permits" },
        { 5, true, "rank 5 permits assignment" },

        // Boundaries.
        { -1, false, "negative mod rejects" },
        { -32768, false, "int16 min rejects" },
        { 2, true, "rank 2 permits" },
        { 4, true, "rank 4 permits" },
        { 32767, true, "int16 max permits" },

        // Residual 1623 / 2972 / 3115 re-pins.
        { 3, true, "residual positive 3" },
        { 0, false, "residual zero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAssignParrySkill(c.canParryMod);
        const bool inlineF = inlineShouldAssignParrySkill3361(c.canParryMod);
        const bool pinF    = pinShouldAssignParrySkill3361(c.canParryMod);
        const bool wantPin = c.canParryMod > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAssignParrySkill dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldAssignParrySkill dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldAssignParrySkill == pin formula") && ok;
        ok = expect(inlineF == pinF, "inline == pin") && ok;
    }

    // Pin composition: free function is the canParryMod > 0 gate only.
    ok = expect(!ShouldAssignParrySkill(0), "0 must reject") && ok;
    ok = expect(ShouldAssignParrySkill(1), "1 must permit") && ok;
    ok = expect(!ShouldAssignParrySkill(-1), "negative must reject") && ok;
    ok = expect(ShouldAssignParrySkill(0) == pinShouldAssignParrySkill3361(0),
                "free == pin residual zero") &&
         ok;
    ok = expect(ShouldAssignParrySkill(3) == pinShouldAssignParrySkill3361(3),
                "free == pin residual positive 3") &&
         ok;
    ok = expect(ShouldAssignParrySkill(1) == pinShouldAssignParrySkill3361(1),
                "free == pin residual rank 1") &&
         ok;

    // Dense compose over representative poles: free == pin == inline.
    const int16 poles[] = { -32768, -1, 0, 1, 2, 3, 5, 100, 32767 };
    for (const int16 canParryMod : poles)
    {
        const bool got  = ShouldAssignParrySkill(canParryMod);
        const bool want = canParryMod > 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok = expect(got == inlineShouldAssignParrySkill3361(canParryMod), "compose free == inline") && ok;
        ok = expect(got == pinShouldAssignParrySkill3361(canParryMod), "compose free == pin") && ok;
    }

    // Host-style inject poles: getMobMod(MOBMOD_CAN_PARRY) as int16.
    // (Live CalculateMobStats assign path is residual 1623 / mobutils.cpp.)
    const struct
    {
        int16 canParryMod;
        bool  want;
    } hostPoles[] = {
        { 0, false },
        { 1, true },
        { 3, true },
        { 5, true },
        { -1, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool inject = h.canParryMod > 0;
        ok                = expect(ShouldAssignParrySkill(h.canParryMod) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldAssignParrySkill(h.canParryMod) == inlineShouldAssignParrySkill3361(h.canParryMod),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldAssignParrySkill(h.canParryMod) == pinShouldAssignParrySkill3361(h.canParryMod),
                    "host inject free == pin") &&
             ok;
        ok = expect(ShouldAssignParrySkill(h.canParryMod) == h.want, "host inject want pin") && ok;
    }

    // Production path semantics:
    //   if ShouldAssignParrySkill(getMobMod(MOBMOD_CAN_PARRY)):
    //       WorkingSkills.skill[SKILL_PARRY] = GetBaseSkill(PMob, canParryMod)
    // when true  → assign parry skill from mod rank
    // when false → leave parry skill unassigned by this gate
    ok = expect(!ShouldAssignParrySkill(0), "no CAN_PARRY → no assign path") && ok;
    ok = expect(ShouldAssignParrySkill(1), "CAN_PARRY rank 1 → assign path") && ok;
    ok = expect(ShouldAssignParrySkill(3), "CAN_PARRY rank 3 → assign path") && ok;

    return ok;
}
