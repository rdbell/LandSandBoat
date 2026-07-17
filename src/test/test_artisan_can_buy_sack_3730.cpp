#include "test_artisan_can_buy_sack_3730.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanBuySack 3730 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 1 pure gate for dual-wire cross-check
// (slice 3730 dedicated expand residual 2879; prior dedicated 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090):
//   getGil() >= 9980 and getContainerSize(MOGSACK) == 0
// Direct return form matching production free function / capacity.
auto inlineCanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= 9980 && sackSize == 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3730).
// Direct return only — same formula as production CanBuySack.
auto pinCanBuySack3730(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= artisanhelpers::BuySackGilCost && sackSize == 0;
}

// Prior dedicated 3685 pin for independence cross-check.
auto pinCanBuySack3685(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= artisanhelpers::BuySackGilCost && sackSize == 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanBuySack
// (Lua moogleOnUpdate option 1 buy-sack gate; OmegaXI internal/artisan;
// slice 3730 dedicated expand residual 2879; prior dedicated 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3685 (direct return formula)
//   - residual poles: short gil, exact 9980, surplus, sack already owned
//   - dense edges around cost × sackSize 0 vs non-zero
//   - residual 0948 / 2879 / prior dedicated 3090 / prior expand 3242 /
//     prior expand 3282 / prior expand 3312 / prior expand 3493 /
//     prior expand 3550 / prior expand 3595 / prior expand 3640 /
//     prior expand 3685 pins still hold
//   - prior dedicated 3685 independence (free == pinCanBuySack3685)
// NOT registered in CMake/main.
auto runArtisanCanBuySack3730SelfTests() -> bool
{
    using artisanhelpers::BuySackGilCost;
    using artisanhelpers::CanBuySack;

    bool ok = true;

    // Residual constant / 0948 / 2879 / prior dedicated 3090 / 3242 / 3282 / 3312 / 3493 / 3550 / 3595 / 3640 / 3685 pins still hold.
    ok = expect(BuySackGilCost == 9980, "BuySackGilCost == 9980") && ok;
    ok = expect(CanBuySack(9980, 0), "residual: exact 9980 empty may buy") && ok;
    ok = expect(CanBuySack(10000, 0), "residual: surplus gil empty may buy") && ok;
    ok = expect(!CanBuySack(9979, 0), "residual: short gil must not buy") && ok;
    ok = expect(!CanBuySack(9980, 30), "residual: sack already owned must not buy") && ok;

    // Residual poles: free == inline == pin == pin3685 (direct return).
    const struct
    {
        int32       gil;
        int32       sackSize;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2879 / 3090 / 3242 / 3282 / 3312 / 3493 / 3550 / 3595 / 3640 / 3685 classic dual poles.
        { 9979, 0, false, "residual short gil" },
        { 9980, 0, true, "residual exact 9980 empty" },
        { 10000, 0, true, "residual surplus empty" },
        { 9980, 30, false, "residual sack already owned" },
        { 0, 0, false, "residual zero gil empty" },
        { -1, 0, false, "residual negative gil empty" },
        { 1, 0, false, "residual token gil empty" },
        { BuySackGilCost, 0, true, "residual cost empty" },
        { BuySackGilCost - 1, 0, false, "residual cost-1 empty" },
        { BuySackGilCost + 1, 0, true, "residual cost+1 empty" },
        { BuySackGilCost, 30, false, "residual owns sack 30" },
        { BuySackGilCost, 1, false, "residual at cost sack size 1" },
        { 100000, 1, false, "residual rich sack size 1" },
        { 100000, 80, false, "residual rich sack size 80" },
        { BuySackGilCost - 1, 30, false, "residual short gil and owns sack" },
        { BuySackGilCost, -1, false, "residual at cost negative sack" },
        { BuySackGilCost - 50, 0, false, "residual well under empty" },
        { BuySackGilCost + 50, 0, true, "residual well over empty" },

        // Prior dedicated 3090 / expand 3242 / expand 3282 / expand 3312 / expand 3493 / expand 3550 / expand 3595 / expand 3640 / expand 3685 dense poles.
        { 9979, 0, false, "prior literal under cost" },
        { 9980, 0, true, "prior literal at cost" },
        { 10000, 0, true, "prior surplus gil empty" },
        { 100000, 0, true, "prior rich empty" },
        { BuySackGilCost, 30, false, "prior at cost owns sack 30" },
        { BuySackGilCost - 100, 0, false, "prior well under empty" },
        { BuySackGilCost + 100, 0, true, "prior well over empty" },

        // Host inject path poles (moogleOnUpdate option 1).
        { BuySackGilCost, 0, true, "host exact cost empty → purchase" },
        { BuySackGilCost - 1, 0, false, "host short gil → skip" },
        { BuySackGilCost, 30, false, "host owns sack → skip" },
        { 100000, 0, true, "host surplus empty → purchase" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanBuySack(p.gil, p.sackSize);
        const bool inlineF = inlineCanBuySack(p.gil, p.sackSize);
        const bool pin     = pinCanBuySack3730(p.gil, p.sackSize);
        const bool prior   = pinCanBuySack3685(p.gil, p.sackSize);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanBuySack dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanBuySack == pin formula") && ok;
        ok = expect(got == prior, "CanBuySack == prior pinCanBuySack3685") && ok;
    }

    // Dense gil edges around cost with empty sack: free == inline == pin == pin3685.
    for (int32 g = BuySackGilCost - 50; g <= BuySackGilCost + 50; ++g)
    {
        const bool got     = CanBuySack(g, 0);
        const bool inlineF = inlineCanBuySack(g, 0);
        const bool pin     = pinCanBuySack3730(g, 0);
        const bool prior   = pinCanBuySack3685(g, 0);
        const bool want    = g >= BuySackGilCost;

        ok = expect(got == want, "dense gil empty formula") && ok;
        ok = expect(got == inlineF, "dense gil free==inline") && ok;
        ok = expect(got == pin, "dense gil free==pin") && ok;
        ok = expect(got == prior, "dense gil free==prior 3685") && ok;
    }

    // Dense edges: sackSize 0 vs non-zero at under/at/over/surplus gil.
    for (const int32 size : { 0, 1, 2, 15, 30, 80, 100, -1 })
    {
        for (const int32 g : { BuySackGilCost - 1, BuySackGilCost, BuySackGilCost + 1, 9979, 9980, 10000, 100000, 0, 1 })
        {
            const bool got     = CanBuySack(g, size);
            const bool inlineF = inlineCanBuySack(g, size);
            const bool pin     = pinCanBuySack3730(g, size);
            const bool prior   = pinCanBuySack3685(g, size);
            const bool want    = g >= BuySackGilCost && size == 0;

            ok = expect(got == want, "dense gil×size formula") && ok;
            ok = expect(got == inlineF, "dense gil×size free==inline") && ok;
            ok = expect(got == pin, "dense gil×size free==pin") && ok;
            ok = expect(got == prior, "dense gil×size free==prior 3685") && ok;
        }
    }

    // Non-empty sack always blocks regardless of gil.
    for (const int32 size : { 1, 2, 15, 30, 80, 100 })
    {
        ok = expect(!CanBuySack(BuySackGilCost, size), "non-empty sack blocks cost") && ok;
        ok = expect(!CanBuySack(100000, size), "non-empty sack blocks rich") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanBuySack(BuySackGilCost, 0), "exact cost empty must apply") && ok;
    ok = expect(CanBuySack(BuySackGilCost + 1, 0), "over cost empty must apply") && ok;
    ok = expect(!CanBuySack(BuySackGilCost - 1, 0), "under cost empty must reject") && ok;
    ok = expect(!CanBuySack(BuySackGilCost, 1), "exact cost non-empty must reject") && ok;
    ok = expect(!CanBuySack(BuySackGilCost, 30), "exact cost owns sack must reject") && ok;

    // Production moogleOnUpdate path semantics:
    // free → may delGil / changeContainerSize; fail → skip purchase.
    ok = expect(CanBuySack(BuySackGilCost, 0), "moogleOnUpdate free → purchase path") && ok;
    ok = expect(!CanBuySack(BuySackGilCost - 1, 0), "moogleOnUpdate under cost → skip") && ok;
    ok = expect(!CanBuySack(BuySackGilCost, 30), "moogleOnUpdate owns sack → skip") && ok;

    // Explicit dual-wire: free == inline == pin == pin3685 for host poles.
    const struct
    {
        int32       gil;
        int32       sackSize;
        const char* label;
    } hostPoles[] = {
        { BuySackGilCost, 0, "eligible purchase path" },
        { BuySackGilCost - 1, 0, "short gil skip" },
        { BuySackGilCost, 30, "owns sack skip" },
        { 10000, 0, "surplus empty purchase" },
        { 9979, 0, "literal short skip" },
        { 9980, 30, "literal owns sack skip" },
        { 100000, 80, "rich owns sack skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = CanBuySack(p.gil, p.sackSize);
        const bool inlineF = inlineCanBuySack(p.gil, p.sackSize);
        const bool pin     = pinCanBuySack3730(p.gil, p.sackSize);
        const bool prior   = pinCanBuySack3685(p.gil, p.sackSize);
        ok                 = expect(got == pin, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
        ok                 = expect(got == prior, "host inject free == prior pin 3685") && ok;
    }

    return ok;
}
