#include "test_notoriety_remove_member_3192.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldRemoveNotorietyMember 3192 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::remove admission for dual-wire cross-check
// (dedicated 3192):
//   ownerPresent && entityPresent
auto inlineShouldRemoveNotorietyMember3192(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

// Compact dual-wire pin matching Go pinShouldRemoveNotorietyMember3192 / C++ capacity
// positive form (formula unchanged from 2819 / 2971):
//   ownerPresent && entityPresent
auto pinShouldRemoveNotorietyMember3192(const bool ownerPresent, const bool entityPresent) -> bool
{
    return ownerPresent && entityPresent;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldRemoveNotorietyMember
// (CNotorietyContainer::remove admission two-bool AND;
// OmegaXI internal/notoriety; dedicated slice 3192; residual expand 2971 / pure 2819).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2971 / 2819 pins still hold
//   - dense 2^2 boolean space free == inline == pin
//   - host inject poles
auto runNotorietyRemoveMember3192SelfTests() -> bool
{
    using notorietyhelpers::ShouldRemoveNotorietyMember;

    bool ok = true;

    // Residual 2819 / 2971 pins still hold under dedicated dual-wire.
    ok = expect(ShouldRemoveNotorietyMember(true, true), "residual admit owner+entity") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "residual reject nil owner") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "residual reject nil entity") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "residual reject both nil") && ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        bool        ownerPresent;
        bool        entityPresent;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — both true.
        { true, true, true, "admit owner+entity" },

        // Residual 2819 / 2971 single-false poles.
        { false, true, false, "reject nil owner" },
        { true, false, false, "reject nil entity" },
        { false, false, false, "reject both nil" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRemoveNotorietyMember(c.ownerPresent, c.entityPresent);
        const bool inlineF = inlineShouldRemoveNotorietyMember3192(c.ownerPresent, c.entityPresent);
        const bool pin     = pinShouldRemoveNotorietyMember3192(c.ownerPresent, c.entityPresent);
        // Positive form pin composition (explicit two-bool AND).
        const bool wantPin = c.ownerPresent && c.entityPresent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldRemoveNotorietyMember free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldRemoveNotorietyMember(true, true) == pinShouldRemoveNotorietyMember3192(true, true),
                "free == pin admit both true") &&
         ok;
    ok = expect(ShouldRemoveNotorietyMember(false, true) == pinShouldRemoveNotorietyMember3192(false, true),
                "free == pin nil owner") &&
         ok;
    ok = expect(ShouldRemoveNotorietyMember(true, false) == pinShouldRemoveNotorietyMember3192(true, false),
                "free == pin nil entity") &&
         ok;
    ok = expect(ShouldRemoveNotorietyMember(false, false) == pinShouldRemoveNotorietyMember3192(false, false),
                "free == pin both nil") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            const bool got     = ShouldRemoveNotorietyMember(ownerPresent, entityPresent);
            const bool inlineF = inlineShouldRemoveNotorietyMember3192(ownerPresent, entityPresent);
            const bool pin     = pinShouldRemoveNotorietyMember3192(ownerPresent, entityPresent);
            const bool want    = ownerPresent && entityPresent;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production CNotorietyContainer::remove path semantics ---
    // Host injects:
    //   ownerPresent  = m_POwner != nullptr
    //   entityPresent = entity != nullptr
    // when true  → m_Lookup.find(entity); erase if present
    // when false → no-op
    ok = expect(ShouldRemoveNotorietyMember(true, true) && pinShouldRemoveNotorietyMember3192(true, true),
                "remove owner+entity → find/erase path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, true), "remove nil owner → no-op") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(true, false), "remove nil entity → no-op") && ok;
    ok = expect(!ShouldRemoveNotorietyMember(false, false), "remove both nil → no-op") && ok;

    // Explicit dual-wire: free == inline == pin for host-style poles.
    const struct
    {
        bool        owner;
        bool        entity;
        const char* label;
    } hostPoles[] = {
        { true, true, "both present admit" },
        { true, false, "nil entity reject" },
        { false, true, "nil owner reject" },
        { false, false, "both nil reject" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRemoveNotorietyMember(p.owner, p.entity);
        const bool inlineF = inlineShouldRemoveNotorietyMember3192(p.owner, p.entity);
        const bool pin     = pinShouldRemoveNotorietyMember3192(p.owner, p.entity);
        const bool want    = p.owner && p.entity;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
