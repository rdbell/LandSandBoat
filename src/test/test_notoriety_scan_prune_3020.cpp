#include "test_notoriety_scan_prune_3020.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety ShouldScanNotorietyForPrune 3020 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::hasEnmity outer gate for dual-wire cross-check
// (slice 3020):
//   ownerPresent && lookupNonEmpty
auto inlineShouldScanNotorietyForPrune(const bool ownerPresent, const bool lookupNonEmpty) -> bool
{
    return ownerPresent && lookupNonEmpty;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::ShouldScanNotorietyForPrune
// (CNotorietyContainer::hasEnmity outer gate two-bool AND; slice 3020).
auto runNotorietyScanPrune3020SelfTests() -> bool
{
    using notorietyhelpers::ShouldScanNotorietyForPrune;

    bool ok = true;

    // Residual 2807 pins still hold under dual-wire.
    ok = expect(ShouldScanNotorietyForPrune(true, true), "residual scan owner+nonempty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true), "residual no scan nil owner") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false), "residual no scan empty") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false), "residual no scan nil+empty") && ok;

    const struct
    {
        bool        ownerPresent;
        bool        lookupNonEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — both true.
        { true, true, true, "scan owner+nonempty" },

        // Residual 2807 single-false poles.
        { false, true, false, "no scan nil owner" },
        { true, false, false, "no scan empty" },
        { false, false, false, "no scan nil+empty" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldScanNotorietyForPrune(c.ownerPresent, c.lookupNonEmpty);
        const bool inlineF = inlineShouldScanNotorietyForPrune(c.ownerPresent, c.lookupNonEmpty);
        const bool wantPin = c.ownerPresent && c.lookupNonEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldScanNotorietyForPrune dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldScanNotorietyForPrune == pin formula AND of two") && ok;
    }

    // Pin composition: only the full-true pole scans.
    ok = expect(ShouldScanNotorietyForPrune(true, true), "all true must scan") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true), "ownerPresent false must skip") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false), "lookupNonEmpty false must skip") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false), "both false must skip") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool lookupNonEmpty : { false, true })
        {
            const bool got  = ShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty);
            const bool want = ownerPresent && lookupNonEmpty;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CNotorietyContainer::hasEnmity outer gate path semantics ---
    // Host injects:
    //   ownerPresent   = m_POwner != nullptr
    //   lookupNonEmpty = !m_Lookup.empty()
    // when true  → walk m_Lookup and prune stale mobs
    // when false → skip prune walk; report !lookup.empty()
    ok = expect(ShouldScanNotorietyForPrune(true, true), "hasEnmity owner+nonempty → prune walk") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, true), "hasEnmity nil owner → skip walk") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(true, false), "hasEnmity empty lookup → skip walk") && ok;
    ok = expect(!ShouldScanNotorietyForPrune(false, false), "hasEnmity nil+empty → skip walk") && ok;

    // Explicit dual-wire: free function is the two-bool AND of injects.
    for (const bool ownerPresent : { false, true })
    {
        for (const bool lookupNonEmpty : { false, true })
        {
            ok = expect(ShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty) ==
                            (ownerPresent && lookupNonEmpty),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty) ==
                            inlineShouldScanNotorietyForPrune(ownerPresent, lookupNonEmpty),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    return ok;
}
