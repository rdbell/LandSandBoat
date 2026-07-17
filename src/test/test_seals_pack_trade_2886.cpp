#include "test_seals_pack_trade_2886.h"

#include "map/seals_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "seals PackStoredPlusTrade 2886 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua bit.lshift(storedSeals + itemCount, 16) for dual-wire cross-check:
//   (storedSeals + itemCount) << 16
auto inlinePackStoredPlusTrade(const int32 storedSeals, const int32 itemCount) -> int32
{
    return (storedSeals + itemCount) << 16;
}

} // namespace

// Pure dual-wire expansion for sealshelpers::PackStoredPlusTrade
// (Lua xi.seals.onTrade event-param pack after getSeals / trade qty inject).
auto runSealsPackTrade2886SelfTests() -> bool
{
    using sealshelpers::PackStoredPlusTrade;

    bool ok = true;

    // Primary pins for PackStoredPlusTrade (residual 0961 + dense table).
    ok = expect(PackStoredPlusTrade(0, 0) == 0, "zero zero") && ok;
    ok = expect(PackStoredPlusTrade(0, 1) == (1 << 16), "zero trade one") && ok;
    ok = expect(PackStoredPlusTrade(10, 0) == (10 << 16), "stored only") && ok;
    ok = expect(PackStoredPlusTrade(0, 5) == (5 << 16), "trade only") && ok;
    ok = expect(PackStoredPlusTrade(10, 5) == (15 << 16), "stored 10 trade 5") && ok;
    ok = expect(PackStoredPlusTrade(0, 1) == 65536, "unit residual pin") && ok;
    ok = expect(PackStoredPlusTrade(10, 5) == 983040, "sum fifteen residual") && ok;

    // Dual-wire matches inline formula across a table.
    const struct
    {
        int32       storedSeals;
        int32       itemCount;
        int32       want;
        const char* label;
    } cases[] = {
        { 0, 0, 0, "table zero zero" },
        { 0, 1, 1 << 16, "table zero trade one" },
        { 10, 0, 10 << 16, "table stored only" },
        { 0, 5, 5 << 16, "table trade only" },
        { 10, 5, 15 << 16, "table stored 10 trade 5" },
        { 1000, 0, 1000 << 16, "table large stored" },
        { 0, 999, 999 << 16, "table large trade" },
        { 500, 250, 750 << 16, "table mixed large" },
        { 0, 99, 99 << 16, "table single seal deposit" },
        { 200, 3, 203 << 16, "table already stocked" },
        { 7, 3, 10 << 16, "table seven plus three" },
        { 50, 50, 100 << 16, "table fifty fifty" },
    };

    for (const auto& c : cases)
    {
        const int32 got       = PackStoredPlusTrade(c.storedSeals, c.itemCount);
        const int32 inlineGot = inlinePackStoredPlusTrade(c.storedSeals, c.itemCount);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: eventParams[sealID + 2] = PackStoredPlusTrade(stored, count).
    // Seal IDs 0..4 map to param slots 2..6 (EventParamIndex).
    const struct
    {
        int32       sealID;
        int32       storedSeals;
        int32       itemCount;
        int32       wantIndex;
        int32       wantPacked;
        const char* label;
    } composeCases[] = {
        { 0, 0, 1, 2, 1 << 16, "compose sealID0 beastmens" },
        { 1, 10, 5, 3, 15 << 16, "compose sealID1 kindreds" },
        { 2, 0, 0, 4, 0, "compose sealID2 crest zero" },
        { 3, 50, 50, 5, 100 << 16, "compose sealID3 high" },
        { 4, 7, 3, 6, 10 << 16, "compose sealID4 sacred" },
    };

    for (const auto& c : composeCases)
    {
        const int32 idx       = c.sealID + 2;
        const int32 packed    = PackStoredPlusTrade(c.storedSeals, c.itemCount);
        const int32 inlineGot = inlinePackStoredPlusTrade(c.storedSeals, c.itemCount);

        ok = expect(idx == c.wantIndex, c.label) && ok;
        ok = expect(packed == c.wantPacked, c.label) && ok;
        ok = expect(packed == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
