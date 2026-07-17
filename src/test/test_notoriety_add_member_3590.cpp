#include "test_notoriety_add_member_3590.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldAddNotorietyMember 3590 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::add admission for dual-wire cross-check
// (dedicated 3590 expand residual 2959; prior dedicated 3546):
//   ownerPresent && entityPresent && differentAllegiance
auto inlineShouldAddNotorietyMember3590(const bool ownerPresent, const bool entityPresent,
                                        const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

// Compact dual-wire pin matching Go pinShouldAddNotorietyMember3590 / C++ capacity
// direct AND-chain (formula unchanged from 2818 / 2959 / 3165 / 3267 / 3297 / 3327 / 3492 / 3546):
//   ownerPresent && entityPresent && differentAllegiance
auto pinShouldAddNotorietyMember3590(const bool ownerPresent, const bool entityPresent,
                                     const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

// Prior dedicated 3546 pin/inline for independence (formula identical).
auto inlineShouldAddNotorietyMember3546(const bool ownerPresent, const bool entityPresent,
                                        const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

auto pinShouldAddNotorietyMember3546(const bool ownerPresent, const bool entityPresent,
                                     const bool differentAllegiance) -> bool
{
    return ownerPresent && entityPresent && differentAllegiance;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldAddNotorietyMember
// (CNotorietyContainer::add admission three-bool AND;
// OmegaXI internal/notoriety; dedicated slice 3590 expand residual 2959 /
// pure 2818; prior dedicated expand 3546 / 3492 / 3327 / 3297 / 3267 / 3165 retained).
//
// Coverage:
//   - free == inline == pin == pin3546 direct AND-chain
//   - residual 2959 / 2818 pins still hold
//   - prior dedicated 3546 independence (free == prior inline == prior pin)
//   - dense 2^3 boolean space free == inline == pin == pin3546
//   - host inject poles + counterfactual pure poles
auto runNotorietyAddMember3590SelfTests() -> bool
{
    using notorietyhelpers::ShouldAddNotorietyMember;

    bool ok = true;

    // Residual 2818 / 2959 / prior dedicated 3546 pins still hold under dedicated dual-wire.
    ok = expect(ShouldAddNotorietyMember(true, true, true), "residual admit owner+entity+diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, true), "residual reject nil owner") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, true), "residual reject nil entity") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, true, false), "residual reject same allegiance") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, false, false), "residual reject all false") && ok;
    // Residual 2959 combined-failure poles.
    ok = expect(!ShouldAddNotorietyMember(false, false, true), "residual 2959 reject both nil +diff") && ok;
    ok = expect(!ShouldAddNotorietyMember(false, true, false), "residual 2959 reject nil owner same") && ok;
    ok = expect(!ShouldAddNotorietyMember(true, false, false), "residual 2959 reject nil entity same") && ok;

    // --- Core poles: free == inline == pin == pin3546 direct AND-chain ---
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

        // Prior dedicated 3546 re-pins.
        { true, true, true, true, "residual 3546 admit owner+entity+diff" },
        { false, true, true, false, "residual 3546 reject nil owner" },
        { true, false, true, false, "residual 3546 reject nil entity" },
        { true, true, false, false, "residual 3546 reject same allegiance" },
        { false, false, false, false, "residual 3546 reject all false" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAddNotorietyMember(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool inlineF = inlineShouldAddNotorietyMember3590(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool pin     = pinShouldAddNotorietyMember3590(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        const bool pin3546 = pinShouldAddNotorietyMember3546(c.ownerPresent, c.entityPresent, c.differentAllegiance);
        // Direct AND-chain pin composition (explicit three-bool AND).
        const bool wantPin = c.ownerPresent && c.entityPresent && c.differentAllegiance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == pin3546 && got == wantPin,
                    "ShouldAddNotorietyMember free == inline == pin == pin3546 direct AND-chain") &&
             ok;
    }

    // Free == pin == pin3546 across residual poles.
    ok = expect(ShouldAddNotorietyMember(true, true, true) == pinShouldAddNotorietyMember3590(true, true, true),
                "free == pin admit all true") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, true, true) == pinShouldAddNotorietyMember3546(true, true, true),
                "free == pin3546 admit all true") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(false, true, true) == pinShouldAddNotorietyMember3590(false, true, true),
                "free == pin nil owner") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(false, true, true) == pinShouldAddNotorietyMember3546(false, true, true),
                "free == pin3546 nil owner") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, false, true) == pinShouldAddNotorietyMember3590(true, false, true),
                "free == pin nil entity") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, false, true) == pinShouldAddNotorietyMember3546(true, false, true),
                "free == pin3546 nil entity") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, true, false) == pinShouldAddNotorietyMember3590(true, true, false),
                "free == pin same allegiance") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, true, false) == pinShouldAddNotorietyMember3546(true, true, false),
                "free == pin3546 same allegiance") &&
         ok;

    // Dense compose: full 2^3 boolean space free == inline == pin == pin3546.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            for (const bool differentAllegiance : { false, true })
            {
                const bool got     = ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance);
                const bool inlineF = inlineShouldAddNotorietyMember3590(ownerPresent, entityPresent, differentAllegiance);
                const bool pin     = pinShouldAddNotorietyMember3590(ownerPresent, entityPresent, differentAllegiance);
                const bool pin3546 = pinShouldAddNotorietyMember3546(ownerPresent, entityPresent, differentAllegiance);
                const bool want    = ownerPresent && entityPresent && differentAllegiance;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin && got == pin3546,
                            "compose free == inline == pin == pin3546") &&
                     ok;
            }
        }
    }

    // Prior dedicated 3546 independence: free still matches prior pin/inline.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool entityPresent : { false, true })
        {
            for (const bool differentAllegiance : { false, true })
            {
                const bool got         = ShouldAddNotorietyMember(ownerPresent, entityPresent, differentAllegiance);
                const bool priorInline = inlineShouldAddNotorietyMember3546(ownerPresent, entityPresent, differentAllegiance);
                const bool priorPin    = pinShouldAddNotorietyMember3546(ownerPresent, entityPresent, differentAllegiance);
                ok                     = expect(got == priorPin, "prior 3546 independence free == prior pin") && ok;
                ok                     = expect(got == priorInline, "prior 3546 independence free == prior inline") && ok;
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
    ok = expect(ShouldAddNotorietyMember(true, true, true) && pinShouldAddNotorietyMember3590(true, true, true),
                "add owner+entity+diff → insert path free/pin dual-wire") &&
         ok;
    ok = expect(pinShouldAddNotorietyMember3546(true, true, true),
                "add owner+entity+diff → insert path pin3546 dual-wire") &&
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
        const bool inlineF = inlineShouldAddNotorietyMember3590(p.owner, p.entity, p.diff);
        const bool pin     = pinShouldAddNotorietyMember3590(p.owner, p.entity, p.diff);
        const bool pin3546 = pinShouldAddNotorietyMember3546(p.owner, p.entity, p.diff);
        const bool want    = p.owner && p.entity && p.diff;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3546,
                    "host inject free == inline == pin == pin3546") &&
             ok;
    }

    return ok;
}
