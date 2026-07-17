#include "test_alliance_skip_del_party_2941.h"

#include "map/alliance_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance ShouldSkipDelPartyWhenEmpty 2941 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline delParty empty-alliance formula for dual-wire cross-check (slice 2941):
//   !hasAlliance || partyListEmpty
auto inlineShouldSkipDelPartyWhenEmpty(const bool hasAlliance, const bool partyListEmpty) -> bool
{
    return !hasAlliance || partyListEmpty;
}

} // namespace

// Pure dual-wire expansion for alliancehelpers::ShouldSkipDelPartyWhenEmpty
// (!hasAlliance || partyListEmpty; slice 2941).
auto runAllianceSkipDelParty2941SelfTests() -> bool
{
    using alliancehelpers::ShouldSkipDelPartyWhenEmpty;

    bool ok = true;

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

    // Dense compose: full 2^2 boolean space.
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

    // Residual 1341 pins still hold under dual-wire.
    ok = expect(ShouldSkipDelPartyWhenEmpty(false, false), "residual no alliance") && ok;
    ok = expect(ShouldSkipDelPartyWhenEmpty(true, true), "residual empty list") && ok;
    ok = expect(!ShouldSkipDelPartyWhenEmpty(true, false), "residual non-empty proceeds") && ok;

    return ok;
}
