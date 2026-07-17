#include "test_artisan_can_buy_sack_3090.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanBuySack 3090 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 1 pure gate for dual-wire cross-check:
//   getGil() >= 9980 and getContainerSize(MOGSACK) == 0
auto inlineCanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= 9980 && sackSize == 0;
}

// Compact dual-wire pin matching Go pinCanBuySack3090:
//   gil >= BuySackGilCost && sackSize == 0
auto pinCanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= artisanhelpers::BuySackGilCost && sackSize == 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanBuySack
// (Lua moogleOnUpdate option 1 buy-sack gate; OmegaXI internal/artisan;
// slice 3090).
//
// Coverage:
//   - gil under / at / over BuySackGilCost
//   - sackSize 0 vs non-zero
//   - free == inline == pin == gil>=cost && sack==0
//   - residual 0948 / 2879 pins still hold
auto runArtisanCanBuySack3090SelfTests() -> bool
{
    using artisanhelpers::BuySackGilCost;
    using artisanhelpers::CanBuySack;

    bool ok = true;

    // Residual constant / 0948 / 2879 pins still hold under dual-wire.
    ok = expect(BuySackGilCost == 9980, "BuySackGilCost == 9980") && ok;
    ok = expect(CanBuySack(9980, 0), "residual: exact gil empty may buy") && ok;
    ok = expect(CanBuySack(10000, 0), "residual: surplus gil empty may buy") && ok;
    ok = expect(!CanBuySack(9979, 0), "residual: short one gil must not buy") && ok;
    ok = expect(!CanBuySack(9980, 30), "residual: owns sack must not buy") && ok;

    // --- gil under / at / over cost; sackSize 0 vs non-zero ---
    const struct
    {
        int32       gil;
        int32       sackSize;
        bool        want;
        const char* label;
    } cases[] = {
        // gil under cost, empty sack
        { BuySackGilCost - 1, 0, false, "under cost, empty sack" },
        { 9979, 0, false, "literal under cost" },
        { 0, 0, false, "zero gil, empty sack" },
        { -1, 0, false, "negative gil, empty sack" },
        { 1, 0, false, "token gil, empty sack" },
        // gil at cost, empty sack
        { BuySackGilCost, 0, true, "at cost, empty sack" },
        { 9980, 0, true, "literal at cost" },
        // gil over cost, empty sack
        { BuySackGilCost + 1, 0, true, "over cost, empty sack" },
        { 10000, 0, true, "surplus gil, empty sack" },
        { 100000, 0, true, "rich, empty sack" },
        // sackSize non-zero blocks at any gil
        { BuySackGilCost, 1, false, "at cost, sack size 1" },
        { BuySackGilCost, 30, false, "at cost, owns sack 30" },
        { 100000, 1, false, "rich, sack size 1" },
        { 100000, 80, false, "rich, sack size 80" },
        { BuySackGilCost - 1, 30, false, "under cost and owns sack" },
        { BuySackGilCost, -1, false, "at cost, negative sack (not empty)" },
        // poles
        { BuySackGilCost - 50, 0, false, "well under, empty" },
        { BuySackGilCost + 50, 0, true, "well over, empty" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBuySack(c.gil, c.sackSize);
        const bool inlineF = inlineCanBuySack(c.gil, c.sackSize);
        const bool pinGot  = pinCanBuySack(c.gil, c.sackSize);
        const bool wantF   = c.gil >= BuySackGilCost && c.sackSize == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantF, "formula free==gil>=cost && sack==0") && ok;
    }

    // Free == inline pin matching C++ across residual poles.
    ok = expect(CanBuySack(BuySackGilCost, 0) == pinCanBuySack(BuySackGilCost, 0),
                "free==pin exact cost empty") &&
         ok;
    ok = expect(CanBuySack(BuySackGilCost - 1, 0) == pinCanBuySack(BuySackGilCost - 1, 0),
                "free==pin under cost empty") &&
         ok;
    ok = expect(CanBuySack(BuySackGilCost, 1) == pinCanBuySack(BuySackGilCost, 1),
                "free==pin non-zero sack") &&
         ok;

    // Dense gil boundary around cost with empty sack: free == inline == pin.
    for (int32 g = BuySackGilCost - 50; g <= BuySackGilCost + 50; ++g)
    {
        const bool got = CanBuySack(g, 0);
        ok             = expect(got == (g >= BuySackGilCost), "dense gil empty formula") && ok;
        ok             = expect(got == inlineCanBuySack(g, 0), "dense gil free==inline") && ok;
        ok             = expect(got == pinCanBuySack(g, 0), "dense gil free==pin") && ok;
    }

    // Non-empty sack always blocks regardless of gil.
    for (const int32 size : { 1, 2, 15, 30, 80, 100 })
    {
        ok = expect(!CanBuySack(BuySackGilCost, size), "non-empty sack blocks cost") && ok;
        ok = expect(!CanBuySack(100000, size), "non-empty sack blocks rich") && ok;
    }

    // Production moogleOnUpdate path semantics:
    // free → may delGil / changeContainerSize; fail → skip purchase.
    ok = expect(CanBuySack(BuySackGilCost, 0), "moogleOnUpdate free → purchase path") && ok;
    ok = expect(!CanBuySack(BuySackGilCost - 1, 0), "moogleOnUpdate under cost → skip") && ok;
    ok = expect(!CanBuySack(BuySackGilCost, 30), "moogleOnUpdate owns sack → skip") && ok;

    // sackSize 0 vs non-zero compose at under/at/over cost.
    for (const int32 size : { 0, 1, 2, 15, 30, 80, 100, -1 })
    {
        for (const int32 g : { BuySackGilCost - 1, BuySackGilCost, BuySackGilCost + 1, 100000 })
        {
            const bool got   = CanBuySack(g, size);
            const bool want  = g >= BuySackGilCost && size == 0;
            ok               = expect(got == want, "compose gil×size formula") && ok;
            ok               = expect(got == inlineCanBuySack(g, size), "compose free==inline") && ok;
            ok               = expect(got == pinCanBuySack(g, size), "compose free==pin") && ok;
        }
    }

    return ok;
}
