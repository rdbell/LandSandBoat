#include "test_item_matches_search_item_3038.h"

#include "map/item_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer MatchesSearchItem 3038 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SearchItem / SearchItems loop-body gate for dual-wire cross-check
// (slice 3038):
//   slotOccupied && idMatches
auto inlineMatchesSearchItem(const bool slotOccupied, const bool idMatches) -> bool
{
    return slotOccupied && idMatches;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::MatchesSearchItem
// (SearchItem / SearchItems loop-body gate; slice 3038).
auto runItemMatchesSearchItem3038SelfTests() -> bool
{
    using itemcontainerhelpers::CanSearchSlotID;
    using itemcontainerhelpers::MatchesSearchItem;

    bool ok = true;

    // Residual 2823 pins still hold under dual-wire.
    ok = expect(MatchesSearchItem(true, true), "residual: match occupied id hit") && ok;
    ok = expect(!MatchesSearchItem(false, true), "residual: match empty slot") && ok;
    ok = expect(!MatchesSearchItem(true, false), "residual: match id mismatch") && ok;
    ok = expect(!MatchesSearchItem(false, false), "residual: match empty and id fail") && ok;

    const struct
    {
        bool        slotOccupied;
        bool        idMatches;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, true, "occupied id hit accepted" },
        { false, false, false, "empty id fail rejected" },

        // Full truth table.
        { true, true, true, "occupied && idMatches" },
        { true, false, false, "occupied id mismatch" },
        { false, true, false, "empty id would-match" },
        { false, false, false, "empty id mismatch" },

        // Residual 2823 poles.
        { true, true, true, "residual match occupied id hit" },
        { false, true, false, "residual match empty slot" },
        { true, false, false, "residual match id mismatch" },
        { false, false, false, "residual match empty and id fail" },
    };

    for (const auto& c : cases)
    {
        const bool got     = MatchesSearchItem(c.slotOccupied, c.idMatches);
        const bool inlineF = inlineMatchesSearchItem(c.slotOccupied, c.idMatches);
        const bool wantPin = c.slotOccupied && c.idMatches;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "MatchesSearchItem dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "MatchesSearchItem == pin formula occupied && idMatches") && ok;
    }

    // Pin composition: only occupied && idMatches.
    ok = expect(MatchesSearchItem(true, true), "occupied id hit must accept") && ok;
    ok = expect(!MatchesSearchItem(false, true), "empty must reject even when idMatches") && ok;
    ok = expect(!MatchesSearchItem(true, false), "id mismatch must reject even when occupied") && ok;
    ok = expect(!MatchesSearchItem(false, false), "empty id fail must reject") && ok;

    // Dense compose: full boolean domain free == inline == pin.
    for (const bool occupied : { false, true })
    {
        for (const bool idMatches : { false, true })
        {
            const bool got  = MatchesSearchItem(occupied, idMatches);
            const bool want = occupied && idMatches;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineMatchesSearchItem(occupied, idMatches), "compose free == inline") && ok;
        }
    }

    // Host-style inject poles: occupancy + id match after null short-circuit.
    // (Live CItemContainer SearchItem scan is residual 2823 / item_container tests.)
    // Production path: nullptr short-circuit then MatchesSearchItem(true, id==).
    for (const auto& pair : {
             std::pair<bool, bool>{ true, true },
             std::pair<bool, bool>{ true, false },
             std::pair<bool, bool>{ false, true },
             std::pair<bool, bool>{ false, false },
         })
    {
        const bool occupied  = pair.first;
        const bool idMatches = pair.second;
        const bool inject    = occupied && idMatches; // mirrors host inject
        ok                   = expect(MatchesSearchItem(occupied, idMatches) == inject, "host inject dual-wire identity") && ok;
        ok                   = expect(MatchesSearchItem(occupied, idMatches) == inlineMatchesSearchItem(occupied, idMatches),
                    "host inject free == inline") &&
             ok;
    }

    // Production SearchItem path semantics:
    // occupied && idMatches → return slotId
    // else → continue / ERROR_SLOTID
    ok = expect(MatchesSearchItem(true, true), "SearchItem hit → accept path") && ok;
    ok = expect(!MatchesSearchItem(true, false), "SearchItem id miss → reject") && ok;
    ok = expect(!MatchesSearchItem(false, true), "SearchItem empty → reject") && ok;
    ok = expect(!MatchesSearchItem(false, false), "SearchItem empty id fail → reject") && ok;

    // Sibling range gate (3033) remains independent of match body.
    ok = expect(CanSearchSlotID(0, 3), "CanSearchSlotID slot 0 scan domain") && ok;
    ok = expect(CanSearchSlotID(3, 3), "CanSearchSlotID boundary scan domain") && ok;
    ok = expect(!CanSearchSlotID(4, 3), "CanSearchSlotID past size out of domain") && ok;

    return ok;
}
