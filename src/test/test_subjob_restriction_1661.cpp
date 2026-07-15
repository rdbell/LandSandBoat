#include "test_subjob_restriction_1661.h"

#include "map/subjob_restriction_capacity.h"

#include <iostream>

namespace
{
using namespace subjobrestrictionhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "subjob restriction 1661 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runSubjobRestriction1661SelfTests() -> bool
{
    bool ok = true;

    // --- pins ---
    ok = expect(JobNone == 0, "JobNone pin") && ok;
    ok = expect(EffectSjRestriction == 157, "EffectSjRestriction pin") && ok;
    ok = expect(EffectObliviscence == 260, "EffectObliviscence pin") && ok;

    // --- SubjobRestricted ---
    ok = expect(!SubjobRestricted(false, false), "neither unrestricted") && ok;
    ok = expect(SubjobRestricted(true, false), "obliviscence alone") && ok;
    ok = expect(SubjobRestricted(false, true), "sj restriction alone") && ok;
    ok = expect(SubjobRestricted(true, true), "both restricted") && ok;

    // --- ResolveSJob unrestricted (ignore flag irrelevant) ---
    ok = expect(ResolveSJob(0, false, false) == 0, "sjob 0 unrestricted") && ok;
    ok = expect(ResolveSJob(6, false, false) == 6, "sjob 6 unrestricted") && ok;
    ok = expect(ResolveSJob(99, false, false) == 99, "sjob 99 unrestricted") && ok;
    ok = expect(ResolveSJob(255, true, false) == 255, "sjob 255 unrestricted ignore") && ok;

    // --- ResolveSJob restricted without ignore → JOB_NON ---
    ok = expect(ResolveSJob(0, false, true) == JobNone, "restricted sjob 0 → NON") && ok;
    ok = expect(ResolveSJob(6, false, true) == JobNone, "restricted sjob 6 → NON") && ok;
    ok = expect(ResolveSJob(99, false, true) == JobNone, "restricted sjob 99 → NON") && ok;
    ok = expect(ResolveSJob(255, false, true) == JobNone, "restricted sjob 255 → NON") && ok;

    // --- ResolveSJob restricted with ignore → stored ---
    ok = expect(ResolveSJob(0, true, true) == 0, "ignore restricted sjob 0") && ok;
    ok = expect(ResolveSJob(6, true, true) == 6, "ignore restricted sjob 6") && ok;
    ok = expect(ResolveSJob(99, true, true) == 99, "ignore restricted sjob 99") && ok;
    ok = expect(ResolveSJob(255, true, true) == 255, "ignore restricted sjob 255") && ok;

    // --- ResolveSLevel unrestricted ---
    ok = expect(ResolveSLevel(0, false) == 0, "slvl 0 unrestricted") && ok;
    ok = expect(ResolveSLevel(1, false) == 1, "slvl 1 unrestricted") && ok;
    ok = expect(ResolveSLevel(49, false) == 49, "slvl 49 unrestricted") && ok;
    ok = expect(ResolveSLevel(99, false) == 99, "slvl 99 unrestricted") && ok;
    ok = expect(ResolveSLevel(255, false) == 255, "slvl 255 unrestricted") && ok;

    // --- ResolveSLevel restricted → 0 (no ignore path) ---
    ok = expect(ResolveSLevel(0, true) == 0, "restricted slvl 0") && ok;
    ok = expect(ResolveSLevel(1, true) == 0, "restricted slvl 1 → 0") && ok;
    ok = expect(ResolveSLevel(49, true) == 0, "restricted slvl 49 → 0") && ok;
    ok = expect(ResolveSLevel(99, true) == 0, "restricted slvl 99 → 0") && ok;
    ok = expect(ResolveSLevel(255, true) == 0, "restricted slvl 255 → 0") && ok;

    // --- end-to-end composition ---
    {
        constexpr std::uint8_t storedSJob = 6;
        constexpr std::uint8_t storedSLvl = 49;

        const bool free = SubjobRestricted(false, false);
        ok = expect(ResolveSJob(storedSJob, false, free) == storedSJob, "e2e free sjob") && ok;
        ok = expect(ResolveSLevel(storedSLvl, free) == storedSLvl, "e2e free slvl") && ok;

        const bool obl = SubjobRestricted(true, false);
        ok = expect(ResolveSJob(storedSJob, false, obl) == JobNone, "e2e obl sjob") && ok;
        ok = expect(ResolveSLevel(storedSLvl, obl) == 0, "e2e obl slvl") && ok;
        ok = expect(ResolveSJob(storedSJob, true, obl) == storedSJob, "e2e obl ignore") && ok;

        const bool sjr = SubjobRestricted(false, true);
        ok = expect(ResolveSJob(storedSJob, false, sjr) == JobNone, "e2e sjr sjob") && ok;
        ok = expect(ResolveSLevel(storedSLvl, sjr) == 0, "e2e sjr slvl") && ok;
    }

    return ok;
}
