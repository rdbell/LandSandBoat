#include "test_notoriety_remove_member_2971.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldRemoveNotorietyMember 2971 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::remove admission for dual-wire cross-check
// (slice 2971):
//   ownerPresent && entityPresent
auto inlineShouldRemoveNotorietyMember(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldRemoveNotorietyMember
// (CNotorietyContainer::remove admission two-bool AND; slice 2971).
auto runNotorietyRemoveMember2971SelfTests() -> bool
{
    using notorietyhelpers::ShouldRemoveNotorietyMember;

    bool ok = true;

    // Residual 2819 pins still hold under dual-wire.
    ok = expect(ShouldRemoveNotorietyMember(true, true), "residual admit owner+entity") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "residual reject nil owner") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "residual reject nil entity") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "residual reject both nil") && ok;

    const struct
    {
        bool        ownerPresent;
        bool        entityPresent;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — both true.
        { true, true, true, "admit owner+entity" },

        // Residual 2819 single-false poles.
        { false, true, false, "reject nil owner" },
        { true, false, false, "reject nil entity" },
        { false, false, false, "reject both nil" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRemoveNotorietyMember(c.ownerPresent, c.entityPresent);
        const bool inlineF = inlineShouldRemoveNotorietyMember(c.ownerPresent, c.entityPresent);
        const bool wantPin = c.ownerPresent && c.entityPresent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRemoveNotorietyMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRemoveNotorietyMember == pin formula AND of two") && ok;
    }

    // Pin composition: only the full-true pole admits.
    ok = expect(ShouldRemoveNotorietyMember(true, true), "all true must admit") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "ownerPresent false must reject") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "entityPresent false must reject") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "both false must reject") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            const bool got  = ShouldRemoveNotorietyMember(ownerPresent, entityPresent);
            const bool want = ownerPresent && entityPresent;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRemoveNotorietyMember(ownerPresent, entityPresent),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CNotorietyContainer::remove path semantics ---
    // Host injects:
    //   ownerPresent  = m_POwner != nullptr
    //   entityPresent = entity != nullptr
    // when true  → m_Lookup.find(entity); erase if present
    // when false → no-op
    ok = expect(ShouldRemoveNotorietyMember(true, true), "remove owner+entity → find/erase path") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "remove nil owner → no-op") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "remove nil entity → no-op") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "remove both nil → no-op") && ok;

    // Explicit dual-wire: free function is the two-bool AND of injects.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            ok = expect(ShouldRemoveNotorietyMember(ownerPresent, entityPresent) ==
                            (ownerPresent && entityPresent),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldRemoveNotorietyMember(ownerPresent, entityPresent) ==
                            inlineShouldRemoveNotorietyMember(ownerPresent, entityPresent),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    return ok;
}
