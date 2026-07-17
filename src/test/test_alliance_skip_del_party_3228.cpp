#include "test_alliance_skip_del_party_3228.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance ShouldSkipDelPartyWhenEmpty 3228 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline delParty empty-alliance formula for dual-wire cross-check (slice 3228):
//   !hasAlliance || partyListEmpty
// Positive form only (avoid De Morgan compound-not rewrites).
// Direct return of the expression is the preferred pin shape.
auto inlineShouldSkipDelPartyWhenEmpty(const bool hasAlliance, const bool partyListEmpty) -> bool
{
    return !hasAlliance || partyListEmpty;
}

} // namespace

// Pure dual-wire expand residual 2941 for alliancehelpers::ShouldSkipDelPartyWhenEmpty
// (!hasAlliance || partyListEmpty; slice 3228).
auto runAllianceSkipDelParty3228SelfTests() -> bool
{
    using alliancehelpers::ShouldSkipDelPartyWhenEmpty;

    bool ok = true;

    // Residual 1341 / prior dual-wire 2941 / prior expansion 3121 pins still hold.
    ok = expect(ShouldSkipDelPartyWhenEmpty(false, false), "residual: no alliance must skip") && ok;
    ok = expect(ShouldSkipDelPartyWhenEmpty(true, true), "residual: empty list must skip") && ok;
    ok = expect(!ShouldSkipDelPartyWhenEmpty(true, false), "residual: has alliance non-empty must proceed") && ok;

    const struct
    {
        bool        hasAlliance;
        bool        partyListEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // No alliance → always skip (regardless of list emptiness flag)
        { false, false, true, "no alliance non-empty flag skips" },
        { false, true, true, "no alliance empty flag skips" },

        // Has alliance + empty list → skip
        { true, true, true, "alliance empty list skips" },

        // Has alliance + non-empty list → proceed (do not skip)
        { true, false, false, "alliance non-empty proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipDelPartyWhenEmpty(c.hasAlliance, c.partyListEmpty);
        const bool inlineF = inlineShouldSkipDelPartyWhenEmpty(c.hasAlliance, c.partyListEmpty);
        // Positive pin form: !hasAlliance || partyListEmpty
        const bool wantPin = !c.hasAlliance || c.partyListEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipDelPartyWhenEmpty dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSkipDelPartyWhenEmpty == pin formula") && ok;
    }

    // Pin composition: OR of !hasAlliance and partyListEmpty.
    ok = expect(ShouldSkipDelPartyWhenEmpty(false, false), "no alliance must skip") && ok;
    ok = expect(ShouldSkipDelPartyWhenEmpty(false, true), "no alliance empty must skip") && ok;
    ok = expect(ShouldSkipDelPartyWhenEmpty(true, true), "empty list must skip") && ok;
    ok = expect(!ShouldSkipDelPartyWhenEmpty(true, false), "has alliance non-empty must proceed") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool hasAlliance : { false, true })
    {
        for (const bool partyListEmpty : { false, true })
        {
            const bool got  = ShouldSkipDelPartyWhenEmpty(hasAlliance, partyListEmpty);
            const bool want = !hasAlliance || partyListEmpty;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldSkipDelPartyWhenEmpty(hasAlliance, partyListEmpty),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production delParty path semantics ---
    // Host injects:
    //   hasAlliance    = party->m_PAlliance != nullptr
    //   partyListEmpty = hasAlliance && partyList.size()==0
    // so the (false, true) corner is not observed in production, but the pure
    // free function still returns true (skip) for any !hasAlliance input.
    ok = expect(ShouldSkipDelPartyWhenEmpty(false, false), "delParty no m_PAlliance → skip") && ok;
    ok = expect(ShouldSkipDelPartyWhenEmpty(true, true), "delParty empty partyList → skip") && ok;
    ok = expect(!ShouldSkipDelPartyWhenEmpty(true, false), "delParty non-empty → erase path") && ok;

    // removeParty null path remains orthogonal (skip gate is delParty-only).
    ok = expect(alliancehelpers::ClassifyRemoveAllianceParty(true, false, false) ==
                    alliancehelpers::remove_alliance_party_plan::NULL_PARTY,
                "removeParty null plan residual") &&
         ok;
    ok = expect(alliancehelpers::FormatDelPartyNullWarning() == "CAlliance::delParty - party is null!",
                "delParty null warning residual") &&
         ok;

    return ok;
}
