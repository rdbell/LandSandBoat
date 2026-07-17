#include "test_artisan_can_expand_2890.h"

#include "map/artisan_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "artisan CanExpand 2890 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline artisan.lua option 2 pure gate for dual-wire cross-check:
//   sackSize < gobbieSize and sackSize > 0
auto inlineCanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

} // namespace

// Pure dual-wire expansion for artisanhelpers::CanExpand
// (artisan.lua moogleOnUpdate option 2 expand-sack gate).
auto runArtisanCanExpand2890SelfTests() -> bool
{
    using artisanhelpers::CanExpand;

    bool ok = true;

    const struct
    {
        int32       sackSize;
        int32       gobbieSize;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 0948 pins.
        { 30, 50, true, "residual sack 30 < gobbie 50" },
        { 50, 50, false, "residual equal sizes" },
        { 0, 50, false, "residual empty sack" },
        { 60, 50, false, "residual sack > gobbie" },
        // Boundary matrix.
        { 1, 2, true, "min owned sack, one slot behind" },
        { 1, 1, false, "min owned equal" },
        { 0, 0, false, "both empty" },
        { 0, 1, false, "empty sack, inventory 1" },
        { 0, 80, false, "empty sack, full gobbie" },
        { 29, 30, true, "one behind buy size" },
        { 30, 30, false, "equal at buy size" },
        { 30, 35, true, "buy size vs first upgrade" },
        { 35, 30, false, "sack ahead of gobbie" },
        { 79, 80, true, "one behind cap" },
        { 80, 80, false, "equal at gobbie cap" },
        { 80, 79, false, "sack at cap, gobbie below" },
        { -1, 50, false, "negative sack not > 0" },
        { 10, -1, false, "negative gobbie blocks less-than" },
        { -5, -1, false, "both negative; sack not > 0" },
        { 1, 0, false, "sack 1 > gobbie 0" },
        { 50, 80, true, "mid sack, max gobbie" },
        { 1, 100, true, "small sack, oversized gobbie" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanExpand(c.sackSize, c.gobbieSize);
        const bool inlineF = inlineCanExpand(c.sackSize, c.gobbieSize);
        const bool wantF   = c.sackSize < c.gobbieSize && c.sackSize > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline sack<gobbie && sack>0") && ok;
        ok = expect(got == wantF, "dual-wire free == formula") && ok;
    }

    // Residual 0948 explicit pins.
    ok = expect(CanExpand(30, 50), "residual CanExpand(30, 50)") && ok;
    ok = expect(!CanExpand(50, 50), "residual CanExpand(50, 50)") && ok;
    ok = expect(!CanExpand(0, 50), "residual CanExpand(0, 50)") && ok;
    ok = expect(!CanExpand(60, 50), "residual CanExpand(60, 50)") && ok;

    // Dense size grid: free function matches formula everywhere.
    for (int32 sack = -5; sack <= 90; ++sack)
    {
        for (int32 gobbie = -5; gobbie <= 90; ++gobbie)
        {
            const bool got  = CanExpand(sack, gobbie);
            const bool want = sack < gobbie && sack > 0;
            ok = expect(got == want, "dense CanExpand formula") && ok;
            ok = expect(got == inlineCanExpand(sack, gobbie), "dense dual-wire == inline") && ok;
        }
    }

    return ok;
}
