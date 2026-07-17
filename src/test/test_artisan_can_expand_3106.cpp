#include "test_artisan_can_expand_3106.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanExpand 3106 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 2 pure gate for dual-wire cross-check:
//   sackSize < gobbieSize and sackSize > 0
auto inlineCanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

// Compact dual-wire pin matching Go pinCanExpand3106:
//   sackSize < gobbieSize && sackSize > 0
auto pinCanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanExpand
// (Lua moogleOnUpdate option 2 expand-sack gate; OmegaXI internal/artisan;
// slice 3106).
//
// Coverage:
//   - sack under / at / over gobbie
//   - empty sack vs owned sack
//   - free == inline == pin == sackSize < gobbieSize && sackSize > 0
//   - residual 0948 / 2890 pins still hold
auto runArtisanCanExpand3106SelfTests() -> bool
{
    using artisanhelpers::CanExpand;

    bool ok = true;

    // Residual 0948 / 2890 pins still hold under dual-wire.
    ok = expect(CanExpand(30, 50), "residual: sack 30 < gobbie 50 may expand") && ok;
    ok = expect(!CanExpand(50, 50), "residual: equal sizes must not expand") && ok;
    ok = expect(!CanExpand(0, 50), "residual: empty sack must not expand") && ok;
    ok = expect(!CanExpand(60, 50), "residual: sack > gobbie must not expand") && ok;

    // --- sack under / at / over gobbie; empty vs owned ---
    const struct
    {
        int32       sackSize;
        int32       gobbieSize;
        bool        want;
        const char* label;
    } cases[] = {
        // residual 0948 poles
        { 30, 50, true, "residual sack 30 < gobbie 50" },
        { 50, 50, false, "residual equal sizes" },
        { 0, 50, false, "residual empty sack" },
        { 60, 50, false, "residual sack > gobbie" },
        // sack under gobbie, owned sack
        { 1, 2, true, "min owned sack, one slot behind" },
        { 29, 30, true, "one behind buy size" },
        { 30, 35, true, "buy size vs first upgrade" },
        { 50, 80, true, "mid sack, max gobbie" },
        { 79, 80, true, "one behind cap" },
        { 1, 100, true, "small sack, oversized gobbie" },
        // sack at gobbie (equal)
        { 1, 1, false, "min owned equal" },
        { 30, 30, false, "equal at buy size" },
        { 80, 80, false, "equal at gobbie cap" },
        // sack over gobbie
        { 35, 30, false, "sack ahead of gobbie" },
        { 80, 79, false, "sack at cap, gobbie below" },
        // empty sack always blocks
        { 0, 0, false, "both empty" },
        { 0, 1, false, "empty sack, inventory 1" },
        { 0, 80, false, "empty sack, full gobbie" },
        // negatives / poles
        { -1, 50, false, "negative sack not > 0" },
        { 10, -1, false, "negative gobbie blocks less-than" },
        { -5, -1, false, "both negative; sack not > 0" },
        { 1, 0, false, "sack 1 > gobbie 0" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanExpand(c.sackSize, c.gobbieSize);
        const bool inlineF = inlineCanExpand(c.sackSize, c.gobbieSize);
        const bool pinGot  = pinCanExpand(c.sackSize, c.gobbieSize);
        const bool wantF   = c.sackSize < c.gobbieSize && c.sackSize > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantF, "formula free==sack<gobbie && sack>0") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(CanExpand(30, 50) == pinCanExpand(30, 50), "free==pin sack 30 < gobbie 50") && ok;
    ok = expect(CanExpand(50, 50) == pinCanExpand(50, 50), "free==pin equal sizes") && ok;
    ok = expect(CanExpand(0, 50) == pinCanExpand(0, 50), "free==pin empty sack") && ok;
    ok = expect(CanExpand(60, 50) == pinCanExpand(60, 50), "free==pin sack > gobbie") && ok;

    // Empty sack always blocks regardless of gobbie size.
    for (const int32 gobbie : { 0, 1, 30, 50, 80, 100, -1 })
    {
        ok = expect(!CanExpand(0, gobbie), "empty sack blocks") && ok;
        ok = expect(!CanExpand(-1, gobbie), "negative sack blocks") && ok;
    }

    // Dense size grid: free == inline == pin == formula.
    for (int32 sack = -5; sack <= 90; ++sack)
    {
        for (int32 gobbie = -5; gobbie <= 90; ++gobbie)
        {
            const bool got  = CanExpand(sack, gobbie);
            const bool want = sack < gobbie && sack > 0;
            ok = expect(got == want, "dense CanExpand formula") && ok;
            ok = expect(got == inlineCanExpand(sack, gobbie), "dense free==inline") && ok;
            ok = expect(got == pinCanExpand(sack, gobbie), "dense free==pin") && ok;
        }
    }

    // Production moogleOnUpdate path semantics:
    // free → may changeContainerSize; fail → gobbieCanUpgrade / skip expand.
    ok = expect(CanExpand(30, 50), "moogleOnUpdate free → expand path") && ok;
    ok = expect(!CanExpand(50, 50), "moogleOnUpdate equal → skip expand") && ok;
    ok = expect(!CanExpand(0, 50), "moogleOnUpdate empty sack → skip expand") && ok;
    ok = expect(!CanExpand(60, 50), "moogleOnUpdate sack ahead → skip expand") && ok;

    return ok;
}
