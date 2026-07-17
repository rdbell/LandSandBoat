#include "test_notoriety_scan_prune_3385.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldScanNotorietyForPrune 3385 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::hasEnmity outer gate for dual-wire cross-check
// (dedicated 3385 expand residual 3020):
//   ownerPresent && lookupNonEmpty
auto inlineShouldScanNotorietyForPrune(const bool ownerPresent, const bool lookupNonEmpty) -> bool
{
    return ownerPresent && lookupNonEmpty;
}

// Compact dual-wire pin matching Go pinShouldScanNotorietyForPrune3385 /
// C++ capacity two-bool AND form (formula unchanged from 2807 / 3020):
//   ownerPresent && lookupNonEmpty
auto pinShouldScanNotorietyForPrune(const bool ownerPresent, const bool lookupNonEmpty) -> bool
{
    return ownerPresent && lookupNonEmpty;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldScanNotorietyForPrune
// (CNotorietyContainer::hasEnmity outer gate two-bool AND;
// OmegaXI internal/notoriety; dedicated slice 3385 expand residual 3020 /
// pure 2807).
//
// Coverage:
//   - free == inline == pin residual pins
//   - poles: owner+nonempty / nil owner / empty / both false
//   - residual 2807 / 3020 pins still hold
//   - dense 2^2 free == inline == pin residual pins
//   - host-style inject + residual independence (add/remove/prune/hasEnmity
//     left alone — do not thrash add_member)
auto runNotorietyScanPrune3385SelfTests() -> bool
{
    using notorietyhelpers::HasEnmityAfterPrune;
    using notorietyhelpers::ShouldAddNotorietyMember;
    using notorietyhelpers::ShouldPruneMobFromNotoriety;
    using notorietyhelpers::ShouldRemoveNotorietyMember;
    using notorietyhelpers::ShouldScanNotorietyForPrune;

    bool ok = true;

    // Residual 2807 / prior dual-wire 3020 pins still hold under dedicated expand.
    ok = expect(ShouldScanNotorietyForPrune(true, true), "residual: scan owner+nonempty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true), "residual: no scan nil owner") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false), "residual: no scan empty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false), "residual: no scan nil+empty") && ok;

    // --- Core poles: free == inline == pin residual pins ---
    const struct
    {
        bool        ownerPresent;
        bool        lookupNonEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — both true → host may prune-walk.
        { true, true, true, "scan owner+nonempty" },

        // Single-false poles skip the prune walk.
        { false, true, false, "no scan nil owner" },
        { true, false, false, "no scan empty" },
        { false, false, false, "no scan nil+empty" },

        // Residual 2807 / 3020 re-pins.
        { true, true, true, "residual 2807 scan owner+nonempty" },
        { false, true, false, "residual 2807 no scan nil owner" },
        { true, false, false, "residual 2807 no scan empty" },
        { false, false, false, "residual 2807 no scan nil+empty" },
        { true, true, true, "residual 3020 scan owner+nonempty" },
        { false, true, false, "residual 3020 no scan nil owner" },
        { true, false, false, "residual 3020 no scan empty" },
        { false, false, false, "residual 3020 no scan nil+empty" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldScanNotorietyForPrune(c.ownerPresent, c.lookupNonEmpty);
        const bool inlineF = inlineShouldScanNotorietyForPrune(c.ownerPresent, c.lookupNonEmpty);
        const bool pinGot  = pinShouldScanNotorietyForPrune(c.ownerPresent, c.lookupNonEmpty);
        // Two-bool AND pin composition (explicit).
        const bool wantPin = c.ownerPresent && c.lookupNonEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==AND pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldScanNotorietyForPrune(true, true) == pinShouldScanNotorietyForPrune(true, true),
                "free==pin owner+nonempty") &&
         ok;
    ok = expect(ShouldScanNotorietyForPrune(false, true) == pinShouldScanNotorietyForPrune(false, true),
                "free==pin nil owner") &&
         ok;
    ok = expect(ShouldScanNotorietyForPrune(true, false) == pinShouldScanNotorietyForPrune(true, false),
                "free==pin empty lookup") &&
         ok;
    ok = expect(ShouldScanNotorietyForPrune(false, false) == pinShouldScanNotorietyForPrune(false, false),
                "free==pin nil+empty") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin residual pins.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool lookupNonEmpty : { false, true })
        {
            const bool got     = ShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty);
            const bool inlineF = inlineShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty);
            const bool pinGot  = pinShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty);
            const bool want    = ownerPresent && lookupNonEmpty;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
            ok                 = expect(got == want, "compose free==AND pin form") && ok;
        }
    }

    // Host-style inject poles: CNotorietyContainer::hasEnmity outer gate.
    //   ownerPresent   = m_POwner != nullptr
    //   lookupNonEmpty = !m_Lookup.empty()
    // when true  → walk m_Lookup and prune stale mobs
    // when false → skip prune walk; report !lookup.empty()
    const struct
    {
        bool        owner;
        bool        lookupNonEmpty;
        bool        wantScan;
        const char* label;
    } hostCases[] = {
        { true, true, true, "hasEnmity owner+nonempty → prune walk" },
        { false, true, false, "hasEnmity nil owner → skip walk" },
        { true, false, false, "hasEnmity empty lookup → skip walk" },
        { false, false, false, "hasEnmity nil+empty → skip walk" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldScanNotorietyForPrune(c.owner, c.lookupNonEmpty);
        const bool inlineF = inlineShouldScanNotorietyForPrune(c.owner, c.lookupNonEmpty);
        const bool pinGot  = pinShouldScanNotorietyForPrune(c.owner, c.lookupNonEmpty);

        ok = expect(got == c.wantScan, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production path semantics.
    ok = expect(ShouldScanNotorietyForPrune(true, true) && pinShouldScanNotorietyForPrune(true, true),
                "owner+nonempty → scan") &&
         ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true) && !pinShouldScanNotorietyForPrune(false, true),
                "nil owner → no scan") &&
         ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false) && !pinShouldScanNotorietyForPrune(true, false),
                "empty lookup → no scan") &&
         ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false) && !pinShouldScanNotorietyForPrune(false, false),
                "both false → no scan") &&
         ok;

    // Residual independence (2807 / 2818 / 2819 / 2832 / dual-wire 2959 / 2971 /
    // 3020 / 3029 / 3034 / 3165 / 3192 / 3208 / 3267 / 3297 / 3327): outer scan
    // is distinct from per-entry prune, add, remove, and hasEnmity-after-prune.
    // Siblings left alone beyond pins — do not thrash add_member.
    ok = expect(ShouldScanNotorietyForPrune(true, true),
                "scan gate must accept when owner present and lookup non-empty") &&
         ok;
    ok = expect(!ShouldPruneMobFromNotoriety(false, true, false, true) &&
                    ShouldPruneMobFromNotoriety(true, true, false, true),
                "per-entry prune residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldAddNotorietyMember(true, true, true) && !ShouldAddNotorietyMember(true, true, false),
                "add residual still holds under dual-wire") &&
         ok;
    ok = expect(ShouldRemoveNotorietyMember(true, true) && !ShouldRemoveNotorietyMember(true, false),
                "remove residual still holds under dual-wire") &&
         ok;
    ok = expect(HasEnmityAfterPrune(false) && !HasEnmityAfterPrune(true),
                "hasEnmity-after-prune residual still holds under dual-wire") &&
         ok;

    return ok;
}
