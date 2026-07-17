#include "test_abyssea_buff_power_2866.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea BuffPower 2866 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua visionsCruorProspectorOnEventFinish power formula for dual-wire
// cross-check:
//   v[3] + getAbyssiteTotal(player, v[4]) * v[5]
auto inlineBuffPower(const int32 base, const int32 abyssiteTotal, const int32 mult) -> int32
{
    return base + abyssiteTotal * mult;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::BuffPower
// (Lua visionsCruorProspectorOnEventFinish ENHANCEMENT power formula).
auto runAbysseaBuffPower2866SelfTests() -> bool
{
    using abysseahelpers::BuffPower;

    bool ok = true;

    const struct
    {
        int32       base;
        int32       total;
        int32       mult;
        int32       want;
        const char* label;
    } cases[] = {
        // HP Merit×10
        { 20, 0, 10, 20, "HP no merit" },
        { 20, 1, 10, 30, "HP 1 merit" },
        { 20, 6, 10, 80, "HP full merit span (6)" },
        // MP Merit×5
        { 10, 0, 5, 10, "MP no merit" },
        { 10, 3, 5, 25, "MP 3 merit" },
        { 10, 6, 5, 40, "MP full merit" },
        // Stat Furtherance×10
        { 10, 0, 10, 10, "stat no furtherance" },
        { 10, 1, 10, 20, "stat 1 furtherance" },
        { 10, 3, 10, 40, "stat full furtherance (3)" },
        // Degenerate / edge
        { 0, 5, 10, 50, "zero base" },
        { 10, 0, 0, 10, "zero mult" },
        { 0, 0, 0, 0, "all zero" },
        { 1, 1, 1, 2, "unit identity" },
        { 100, 10, 10, 200, "large base+total" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = BuffPower(c.base, c.total, c.mult);
        const int32 inlineF = inlineBuffPower(c.base, c.total, c.mult);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Catalog compose dual-wire pins (sel-11 sample totals Merit=2 / Furtherance=1):
    //   HP 20+2*10=40, MP 10+2*5=20, stats 10+1*10=20
    ok = expect(BuffPower(20, 2, 10) == 40, "sel11 HP merit=2") && ok;
    ok = expect(BuffPower(10, 2, 5) == 20, "sel11 MP merit=2") && ok;
    ok = expect(BuffPower(10, 1, 10) == 20, "sel11 stat furtherance=1") && ok;
    ok = expect(BuffPower(20, 2, 10) == inlineBuffPower(20, 2, 10), "sel11 HP dual-wire") && ok;
    ok = expect(BuffPower(10, 2, 5) == inlineBuffPower(10, 2, 5), "sel11 MP dual-wire") && ok;
    ok = expect(BuffPower(10, 1, 10) == inlineBuffPower(10, 1, 10), "sel11 stat dual-wire") && ok;

    return ok;
}
