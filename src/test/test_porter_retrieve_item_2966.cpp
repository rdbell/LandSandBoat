#include "test_porter_retrieve_item_2966.h"

#include "map/porter_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "porter CanRetrieveItem 2966 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua porter_moogle hasItem / freeSlots gate for dual-wire cross-check
// (slice 2966):
//   !hasItem && freeSlots > 0
auto inlineCanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

} // namespace

// Pure dual-wire expansion for portermooglehelpers::CanRetrieveItem
// (Lua porter_moogle hasItem + freeSlots retrieve gate; slice 2966).
auto runPorterRetrieveItem2966SelfTests() -> bool
{
    using portermooglehelpers::CanRetrieveItem;

    bool ok = true;

    // Residual 1144 pins still hold under dual-wire.
    ok = expect(CanRetrieveItem(false, 1), "residual free slot + not owned allows") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "residual hasItem rejects") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "residual zero freeSlots rejects") && ok;

    const struct
    {
        bool        hasItem;
        int32       freeSlots;
        bool        want;
        const char* label;
    } cases[] = {
        // Happy path: not owned + free inventory
        { false, 1, true, "not owned one free allows" },
        { false, 2, true, "not owned multi free allows" },
        { false, 80, true, "not owned full free-slot count allows" },

        // Already owns item → cannot obtain even with free slots
        { true, 1, false, "owned one free rejects" },
        { true, 80, false, "owned multi free rejects" },
        { true, 0, false, "owned zero free rejects" },

        // No free slots → cannot obtain even when not owned
        { false, 0, false, "not owned zero free rejects" },

        // Boundary / defensive freeSlots poles
        { false, -1, false, "not owned negative free rejects" },
        { true, -1, false, "owned negative free rejects" },
        { false, 1, true, "exact freeSlots>0 boundary allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRetrieveItem(c.hasItem, c.freeSlots);
        const bool inlineF = inlineCanRetrieveItem(c.hasItem, c.freeSlots);
        const bool wantPin = !c.hasItem && c.freeSlots > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRetrieveItem dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "CanRetrieveItem == pin formula") && ok;
    }

    // Pin composition: AND of !hasItem and freeSlots > 0.
    ok = expect(CanRetrieveItem(false, 1), "!hasItem && freeSlots>0 must allow") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "hasItem must reject") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "freeSlots==0 must reject") && ok;
    ok = expect(!CanRetrieveItem(true, 0), "hasItem and zero free must reject") && ok;

    // Dense compose: bool × freeSlots poles.
    const int32 freePoles[] = { -1, 0, 1, 2, 80 };
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got  = CanRetrieveItem(hasItem, freeSlots);
            const bool want = !hasItem && freeSlots > 0;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanRetrieveItem(hasItem, freeSlots),
                        "compose free == inline") &&
                 ok;
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

    // Explicit dual-wire: free == !hasItem && freeSlots > 0 for host poles.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got  = CanRetrieveItem(hasItem, freeSlots);
            const bool want = !hasItem && freeSlots > 0;
            ok              = expect(got == want, "host inject free == pin") && ok;
            ok              = expect(got == inlineCanRetrieveItem(hasItem, freeSlots),
                        "host inject free == inline") &&
                 ok;
        }
    }

    // Buy-slip residual polarity: hasSlip || freeSlots <= 0 ≡ !CanRetrieveItem.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : { -1, 0, 1, 2 })
        {
            const bool can       = CanRetrieveItem(hasItem, freeSlots);
            const bool buyReject = hasItem || freeSlots <= 0;
            ok                   = expect(can != buyReject, "buy polarity residual") && ok;
        }
    }

    return ok;
}
