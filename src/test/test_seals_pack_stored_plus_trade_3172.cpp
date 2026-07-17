#include "test_seals_pack_stored_plus_trade_3172.h"

#include "map/seals_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "seals PackStoredPlusTrade 3172 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua bit.lshift(storedSeals + itemCount, 16) for dual-wire cross-check
// (dedicated 3172):
//   (storedSeals + itemCount) << 16
auto inlinePackStoredPlusTrade3172(const int32 storedSeals, const int32 itemCount) -> int32
{
    return (storedSeals + itemCount) << 16;
}

// Compact dual-wire pin matching Go pinPackStoredPlusTrade3172 / C++ capacity
// positive form (formula unchanged from 0961 / 2886):
//   (storedSeals + itemCount) << 16
auto pinPackStoredPlusTrade3172(const int32 storedSeals, const int32 itemCount) -> int32
{
    return (storedSeals + itemCount) << 16;
}

} // namespace

// Pure dual-wire expansion for sealshelpers::PackStoredPlusTrade
// (Lua xi.seals.onTrade event-param pack after getSeals / trade qty inject;
// OmegaXI internal/seals; dedicated slice 3172; residual expand 2886 / pure 0961).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2886 / 0961 pins still hold
//   - poles: zeros, positives, shift semantics
//   - host inject poles (eventParams[sealID + 2])
auto runSealsPackStoredPlusTrade3172SelfTests() -> bool
{
    using sealshelpers::PackStoredPlusTrade;

    bool ok = true;

    // Residual 0961 pins still hold under dedicated dual-wire.
    ok = expect(PackStoredPlusTrade(10, 5) == (15 << 16), "residual 0961 stored 10 trade 5") && ok;
    ok = expect(PackStoredPlusTrade(0, 1) == (1 << 16), "residual 0961 zero trade one") && ok;
    ok = expect(PackStoredPlusTrade(0, 0) == 0, "residual 0961 zero zero") && ok;
    ok = expect(PackStoredPlusTrade(0, 1) == 65536, "residual 0961 unit absolute pin") && ok;
    ok = expect(PackStoredPlusTrade(10, 5) == 983040, "residual 0961 sum-fifteen absolute pin") && ok;

    // Residual 2886 poles still hold.
    ok = expect(PackStoredPlusTrade(10, 0) == (10 << 16), "residual 2886 stored only") && ok;
    ok = expect(PackStoredPlusTrade(0, 5) == (5 << 16), "residual 2886 trade only") && ok;
    ok = expect(PackStoredPlusTrade(500, 250) == (750 << 16), "residual 2886 mixed large") && ok;
    ok = expect(PackStoredPlusTrade(200, 3) == (203 << 16), "residual 2886 already stocked") && ok;

    // --- Core poles: free == inline == pin positive form ---
    // Covers zeros, positives, and shift semantics (low 16 bits clear).
    const struct
    {
        int32       storedSeals;
        int32       itemCount;
        int32       want;
        const char* label;
    } cases[] = {
        // Zero poles.
        { 0, 0, 0, "zero zero" },
        { 0, 1, 1 << 16, "zero trade one" },
        { 1, 0, 1 << 16, "stored one trade zero" },

        // Residual 0961 absolute pins.
        { 0, 1, 65536, "unit residual pin" },
        { 10, 5, 983040, "sum fifteen residual" },

        // Positive poles.
        { 10, 0, 10 << 16, "stored only" },
        { 0, 5, 5 << 16, "trade only" },
        { 10, 5, 15 << 16, "stored 10 trade 5" },
        { 0, 99, 99 << 16, "single seal deposit" },
        { 200, 3, 203 << 16, "already stocked" },
        { 1000, 0, 1000 << 16, "large stored" },
        { 0, 999, 999 << 16, "large trade" },
        { 500, 250, 750 << 16, "mixed large" },
        { 7, 3, 10 << 16, "seven plus three" },
        { 50, 50, 100 << 16, "fifty fifty" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = PackStoredPlusTrade(c.storedSeals, c.itemCount);
        const int32 inlineF = inlinePackStoredPlusTrade3172(c.storedSeals, c.itemCount);
        const int32 pin     = pinPackStoredPlusTrade3172(c.storedSeals, c.itemCount);
        // Positive form pin composition (explicit sum then left-shift).
        const int32 wantPin = (c.storedSeals + c.itemCount) << 16;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "PackStoredPlusTrade free == inline == pin positive form") &&
             ok;

        // Shift semantics: packed word occupies high half only.
        if (c.storedSeals >= 0 && c.itemCount >= 0)
        {
            const int32 sum = c.storedSeals + c.itemCount;
            if (sum >= 0 && sum < (1 << 15))
            {
                ok = expect((got & 0xFFFF) == 0, "shift semantics low 16 bits clear") && ok;
                ok = expect((got >> 16) == sum, "shift semantics high half == sum") && ok;
            }
        }
    }

    // Free == pin across residual poles.
    ok = expect(PackStoredPlusTrade(0, 0) == pinPackStoredPlusTrade3172(0, 0),
                "free == pin zero zero") &&
         ok;
    ok = expect(PackStoredPlusTrade(0, 1) == pinPackStoredPlusTrade3172(0, 1),
                "free == pin unit residual") &&
         ok;
    ok = expect(PackStoredPlusTrade(10, 5) == pinPackStoredPlusTrade3172(10, 5),
                "free == pin sum fifteen residual") &&
         ok;
    ok = expect(PackStoredPlusTrade(500, 250) == pinPackStoredPlusTrade3172(500, 250),
                "free == pin mixed large") &&
         ok;

    // Shift semantics poles: free/inline/pin and high-half identity.
    const struct
    {
        int32       stored;
        int32       count;
        const char* label;
    } shiftPoles[] = {
        { 0, 0, "shift zero" },
        { 0, 1, "shift trade one" },
        { 1, 0, "shift stored one" },
        { 1, 1, "shift both one" },
        { 0xFF, 1, "shift 0xFF+1" },
        { 0x100, 0, "shift 0x100" },
        { 0x7FFF, 0, "shift max safe half" },
    };

    for (const auto& p : shiftPoles)
    {
        const int32 got     = PackStoredPlusTrade(p.stored, p.count);
        const int32 inlineF = inlinePackStoredPlusTrade3172(p.stored, p.count);
        const int32 pin     = pinPackStoredPlusTrade3172(p.stored, p.count);
        const int32 want    = (p.stored + p.count) << 16;

        ok = expect(got == want, p.label) && ok;
        ok = expect(got == inlineF && got == pin, "shift free == inline == pin") && ok;
        ok = expect((got & 0xFFFF) == 0, "shift low 16 bits clear") && ok;
        ok = expect((got >> 16) == (p.stored + p.count), "shift high half == sum") && ok;
    }

    // Host inject poles: getSeals + trade qty → eventParams[sealID + 2].
    // Seal IDs 0..4 map to param slots 2..6 (EventParamIndex).
    const struct
    {
        int32       sealID;
        int32       storedSeals;
        int32       itemCount;
        int32       wantIndex;
        int32       wantPacked;
        const char* label;
    } hostPoles[] = {
        { 0, 0, 1, 2, 1 << 16, "host beastmens deposit" },
        { 1, 10, 5, 3, 15 << 16, "host kindreds stocked+trade" },
        { 2, 0, 0, 4, 0, "host crest empty" },
        { 3, 50, 50, 5, 100 << 16, "host high fifty fifty" },
        { 4, 7, 3, 6, 10 << 16, "host sacred seven three" },
    };

    for (const auto& p : hostPoles)
    {
        const int32 idx     = p.sealID + 2;
        const int32 packed  = PackStoredPlusTrade(p.storedSeals, p.itemCount);
        const int32 inlineF = inlinePackStoredPlusTrade3172(p.storedSeals, p.itemCount);
        const int32 pin     = pinPackStoredPlusTrade3172(p.storedSeals, p.itemCount);

        ok = expect(idx == p.wantIndex, p.label) && ok;
        ok = expect(packed == p.wantPacked, p.label) && ok;
        ok = expect(packed == inlineF && packed == pin, "host free == inline == pin") && ok;
    }

    return ok;
}
