#include "test_guildpoints_can_afford_2944.h"

#include "map/guild_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "guildpoints CanAfford 2944 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua guild_points getCurrency gate for dual-wire cross-check:
//   currency >= cost
auto inlineCanAfford(const int32 currency, const int32 cost) -> bool
{
    return currency >= cost;
}

} // namespace

// Pure dual-wire expansion for guildpointshelpers::CanAfford
// (Lua guild_points getCurrency >= cost purchase gate; slice 2944).
auto runGuildpointsCanAfford2944SelfTests() -> bool
{
    using guildpointshelpers::CanAfford;

    bool ok = true;

    const struct
    {
        int32       currency;
        int32       cost;
        bool        want;
        const char* label;
    } cases[] = {
        { 100, 100, true, "exact equality affords" },
        { 101, 100, true, "one over cost affords" },
        { 99, 100, false, "one under cost rejects" },
        { 0, 0, true, "zero/zero affords" },
        { 0, 1, false, "empty currency non-zero cost" },
        { 1, 0, true, "any currency zero cost" },
        { 30000, 30000, true, "fishing KI frog exact" },
        { 29999, 30000, false, "fishing KI frog short" },
        { 1500, 1500, true, "robber rig exact" },
        { 200, 200, true, "hq crystal unit exact" },
        { 1999, 2000, false, "10x crystal short" },
        { 2000, 2000, true, "10x crystal exact" },
        { -1, 0, false, "negative currency vs zero cost" },
        { -1, -1, true, "negative exact equality" },
        { -2, -1, false, "negative short" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAfford(c.currency, c.cost);
        const bool inlineF = inlineCanAfford(c.currency, c.cost);
        const bool wantPin = c.currency >= c.cost;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanAfford dual-wire == inline Lua formula") && ok;
        ok = expect(got == wantPin, "CanAfford == currency >= cost pin") && ok;
    }

    // Pin composition: free function is currency >= cost only.
    ok = expect(CanAfford(0, 0), "0>=0 must afford") && ok;
    ok = expect(!CanAfford(0, 1), "0>=1 must reject") && ok;
    ok = expect(CanAfford(1, 1), "1>=1 must afford") && ok;
    ok = expect(CanAfford(1, 0), "1>=0 must afford") && ok;
    ok = expect(!CanAfford(99, 100), "99>=100 must reject") && ok;
    ok = expect(CanAfford(100, 100), "100>=100 must afford") && ok;
    ok = expect(CanAfford(101, 100), "101>=100 must afford") && ok;

    // --- Production guild_points path semantics ---
    // Host injects:
    //   currency = player:getCurrency(currencyName)
    //   cost     = keyItem.cost | quantity*item.cost | quantity*crystal.cost
    // KI path: exact cost opens purchase; short rejects before delCurrency.
    ok = expect(CanAfford(30000, 30000), "KI exact opens purchase") && ok;
    ok = expect(!CanAfford(29999, 30000), "KI short rejects purchase") && ok;
    // Multi-buy item: quantity * unit cost.
    ok = expect(CanAfford(120000, 120000), "12x belt exact opens") && ok;
    ok = expect(!CanAfford(119999, 120000), "12x belt short rejects") && ok;
    // HQ crystal multi-buy.
    ok = expect(CanAfford(2000, 2000), "10x crystal exact opens") && ok;
    ok = expect(!CanAfford(1999, 2000), "10x crystal short rejects") && ok;

    // Dense host poles: free == pin == inline.
    const int32 poles[][2] = {
        { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 }, { 99, 100 }, { 100, 100 }, { 101, 100 },
    };
    for (const auto& p : poles)
    {
        const int32 currency = p[0];
        const int32 cost     = p[1];
        const bool  got      = CanAfford(currency, cost);
        const bool  want     = currency >= cost;
        ok                   = expect(got == want, "compose free == pin formula") && ok;
        ok                   = expect(got == inlineCanAfford(currency, cost), "compose free == inline") && ok;
    }

    // Residual 1016 pins still hold under dual-wire.
    ok = expect(CanAfford(100, 100), "residual exact") && ok;
    ok = expect(CanAfford(101, 100), "residual over") && ok;
    ok = expect(!CanAfford(99, 100), "residual short") && ok;
    ok = expect(CanAfford(0, 0), "residual zero/zero") && ok;

    return ok;
}
