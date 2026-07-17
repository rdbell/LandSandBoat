#include "test_portermoogle_can_retrieve_item_3568.h"

#include "map/porter_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "portermoogle CanRetrieveItem 3568 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua porter_moogle hasItem / freeSlots gate for dual-wire cross-check
// (dedicated slice 3568):
//   !hasItem && freeSlots > 0
auto inlineCanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

// Compact dual-wire pin matching Go pinCanRetrieveItem3568 / C++ capacity
// CanRetrieveItem direct-return form:
//   !hasItem && freeSlots > 0
auto pinCanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

// Compact dual-wire pin matching Go pinCanRetrieveItem3512 (prior dedicated):
//   !hasItem && freeSlots > 0
auto pinCanRetrieveItem3512(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

} // namespace

// Pure dual-wire expansion for portermooglehelpers::CanRetrieveItem
// (Lua porter_moogle hasItem + freeSlots retrieve gate; dedicated slice 3568
// expand residual 2966 / prior dedicated ~3512 / 3463 / 3409 / 3316 / 3286 / 3243 / 3171 / pure 1144 —
// formula unchanged).
//
// Coverage:
//   - free == inline == pin == pin3512 == (!hasItem && freeSlots > 0) (pin direct return)
//   - residual 1144 / 2966 / prior 3171 / 3243 / 3286 / 3316 / 3409 / 3463 / 3512 pins still hold
//   - residual poles + dense 2×slots: hasItem × freeSlots {0, 1, n, negative}
auto runPortermoogleCanRetrieveItem3568SelfTests() -> bool
{
    using portermooglehelpers::CanRetrieveItem;

    bool ok = true;

    // Residual 1144 / 2966 / prior 3171 / 3243 / 3286 / 3316 / 3409 / 3463 / 3512 pins still hold under dual-wire.
    ok = expect(CanRetrieveItem(false, 1), "residual free slot + not owned allows") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "residual hasItem rejects") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "residual zero freeSlots rejects") && ok;
    ok = expect(!CanRetrieveItem(false, -1), "residual negative freeSlots rejects") && ok;
    ok = expect(!CanRetrieveItem(true, -1), "residual owned + negative freeSlots rejects") && ok;

    // --- Composition table: free == inline == pin == pin3512 ---
    // Required poles: hasItem × freeSlots 0 / -1 / 1 / n.
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

        // Residual 2966 / prior 3171 / 3243 / 3286 / 3316 / 3409 / 3463 / 3512 happy / multi free (n).
        { false, 2, true, "not owned multi free allows" },
        { false, 80, true, "not owned full free-slot count allows" },
        { true, 2, false, "owned multi free rejects" },
        { true, 80, false, "owned full free-slot count rejects" },

        // Exact freeSlots>0 boundary (positive form).
        { false, 1, true, "exact freeSlots>0 boundary allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRetrieveItem(c.hasItem, c.freeSlots);
        const bool inlineF = inlineCanRetrieveItem(c.hasItem, c.freeSlots);
        const bool pin     = pinCanRetrieveItem(c.hasItem, c.freeSlots);
        const bool pin3512 = pinCanRetrieveItem3512(c.hasItem, c.freeSlots);
        const bool wantPin = !c.hasItem && c.freeSlots > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == pin3512, "CanRetrieveItem dual-wire free == inline == pin == pin3512") && ok;
        ok = expect(got == wantPin, "CanRetrieveItem == pin formula") && ok;
    }

    // Pin composition: AND of !hasItem and freeSlots > 0.
    ok = expect(CanRetrieveItem(false, 1), "!hasItem && freeSlots>0 must allow") && ok;
    ok = expect(!CanRetrieveItem(true, 1), "hasItem must reject") && ok;
    ok = expect(!CanRetrieveItem(false, 0), "freeSlots==0 must reject") && ok;
    ok = expect(!CanRetrieveItem(true, 0), "hasItem and zero free must reject") && ok;
    ok = expect(!CanRetrieveItem(false, -1), "freeSlots==-1 must reject") && ok;
    ok = expect(!CanRetrieveItem(true, -1), "hasItem and freeSlots==-1 must reject") && ok;

    // Dense 2×slots: hasItem × freeSlots {0, 1, n, negative} free == inline == pin == pin3512.
    // n covers multi free (2) and production-scale free count (80).
    const int32 freePoles[] = { 0, 1, 2, 80, -1 };
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got     = CanRetrieveItem(hasItem, freeSlots);
            const bool inlineF = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin     = pinCanRetrieveItem(hasItem, freeSlots);
            const bool pin3512 = pinCanRetrieveItem3512(hasItem, freeSlots);
            const bool want    = !hasItem && freeSlots > 0;
            ok                 = expect(got == want, "dense 2xslots free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin && got == pin3512, "dense 2xslots free == inline == pin == pin3512") && ok;
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
    ok = expect(CanRetrieveItem(false, 2), "host inject multi free opens") && ok;
    ok = expect(!CanRetrieveItem(false, -1), "host inject negative free rejects") && ok;
    ok = expect(!CanRetrieveItem(true, -1), "host inject owned negative free rejects") && ok;
    ok = expect(!CanRetrieveItem(true, 2), "host inject owned multi free rejects") && ok;

    // Explicit dual-wire: free == inline == pin == pin3512 for host poles.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool got     = CanRetrieveItem(hasItem, freeSlots);
            const bool inlineF = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin     = pinCanRetrieveItem(hasItem, freeSlots);
            const bool pin3512 = pinCanRetrieveItem3512(hasItem, freeSlots);
            const bool want    = !hasItem && freeSlots > 0;
            ok                 = expect(got == want, "host inject free == pin") && ok;
            ok                 = expect(got == inlineF && got == pin && got == pin3512, "host inject free == inline == pin == pin3512") && ok;
        }
    }

    // Buy-slip residual polarity: hasSlip || freeSlots <= 0 ≡ !CanRetrieveItem.
    for (const bool hasItem : { false, true })
    {
        for (const int32 freeSlots : freePoles)
        {
            const bool can       = CanRetrieveItem(hasItem, freeSlots);
            const bool buyReject = hasItem || freeSlots <= 0;
            const bool inlineF   = inlineCanRetrieveItem(hasItem, freeSlots);
            const bool pin       = pinCanRetrieveItem(hasItem, freeSlots);
            const bool pin3512   = pinCanRetrieveItem3512(hasItem, freeSlots);
            ok                   = expect(can != buyReject, "buy polarity residual") && ok;
            ok                   = expect(can == inlineF && can == pin && can == pin3512, "buy polarity free == inline == pin == pin3512") && ok;
        }
    }

    return ok;
}
