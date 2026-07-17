#include "test_artisan_buy_sack_2879.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanBuySack 2879 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 1 pure gate for dual-wire cross-check:
//   getGil() >= 9980 and getContainerSize(MOGSACK) == 0
auto inlineCanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= 9980 && sackSize == 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanBuySack
// (artisan.lua moogleOnUpdate option 1 buy-sack gate).
auto runArtisanBuySack2879SelfTests() -> bool
{
    using artisanhelpers::BuySackGilCost;
    using artisanhelpers::CanBuySack;

    bool ok = true;

    // Constant pin: 9980 from artisan.lua delGil / getGil compare.
    ok = expect(BuySackGilCost == 9980, "BuySackGilCost == 9980") && ok;

    const struct
    {
        int32       gil;
        int32       sackSize;
        bool        want;
        const char* label;
    } cases[] = {
        { 9980, 0, true, "exact gil, empty sack" },
        { 10000, 0, true, "extra gil, empty sack" },
        { 9979, 0, false, "short one gil" },
        { 0, 0, false, "zero gil" },
        { -1, 0, false, "negative gil" },
        { 9980, 30, false, "exact gil, owns sack 30" },
        { 10000, 1, false, "extra gil, sack size 1" },
        { 9979, 30, false, "short gil and owns sack" },
        { 9980, -1, false, "exact gil, negative sack (not empty)" },
        { 1, 0, false, "token gil only" },
        { 9980, 0, true, "residual 0948 exact pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBuySack(c.gil, c.sackSize);
        const bool inlineF = inlineCanBuySack(c.gil, c.sackSize);
        const bool wantF   = c.gil >= BuySackGilCost && c.sackSize == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline 9980/empty") && ok;
        ok = expect(got == wantF, "dual-wire free == gil>=cost && sack==0") && ok;
    }

    // Residual 0948 pins.
    ok = expect(CanBuySack(9980, 0), "residual CanBuySack(9980, 0)") && ok;
    ok = expect(CanBuySack(10000, 0), "residual CanBuySack(10000, 0)") && ok;
    ok = expect(!CanBuySack(9979, 0), "residual CanBuySack(9979, 0)") && ok;
    ok = expect(!CanBuySack(9980, 30), "residual CanBuySack(9980, 30)") && ok;

    // Boundary: cost - 1 fails, cost succeeds, cost + 1 succeeds (empty sack).
    ok = expect(!CanBuySack(BuySackGilCost - 1, 0), "cost-1 fails") && ok;
    ok = expect(CanBuySack(BuySackGilCost, 0), "cost exact ok") && ok;
    ok = expect(CanBuySack(BuySackGilCost + 1, 0), "cost+1 ok") && ok;

    // Any non-zero sack size fails even with abundant gil.
    for (const int32 size : { 1, 2, 15, 30, 80, 100 })
    {
        ok = expect(!CanBuySack(BuySackGilCost, size), "non-empty sack blocks") && ok;
        ok = expect(!CanBuySack(100000, size), "non-empty sack blocks rich") && ok;
    }

    // Dense gil boundary around cost with empty sack.
    for (int32 g = BuySackGilCost - 50; g <= BuySackGilCost + 50; ++g)
    {
        const bool got  = CanBuySack(g, 0);
        const bool want = g >= BuySackGilCost;
        ok = expect(got == want, "dense gil empty-sack boundary") && ok;
        ok = expect(got == inlineCanBuySack(g, 0), "dense gil dual-wire == inline") && ok;
    }

    return ok;
}
