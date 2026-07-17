#include "test_portermoogle_can_retrieve_item_3171.h"

#include "map/porter_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "portermoogle CanRetrieveItem 3171 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua porter_moogle hasItem / freeSlots gate for dual-wire cross-check
// (dedicated slice 3171):
//   !hasItem && freeSlots > 0
auto inlineCanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

// Compact dual-wire pin matching Go pinCanRetrieveItem3171 / C++ capacity:
//   !hasItem && freeSlots > 0
auto pinCanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

} // namespace

// Pure dual-wire expansion for portermooglehelpers::CanRetrieveItem
// (Lua porter_moogle hasItem + freeSlots retrieve gate; dedicated slice 3171
// expand residual 2966 / pure 1144).
//
// Coverage:
//   - free == inline == pin == (!hasItem && freeSlots > 0)
//   - residual 1144 / 2966 pins still hold
//   - poles: hasItem true/false × freeSlots 0 / -1 / 1 (plus multi free)
auto runPortermoogleCanRetrieveItem3171SelfTests() -> bool
{
    using portermooglehelpers::CanRetrieveItem;

    bool ok = true;

    // Residual 1144 / 2966 pins still hold under dual-wire.
    ok = expect(CanRetrieveItem(false, 1), "residual free slot + not owned allows") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "residual hasItem rejects") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "residual zero freeSlots rejects") && ok;

    // --- Composition table: free == inline == pin ---
    // Required poles: hasItem, freeSlots 0 / -1 / 1.
    const struct
    {
        bool        hasItem;
        int32       freeSlots;
        bool        want;
        const char* label;
    } cases[] = {
        // Required poles: freeSlots 0 / -1 / 1 × hasItem.
        { false, 1, true, "not owned freeSlots=1 allows" },
        { true, 1, false, "owned freeSlots=1 rejects" },
        { false, 0, false, "not owned freeSlots=0 rejects" },
        { true, 0, false, "owned freeSlots=0 rejects" },
        { false, -1, false, "not owned freeSlots=-1 rejects" },
        { true, -1, false, "owned freeSlots=-1 rejects" },

        // Residual 2966 happy / multi free.
        { false, 2, true, "not owned multi free allows" },
        { false, 80, true, "not owned full free-slot count allows" },
        { true, 80, false, "owned multi free rejects" },

        // Exact freeSlots>0 boundary (positive form).
        { false, 1, true, "exact freeSlots>0 boundary allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRetrieveItem(c.hasItem, c.freeSlots);
        const bool inlineF = inlineCanRetrieveItem(c.hasItem, c.freeSlots);
        const bool pin     = pinCanRetrieveItem(c.hasItem, c.freeSlots);
        const bool wantPin = !c.hasItem && c.freeSlots > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "CanRetrieveItem dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "CanRetrieveItem == pin formula") && ok;
    }

    // Pin composition: AND of !hasItem and freeSlots > 0.
    ok = expect(CanRetrieveItem(false, 1), "!hasItem && freeSlots>0 must allow") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "hasItem must reject") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "freeSlots==0 must reject") && ok;
    ok = expect(!CanRetrieveItem(true, 0), "hasItem and zero free must reject") && ok;
    ok = expect(!CanRetrieveItem(false, -1), "freeSlots==-1 must reject") && ok;
    ok = expect(!CanRetrieveItem(true, -1), "hasItem and freeSlots==-1 must reject") && ok;

    // Dense compose: bool × freeSlots poles free == inline == pin.
    const int32 freePoles[] = { -1, 0, 1, 2, 80 };
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got     = CanRetrieveItem(hasItem, freeSlots);
            const bool inlineF = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin     = pinCanRetrieveItem(hasItem, freeSlots);
            const bool want    = !hasItem && freeSlots > 0;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // --- Production onEventUpdate path semantics ---
    // Host injects:
    //   hasItem   = player:hasItem(retrievedItemId)
    //   freeSlots = player:getFreeSlotsCount()
    // if !CanRetrieveItem → messageSpecial(ITEM_CANNOT_BE_OBTAINED)
    // else → clear bit / retrieve path
    ok = expect(CanRetrieveItem(false, 1), "host inject retrieve open") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "host inject inventory full rejects") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "host inject already owns rejects") && ok;
    ok = expect(!CanRetrieveItem(true, 0), "host inject owns and full rejects") && ok;
    ok = expect(CanRetrieveItem(false, 80), "host inject plenty free opens") && ok;
    ok = expect(!CanRetrieveItem(false, -1), "host inject negative free rejects") && ok;
    ok = expect(!CanRetrieveItem(true, -1), "host inject owned negative free rejects") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got     = CanRetrieveItem(hasItem, freeSlots);
            const bool inlineF = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin     = pinCanRetrieveItem(hasItem, freeSlots);
            const bool want    = !hasItem && freeSlots > 0;
            ok                 = expect(got == want, "host inject free == pin") && ok;
            ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
        }
    }

    // Buy-slip residual polarity: hasSlip || freeSlots <= 0 ≡ !CanRetrieveItem.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : { -1, 0, 1, 2 })
        {
            const bool can       = CanRetrieveItem(hasItem, freeSlots);
            const bool buyReject = hasItem || freeSlots <= 0;
            const bool inlineF   = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin       = pinCanRetrieveItem(hasItem, freeSlots);
            ok                   = expect(can != buyReject, "buy polarity residual") && ok;
            ok                   = expect(can == inlineF && can == pin, "buy polarity free == inline == pin") && ok;
        }
    }

    return ok;
}
