#include "test_artisan_can_expand_3363.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanExpand 3363 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 2 pure gate for dual-wire cross-check
// (slice 3363 dedicated expand residual 2890; prior dedicated 3106):
//   sackSize < gobbieSize and sackSize > 0
// Direct return form matching production free function / capacity.
auto inlineCanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3363).
// Direct return only — same formula as production CanExpand.
auto pinCanExpand3363(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanExpand
// (Lua moogleOnUpdate option 2 expand-sack gate; OmegaXI internal/artisan;
// slice 3363 dedicated expand residual 2890; prior dedicated 3106).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual poles: sack under / at / over gobbie; empty vs owned
//   - dense size grid
//   - residual 0948 / 2890 / prior dedicated 3106 pins still hold
auto runArtisanCanExpand3363SelfTests() -> bool
{
    using artisanhelpers::CanExpand;

    bool ok = true;

    // Residual 0948 / 2890 / prior dedicated 3106 pins still hold under dual-wire.
    ok = expect(CanExpand(30, 50), "residual: sack 30 < gobbie 50 may expand") && ok;
    ok = expect(!CanExpand(50, 50), "residual: equal sizes must not expand") && ok;
    ok = expect(!CanExpand(0, 50), "residual: empty sack must not expand") && ok;
    ok = expect(!CanExpand(60, 50), "residual: sack > gobbie must not expand") && ok;

    // Residual poles: free == inline == pin (direct return).
    const struct
    {
        int32       sackSize;
        int32       gobbieSize;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 0948 / 2890 classic dual poles.
        { 30, 50, true, "residual sack 30 < gobbie 50" },
        { 50, 50, false, "residual equal sizes" },
        { 0, 50, false, "residual empty sack" },
        { 60, 50, false, "residual sack > gobbie" },
        { 1, 2, true, "residual min owned, one slot behind" },
        { 1, 1, false, "residual min owned equal" },
        { 0, 0, false, "residual both empty" },
        { 0, 1, false, "residual empty sack, inventory 1" },
        { 0, 80, false, "residual empty sack, full gobbie" },
        { 29, 30, true, "residual one behind buy size" },
        { 30, 30, false, "residual equal at buy size" },
        { 30, 35, true, "residual buy size vs first upgrade" },
        { 35, 30, false, "residual sack ahead of gobbie" },
        { 79, 80, true, "residual one behind cap" },
        { 80, 80, false, "residual equal at gobbie cap" },
        { 80, 79, false, "residual sack at cap, gobbie below" },
        { -1, 50, false, "residual negative sack not > 0" },
        { 10, -1, false, "residual negative gobbie blocks less-than" },
        { -5, -1, false, "residual both negative; sack not > 0" },
        { 1, 0, false, "residual sack 1 > gobbie 0" },
        { 50, 80, true, "residual mid sack, max gobbie" },
        { 1, 100, true, "residual small sack, oversized gobbie" },

        // Prior dedicated 3106 dense poles.
        { 30, 50, true, "prior sack under gobbie" },
        { 50, 50, false, "prior sack at gobbie" },
        { 60, 50, false, "prior sack over gobbie" },
        { 0, 50, false, "prior empty sack" },
        { 1, 2, true, "prior min owned one behind" },
        { 79, 80, true, "prior one behind cap" },
        { 80, 80, false, "prior equal at cap" },

        // Host inject path poles (moogleOnUpdate option 2).
        { 30, 50, true, "host sack behind → expand" },
        { 50, 50, false, "host equal → skip expand" },
        { 0, 50, false, "host empty sack → skip expand" },
        { 60, 50, false, "host sack ahead → skip expand" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanExpand(p.sackSize, p.gobbieSize);
        const bool inlineF = inlineCanExpand(p.sackSize, p.gobbieSize);
        const bool pin     = pinCanExpand3363(p.sackSize, p.gobbieSize);
        const bool wantF   = p.sackSize < p.gobbieSize && p.sackSize > 0;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanExpand dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "CanExpand == pin formula") && ok;
        ok = expect(got == wantF, "formula free==sack<gobbie && sack>0") && ok;
    }

    // Empty sack always blocks regardless of gobbie size.
    for (const int32 gobbie : { 0, 1, 30, 50, 80, 100, -1 })
    {
        ok = expect(!CanExpand(0, gobbie), "empty sack blocks") && ok;
        ok = expect(!CanExpand(-1, gobbie), "negative sack blocks") && ok;
        ok = expect(CanExpand(0, gobbie) == pinCanExpand3363(0, gobbie), "empty free==pin") && ok;
    }

    // Dense size grid: free == inline == pin == formula.
    for (int32 sack = -5; sack <= 90; ++sack)
    {
        for (int32 gobbie = -5; gobbie <= 90; ++gobbie)
        {
            const bool got     = CanExpand(sack, gobbie);
            const bool inlineF = inlineCanExpand(sack, gobbie);
            const bool pin     = pinCanExpand3363(sack, gobbie);
            const bool want    = sack < gobbie && sack > 0;

            ok = expect(got == want, "dense CanExpand formula") && ok;
            ok = expect(got == inlineF, "dense free==inline") && ok;
            ok = expect(got == pin, "dense free==pin") && ok;
        }
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanExpand(30, 50), "sack under must apply") && ok;
    ok = expect(!CanExpand(50, 50), "equal sizes must reject") && ok;
    ok = expect(!CanExpand(0, 50), "empty sack must reject") && ok;
    ok = expect(!CanExpand(60, 50), "sack over must reject") && ok;

    // Production moogleOnUpdate path semantics:
    // free → may changeContainerSize; fail → gobbieCanUpgrade / skip expand.
    ok = expect(CanExpand(30, 50), "moogleOnUpdate free → expand path") && ok;
    ok = expect(!CanExpand(50, 50), "moogleOnUpdate equal → skip expand") && ok;
    ok = expect(!CanExpand(0, 50), "moogleOnUpdate empty sack → skip expand") && ok;
    ok = expect(!CanExpand(60, 50), "moogleOnUpdate sack ahead → skip expand") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    const struct
    {
        int32       sackSize;
        int32       gobbieSize;
        const char* label;
    } hostPoles[] = {
        { 30, 50, "eligible expand path" },
        { 50, 50, "equal skip" },
        { 0, 50, "empty sack skip" },
        { 60, 50, "sack ahead skip" },
        { 1, 2, "min owned expand" },
        { 79, 80, "one behind cap expand" },
        { 80, 80, "equal at cap skip" },
        { 0, 0, "both empty skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = CanExpand(p.sackSize, p.gobbieSize);
        const bool inlineF = inlineCanExpand(p.sackSize, p.gobbieSize);
        const bool pin     = pinCanExpand3363(p.sackSize, p.gobbieSize);
        ok                 = expect(got == pin, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    return ok;
}
