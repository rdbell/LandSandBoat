#include "test_notoriety_has_enmity_after_prune_3034.h"

#include "map/notoriety_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "notoriety HasEnmityAfterPrune 3034 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CNotorietyContainer::hasEnmity final return for dual-wire
// cross-check (slice 3034):
//   !lookupEmpty
auto inlineHasEnmityAfterPrune(const bool lookupEmpty) -> bool
{
    return !lookupEmpty;
}

} // namespace

// Pure dual-wire expansion for notorietyhelpers::HasEnmityAfterPrune
// (CNotorietyContainer::hasEnmity final empty report; slice 3034).
auto runNotorietyHasEnmityAfterPrune3034SelfTests() -> bool
{
    using notorietyhelpers::HasEnmityAfterPrune;

    bool ok = true;

    // Residual 2832 pins still hold under dual-wire.
    ok = expect(HasEnmityAfterPrune(false), "residual non-empty reports enmity") && ok;
    ok = expect(!HasEnmityAfterPrune(true), "residual empty reports no enmity") && ok;

    const struct
    {
        bool        lookupEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic poles — formula is NOT of empty-state.
        { false, true, "non-empty reports enmity" },
        { true, false, "empty reports no enmity" },
    };

    for (const auto& c : cases)
    {
        const bool got     = HasEnmityAfterPrune(c.lookupEmpty);
        const bool inlineF = inlineHasEnmityAfterPrune(c.lookupEmpty);
        const bool wantPin = !c.lookupEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "HasEnmityAfterPrune dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "HasEnmityAfterPrune == pin formula !lookupEmpty") && ok;
    }

    // Pin composition: both poles of the single bool.
    ok = expect(HasEnmityAfterPrune(false), "lookupEmpty false must report enmity") && ok;
    ok = expect(!HasEnmityAfterPrune(true), "lookupEmpty true must report no enmity") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool lookupEmpty : { false, true })
    {
        const bool got  = HasEnmityAfterPrune(lookupEmpty);
        const bool want = !lookupEmpty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineHasEnmityAfterPrune(lookupEmpty), "compose free == inline") && ok;
    }

    // --- Production CNotorietyContainer::hasEnmity final return path semantics ---
    // Host injects:
    //   lookupEmpty = m_Lookup.empty()  (after optional prune walk)
    // when true  → no remaining reverse-enmity members
    // when false → at least one entry remains
    ok = expect(HasEnmityAfterPrune(false), "hasEnmity non-empty → true") && ok;
    ok = expect(!HasEnmityAfterPrune(true), "hasEnmity empty → false") && ok;

    // Explicit dual-wire: free function is NOT of the empty inject.
    for (const bool lookupEmpty : { false, true })
    {
        ok = expect(HasEnmityAfterPrune(lookupEmpty) == !lookupEmpty, "host inject identity") && ok;
        ok = expect(HasEnmityAfterPrune(lookupEmpty) == inlineHasEnmityAfterPrune(lookupEmpty),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    return ok;
}
