#include "test_notoriety_remove_2819.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety remove 2819 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNotorietyRemove2819SelfTests() -> bool
{
    using namespace notorietyhelpers;

    bool ok = true;

    // Both required for admission
    ok = expect(ShouldRemoveNotorietyMember(true, true), "admit owner+entity") && ok;

    // Missing either gate rejects
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "reject nil owner") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "reject nil entity") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "reject both nil") && ok;

    // Exhaustive truth table over the two bools
    const struct
    {
        bool        ownerPresent;
        bool        entityPresent;
        bool        wantRemove;
        const char* label;
    } cases[] = {
        { true, true, true, "table admit" },
        { true, false, false, "table nil entity" },
        { false, true, false, "table nil owner" },
        { false, false, false, "table both nil" },
    };
    for (const auto& c : cases)
    {
        const bool got = ShouldRemoveNotorietyMember(c.ownerPresent, c.entityPresent);
        ok             = expect(got == c.wantRemove, c.label) && ok;
        const bool expected = c.ownerPresent && c.entityPresent;
        ok                  = expect(got == expected, "compose expression") && ok;
    }

    return ok;
}
