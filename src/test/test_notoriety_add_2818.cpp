#include "test_notoriety_add_2818.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety add 2818 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNotorietyAdd2818SelfTests() -> bool
{
    using namespace notorietyhelpers;

    bool ok = true;

    // All three required for admission
    ok = expect(ShouldAddNotorietyMember(true, true, true), "admit owner+entity+diff") && ok;

    // Missing any gate rejects
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "reject nil owner") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "reject nil entity") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "reject same allegiance") && ok;

    // Combined failures
    ok = expect(!ShouldAddNotorietyMember(false, false, true), "reject both nil +diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, false), "reject nil owner same") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, false), "reject nil entity same") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, false, false), "reject all false") && ok;

    // Exhaustive truth table over the three bools
    const struct
    {
        bool        ownerPresent;
        bool        entityPresent;
        bool        differentAllegiance;
        bool        wantAdd;
        const char* label;
    } cases[] = {
        { true, true, true, true, "table admit" },
        { true, true, false, false, "table same allegiance" },
        { true, false, true, false, "table nil entity" },
        { true, false, false, false, "table nil entity same" },
        { false, true, true, false, "table nil owner" },
        { false, true, false, false, "table nil owner same" },
        { false, false, true, false, "table both nil" },
        { false, false, false, false, "table all false" },
    };
    for (const auto& c : cases)
    {
        const bool got = ShouldAddNotorietyMember(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        ok             = expect(got == c.wantAdd, c.label) && ok;
        const bool expected = c.ownerPresent && c.entityPresent && c.differentAllegiance;
        ok                  = expect(got == expected, "compose expression") && ok;
    }

    return ok;
}
