#include "test_guildpoints_can_afford_3368.h"

#include "map/guild_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "guildpoints CanAfford 3368 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua guild_points getCurrency gate for dual-wire cross-check
// (dedicated 3368 expand residual 2944; prior 3221):
//   currency >= cost
auto inlineCanAfford(const int32 currency, const int32 cost) -> bool
{
    return currency >= cost;
}

// Compact dual-wire pin matching Go pinCanAfford3368 / C++ capacity:
//   currency >= cost
auto pinCanAfford(const int32 currency, const int32 cost) -> bool
{
    return currency >= cost;
}

} // namespace

// Pure dual-wire expansion for guildpointshelpers::CanAfford
// (Lua guild_points getCurrency >= cost purchase gate; OmegaXI
// internal/guildpoints; dedicated slice 3368 expand residual 2944;
// prior dedicated 3221 retained).
//
// Coverage:
//   - free == inline == pin == (currency >= cost)
//   - residual 2944 / 3221 / 1016 pins still hold
//   - poles: equal, short, surplus, zero cost, negative edges
//   - dense: 0/0, 0/1, 1/0, 1/1, 100/99, 100/100, 100/101, MaxInt-ish
auto runGuildpointsCanAfford3368SelfTests() -> bool
{
    using guildpointshelpers::CanAfford;

    bool ok = true;

    // Residual 2944 / 3221 / 1016 pins still hold under dual-wire.
    ok = expect(CanAfford(100, 100), "residual exact equality") && ok;
    ok = expect(CanAfford(101, 100), "residual surplus") && ok;
    ok = expect(!CanAfford(99, 100), "residual short") && ok;
    ok = expect(CanAfford(0, 0), "residual zero/zero") && ok;
    ok = expect(!CanAfford(0, 1), "residual empty non-zero cost") && ok;
    ok = expect(CanAfford(1, 0), "residual zero cost") && ok;

    constexpr int32 kMaxInt = std::numeric_limits<int32>::max();
    constexpr int32 kMinInt = std::numeric_limits<int32>::min();

    // --- Composition table: free == inline == pin ---
    const struct
    {
        int32       currency;
        int32       cost;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2944 / 3221 poles.
        { 100, 100, true, "residual exact equality" },
        { 101, 100, true, "residual surplus" },
        { 99, 100, false, "residual short" },
        { 0, 0, true, "residual zero/zero" },
        { 0, 1, false, "residual empty non-zero cost" },
        { 1, 0, true, "residual zero cost" },
        { 30000, 30000, true, "residual fishing KI frog exact" },
        { 29999, 30000, false, "residual fishing KI frog short" },
        { 1500, 1500, true, "residual robber rig exact" },
        { 200, 200, true, "residual hq crystal unit exact" },
        { 1999, 2000, false, "residual 10x crystal short" },
        { 2000, 2000, true, "residual 10x crystal exact" },
        { 120000, 120000, true, "residual 12x belt multi-buy exact" },
        { 119999, 120000, false, "residual 12x belt multi-buy short" },

        // Residual poles: equal, short, surplus, zero cost.
        { 50, 50, true, "pole equal" },
        { 49, 50, false, "pole short" },
        { 51, 50, true, "pole surplus" },
        { 999, 0, true, "pole zero cost surplus currency" },
        { 0, 0, true, "pole zero cost zero currency" },

        // Negative edges (int32 allows).
        { -1, 0, false, "negative currency vs zero cost" },
        { 0, -1, true, "zero currency vs negative cost" },
        { -1, -1, true, "negative exact equality" },
        { -2, -1, false, "negative short" },
        { -1, -2, true, "negative surplus" },
        { 1, -1, true, "positive vs negative cost" },
        { -100, 1, false, "large negative currency" },

        // Dense host poles (required).
        { 0, 0, true, "dense 0/0" },
        { 0, 1, false, "dense 0/1" },
        { 1, 0, true, "dense 1/0" },
        { 1, 1, true, "dense 1/1" },
        { 100, 99, true, "dense 100/99" },
        { 100, 100, true, "dense 100/100" },
        { 100, 101, false, "dense 100/101" },

        // MaxInt-ish values (careful; avoid overflow).
        { kMaxInt, kMaxInt, true, "MaxInt exact" },
        { kMaxInt - 1, kMaxInt, false, "MaxInt short" },
        { kMaxInt, kMaxInt - 1, true, "MaxInt surplus" },
        { kMinInt, kMinInt, true, "MinInt exact" },
        { kMinInt, kMinInt + 1, false, "MinInt short" },
        { kMinInt + 1, kMinInt, true, "MinInt surplus" },
        { 1 << 30, 1 << 30, true, "1<<30 exact" },
        { (1 << 30) - 1, 1 << 30, false, "1<<30 short" },
        { 1 << 30, (1 << 30) - 1, true, "1<<30 surplus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanAfford(c.currency, c.cost);
        const bool inlineF = inlineCanAfford(c.currency, c.cost);
        const bool pin     = pinCanAfford(c.currency, c.cost);
        const bool wantPin = c.currency >= c.cost;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula currency >= cost") && ok;
    }

    // Explicit residual poles free == inline == pin.
    const struct
    {
        int32       currency;
        int32       cost;
        const char* label;
    } poles[] = {
        { 100, 100, "equal" },
        { 99, 100, "short" },
        { 101, 100, "surplus" },
        { 5, 0, "zero cost" },
        { 0, 0, "currency 0 cost 0" },
        { -1, 0, "currency -1 cost 0" },
        { kMaxInt, kMaxInt, "MaxInt equal" },
        { kMaxInt - 1, kMaxInt, "MaxInt short" },
    };
    for (const auto& p : poles)
    {
        const bool got     = CanAfford(p.currency, p.cost);
        const bool inlineF = inlineCanAfford(p.currency, p.cost);
        const bool pin     = pinCanAfford(p.currency, p.cost);
        const bool want    = p.currency >= p.cost;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose free == inline == pin over required pairs + neighbors.
    const int32 dense[][2] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
        { 100, 99 },
        { 100, 100 },
        { 100, 101 },
        { 99, 100 },
        { 101, 100 },
        { -1, 0 },
        { 0, -1 },
        { -1, -1 },
        { kMaxInt, kMaxInt },
        { kMaxInt - 1, kMaxInt },
        { kMaxInt, kMaxInt - 1 },
        { 1 << 30, 1 << 30 },
        { (1 << 30) - 1, 1 << 30 },
    };
    for (const auto& p : dense)
    {
        const int32 currency = p[0];
        const int32 cost     = p[1];
        const bool  got      = CanAfford(currency, cost);
        const bool  inlineF  = inlineCanAfford(currency, cost);
        const bool  pin      = pinCanAfford(currency, cost);
        const bool  want     = currency >= cost;
        ok                   = expect(got == want, "dense free == pin formula") && ok;
        ok                   = expect(got == inlineF && got == pin, "dense free == inline == pin") && ok;
    }

    // Host-style guild_points purchase inject poles.
    const int32 inject[][2] = {
        { 30000, 30000 },
        { 29999, 30000 },
        { 120000, 120000 },
        { 119999, 120000 },
        { 2000, 2000 },
        { 1999, 2000 },
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
        { 100, 100 },
        { 99, 100 },
        { 101, 100 },
    };
    for (const auto& p : inject)
    {
        const int32 currency = p[0];
        const int32 cost     = p[1];
        const bool  got      = CanAfford(currency, cost);
        const bool  inlineF  = inlineCanAfford(currency, cost);
        const bool  pin      = pinCanAfford(currency, cost);
        const bool  want     = currency >= cost;
        ok                   = expect(got == want, "inject free == pin formula") && ok;
        ok                   = expect(got == inlineF && got == pin, "inject free == inline == pin") && ok;
    }

    // Production path semantics still hold.
    ok = expect(CanAfford(0, 0), "0>=0 must afford") && ok;
    ok = expect(!CanAfford(0, 1), "0>=1 must reject") && ok;
    ok = expect(CanAfford(1, 1), "1>=1 must afford") && ok;
    ok = expect(CanAfford(1, 0), "1>=0 must afford") && ok;
    ok = expect(!CanAfford(99, 100), "99>=100 must reject") && ok;
    ok = expect(CanAfford(100, 100), "100>=100 must afford") && ok;
    ok = expect(CanAfford(101, 100), "101>=100 must afford") && ok;

    return ok;
}
