#include "test_notoriety_add_member_3267.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldAddNotorietyMember 3267 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::add admission for dual-wire cross-check
// (dedicated 3267):
//   ownerPresent && entityPresent && differentAllegiance
auto inlineShouldAddNotorietyMember3267(const bool ownerPresent, const bool entityPresent,
                                        const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

// Compact dual-wire pin matching Go pinShouldAddNotorietyMember3267 / C++ capacity
// direct AND-chain (formula unchanged from 2818 / 2959 / 3165):
//   ownerPresent && entityPresent && differentAllegiance
auto pinShouldAddNotorietyMember3267(const bool ownerPresent, const bool entityPresent,
                                     const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldAddNotorietyMember
// (CNotorietyContainer::add admission three-bool AND;
// OmegaXI internal/notoriety; dedicated slice 3267; residual expand 2959 /
// pure 2818; prior dedicated expand 3165).
//
// Coverage:
//   - free == inline == pin direct AND-chain
//   - residual 2959 / 2818 pins still hold
//   - dense 2^3 boolean space free == inline == pin
//   - host inject poles + counterfactual pure poles
auto runNotorietyAddMember3267SelfTests() -> bool
{
    using notorietyhelpers::ShouldAddNotorietyMember;

    bool ok = true;

    // Residual 2818 / 2959 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAddNotorietyMember(true, true, true), "residual admit owner+entity+diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "residual reject nil owner") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "residual reject nil entity") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "residual reject same allegiance") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, false, false), "residual reject all false") && ok;
    // Residual 2959 combined-failure poles.
    ok = expect(!ShouldAddNotorietyMember(false, false, true), "residual 2959 reject both nil +diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, false), "residual 2959 reject nil owner same") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, false), "residual 2959 reject nil entity same") && ok;

    // --- Core poles: free == inline == pin direct AND-chain ---
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

        // Combined failures (residual 2959).
        { false, false, true, false, "reject both nil +diff" },
        { false, true, false, false, "reject nil owner same" },
        { true, false, false, false, "reject nil entity same" },
        { false, false, false, false, "reject all false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddNotorietyMember(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool inlineF = inlineShouldAddNotorietyMember3267(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool pin     = pinShouldAddNotorietyMember3267(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        // Direct AND-chain pin composition (explicit three-bool AND).
        const bool wantPin = c.ownerPresent && c.entityPresent && c.differentAllegiance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldAddNotorietyMember free == inline == pin direct AND-chain") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldAddNotorietyMember(true, true, true) == pinShouldAddNotorietyMember3267(true, true, true),
                "free == pin admit all true") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(false, true, true) == pinShouldAddNotorietyMember3267(false, true, true),
                "free == pin nil owner") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, false, true) == pinShouldAddNotorietyMember3267(true, false, true),
                "free == pin nil entity") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, true, false) == pinShouldAddNotorietyMember3267(true, true, false),
                "free == pin same allegiance") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            for (const bool differentAllegiance : { false, true })
            {
                const bool got     = ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance);
                const bool inlineF = inlineShouldAddNotorietyMember3267(ownerPresent, entityPresent, differentAllegiance);
                const bool pin     = pinShouldAddNotorietyMember3267(ownerPresent, entityPresent, differentAllegiance);
                const bool want    = ownerPresent && entityPresent && differentAllegiance;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
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
    ok = expect(ShouldAddNotorietyMember(true, true, true) && pinShouldAddNotorietyMember3267(true, true, true),
                "add owner+entity+diff → insert path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "add nil owner → no-op") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "add nil entity → no-op") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "add same allegiance → no-op") && ok;

    // Host inject poles: differentAllegiance is host-safe — false when either
    // pointer is null. Pure helper still evaluates the three injected bools
    // independently (production never injects differentAllegiance=true when a
    // pointer is missing).
    const struct
    {
        bool        owner;
        bool        entity;
        bool        diff;
        const char* label;
    } hostPoles[] = {
        { true, true, true, "opposing allegiance admit" },
        { true, true, false, "same allegiance reject" },
        { false, true, false, "nil owner host-safe diff=false" },
        { true, false, false, "nil entity host-safe diff=false" },
        { false, false, false, "both nil host-safe" },
        // Counterfactual pure poles (production won't inject these):
        { false, true, true, "nil owner + forced diff" },
        { true, false, true, "nil entity + forced diff" },
        { false, false, true, "both nil + forced diff" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldAddNotorietyMember(p.owner, p.entity, p.diff);
        const bool inlineF = inlineShouldAddNotorietyMember3267(p.owner, p.entity, p.diff);
        const bool pin     = pinShouldAddNotorietyMember3267(p.owner, p.entity, p.diff);
        const bool want    = p.owner && p.entity && p.diff;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    return ok;
}
