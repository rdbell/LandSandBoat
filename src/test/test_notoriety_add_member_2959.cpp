#include "test_notoriety_add_member_2959.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldAddNotorietyMember 2959 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::add admission for dual-wire cross-check
// (slice 2959):
//   ownerPresent && entityPresent && differentAllegiance
auto inlineShouldAddNotorietyMember(const bool ownerPresent, const bool entityPresent,
                                    const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldAddNotorietyMember
// (CNotorietyContainer::add admission three-bool AND; slice 2959).
auto runNotorietyAddMember2959SelfTests() -> bool
{
    using notorietyhelpers::ShouldAddNotorietyMember;

    bool ok = true;

    // Residual 2818 pins still hold under dual-wire.
    ok = expect(ShouldAddNotorietyMember(true, true, true), "residual admit owner+entity+diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "residual reject nil owner") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "residual reject nil entity") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "residual reject same allegiance") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, false, false), "residual reject all false") && ok;

    const struct
    {
        bool        ownerPresent;
        bool        entityPresent;
        bool        differentAllegiance;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — all three true.
        { true, true, true, true, "admit owner+entity+diff" },

        // Residual 2818 single-false poles.
        { false, true, true, false, "reject nil owner" },
        { true, false, true, false, "reject nil entity" },
        { true, true, false, false, "reject same allegiance" },

        // Combined failures.
        { false, false, true, false, "reject both nil +diff" },
        { false, true, false, false, "reject nil owner same" },
        { true, false, false, false, "reject nil entity same" },
        { false, false, false, false, "reject all false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddNotorietyMember(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool inlineF = inlineShouldAddNotorietyMember(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool wantPin = c.ownerPresent && c.entityPresent && c.differentAllegiance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAddNotorietyMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAddNotorietyMember == pin formula AND of three") && ok;
    }

    // Pin composition: only the full-true pole admits.
    ok = expect(ShouldAddNotorietyMember(true, true, true), "all true must admit") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "ownerPresent false must reject") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "entityPresent false must reject") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "differentAllegiance false must reject") && ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            for (const bool differentAllegiance : { false, true })
            {
                const bool got  = ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance);
                const bool want = ownerPresent && entityPresent && differentAllegiance;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // --- Production CNotorietyContainer::add path semantics ---
    // Host injects:
    //   ownerPresent        = m_POwner != nullptr
    //   entityPresent       = entity != nullptr
    //   differentAllegiance = ownerPresent && entityPresent &&
    //                         entity->allegiance != m_POwner->allegiance
    // when true  → m_Lookup.insert(entity)
    // when false → no-op
    ok = expect(ShouldAddNotorietyMember(true, true, true), "add owner+entity+diff → insert path") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "add nil owner → no-op") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "add nil entity → no-op") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "add same allegiance → no-op") && ok;

    // Host inject poles: differentAllegiance is host-safe — false when either
    // pointer is null. Pure helper still evaluates the three injected bools
    // independently (production never injects differentAllegiance=true when a
    // pointer is missing).
    ok = expect(!ShouldAddNotorietyMember(false, false, true),
                "both nil inject must reject even if differentAllegiance true") &&
         ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true),
                "nil owner inject must reject even if differentAllegiance true") &&
         ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true),
                "nil entity inject must reject even if differentAllegiance true") &&
         ok;

    // Explicit dual-wire: free function is the three-bool AND of injects.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            for (const bool differentAllegiance : { false, true })
            {
                ok = expect(ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance) ==
                                (ownerPresent && entityPresent && differentAllegiance),
                            "host inject identity") &&
                     ok;
                ok = expect(ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance) ==
                                inlineShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance),
                            "host inject dual-wire free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
