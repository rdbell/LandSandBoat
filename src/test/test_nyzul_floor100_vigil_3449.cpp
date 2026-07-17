#include "test_nyzul_floor100_vigil_3449.h"

#include "map/nyzul_capacity.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul floor100 vigil 3449 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua vigilWeaponDrop floor-100 gate for dual-wire cross-check
// (slice 3449 dedicated expand residual 2900; prior dedicated 3352):
//   instance:getLocalVar('Nyzul_Current_Floor') == 100
// Direct return form matching production free function / capacity.
auto inlineShouldDropFloor100VigilWeapons(const int32 currentFloor) -> bool
{
    return currentFloor == 100;
}

// Compact dual-wire pin matching free function / capacity body (slice 3449).
// Direct return only — same formula as production ShouldDropFloor100VigilWeapons.
auto pinShouldDropFloor100VigilWeapons3449(const int32 currentFloor) -> bool
{
    return currentFloor == nyzulhelpers::Floor100;
}

// Prior dedicated 3352 pin (retained cross-check under 3449).
auto pinShouldDropFloor100VigilWeapons3352(const int32 currentFloor) -> bool
{
    return currentFloor == nyzulhelpers::Floor100;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldDropFloor100VigilWeapons
// (Lua vigilWeaponDrop floor-100 guaranteed-drop gate; slice 3449 dedicated
// expand residual 2900; prior dedicated 3352). Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 2900 / 1088 poles still hold (100 grants; non-100 misses)
//   - prior dedicated 3352 pins still hold
//   - residual poles + dense edges around Floor100 + dense compose
auto runNyzulFloor100Vigil3449SelfTests() -> bool
{
    using nyzulhelpers::Floor100;
    using nyzulhelpers::ShouldDropFloor100VigilWeapons;

    bool ok = true;

    ok = expect(Floor100 == 100, "Floor100 == 100") && ok;

    // Residual 2900 / 1088 / prior dedicated 3352 pins still hold.
    ok = expect(ShouldDropFloor100VigilWeapons(100), "residual floor 100 grants") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(99), "residual floor 99 misses") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(101), "residual floor 101 misses") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(1), "residual floor 1 misses") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(0), "residual floor 0 misses") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(-1), "residual negative floor misses") && ok;

    const struct
    {
        int32       floor;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2900 / 1088 classic dual poles.
        { 100, true, "residual floor 100 grants" },
        { 99, false, "residual floor 99 misses" },
        { 101, false, "residual floor 101 misses" },
        { 1, false, "residual floor 1 misses" },
        { 0, false, "residual floor 0 misses" },
        { -1, false, "residual negative floor misses" },
        { 50, false, "residual mid floor misses" },
        { 20, false, "residual boss floor 20 misses" },
        { 80, false, "residual boss floor 80 misses" },
        { 200, false, "residual above max misses" },

        // Host inject path poles (vigilWeaponDrop Nyzul_Current_Floor).
        { 100, true, "host floor 100 guaranteed path" },
        { 99, false, "host floor 99 NM-roll path" },
        { 101, false, "host floor 101 miss" },
        { Floor100, true, "host Floor100 constant grants" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldDropFloor100VigilWeapons(p.floor);
        const bool inlineF = inlineShouldDropFloor100VigilWeapons(p.floor);
        const bool pin     = pinShouldDropFloor100VigilWeapons3449(p.floor);
        const bool prior   = pinShouldDropFloor100VigilWeapons3352(p.floor);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "ShouldDropFloor100VigilWeapons dual-wire == inline Lua formula") && ok;
        ok = expect(got == pin, "ShouldDropFloor100VigilWeapons == pin formula") && ok;
        ok = expect(got == prior, "ShouldDropFloor100VigilWeapons == prior 3352 pin") && ok;
        ok = expect(got == (p.floor == Floor100), "ShouldDropFloor100VigilWeapons == formula") && ok;
    }

    // Dense edges around Floor100: free == inline == pin == formula.
    const struct
    {
        int32       floor;
        const char* label;
    } edges[] = {
        { -2, "edge neg -2" },
        { -1, "edge neg -1" },
        { 0, "edge zero" },
        { 1, "edge start floor" },
        { 20, "edge boss 20" },
        { 80, "edge boss 80" },
        { 99, "edge just below Floor100" },
        { 100, "edge Floor100 grants" },
        { 101, "edge just above Floor100" },
        { 200, "edge above max" },
        { 0x7FFFFFFF, "edge max int" },
        { std::numeric_limits<int32>::min(), "edge min int" },
    };

    for (const auto& e : edges)
    {
        const bool got     = ShouldDropFloor100VigilWeapons(e.floor);
        const bool inlineF = inlineShouldDropFloor100VigilWeapons(e.floor);
        const bool pin     = pinShouldDropFloor100VigilWeapons3449(e.floor);
        const bool want    = e.floor == Floor100;

        ok = expect(got == want, e.label) && ok;
        ok = expect(got == inlineF, "dense edge free == inline") && ok;
        ok = expect(got == pin, "dense edge free == pin") && ok;
    }

    // Dense compose range identity: floors -2..Floor100+5.
    // free == inline == pin == formula.
    for (int32 floor = -2; floor <= Floor100 + 5; ++floor)
    {
        const bool got     = ShouldDropFloor100VigilWeapons(floor);
        const bool inlineF = inlineShouldDropFloor100VigilWeapons(floor);
        const bool pin     = pinShouldDropFloor100VigilWeapons3449(floor);
        const bool want    = floor == Floor100;

        ok = expect(got == want, "dense compose free == formula") && ok;
        ok = expect(got == inlineF, "dense compose free == inline") && ok;
        ok = expect(got == pin, "dense compose free == pin") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(ShouldDropFloor100VigilWeapons(Floor100), "Floor100 must grant") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(Floor100 - 1), "Floor100-1 must miss") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(Floor100 + 1), "Floor100+1 must miss") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(0), "floor 0 must miss") && ok;
    ok = expect(ShouldDropFloor100VigilWeapons(100) == pinShouldDropFloor100VigilWeapons3449(100),
                "free == pin 100") &&
         ok;
    ok = expect(ShouldDropFloor100VigilWeapons(99) == pinShouldDropFloor100VigilWeapons3449(99),
                "free == pin 99") &&
         ok;
    ok = expect(ShouldDropFloor100VigilWeapons(100) == inlineShouldDropFloor100VigilWeapons(100),
                "free == inline 100") &&
         ok;
    ok = expect(ShouldDropFloor100VigilWeapons(99) == inlineShouldDropFloor100VigilWeapons(99),
                "free == inline 99") &&
         ok;

    // Production path semantics (host inject model for vigilWeaponDrop).
    const struct
    {
        int32       floor;
        const char* label;
    } hostPoles[] = {
        { 100, "eligible floor-100 guaranteed path" },
        { 99, "floor 99 NM-roll path" },
        { 1, "start floor miss" },
        { -1, "defensive negative miss" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldDropFloor100VigilWeapons(p.floor);
        const bool inlineF = inlineShouldDropFloor100VigilWeapons(p.floor);
        const bool pin     = pinShouldDropFloor100VigilWeapons3449(p.floor);

        ok = expect(got == pin, p.label) && ok;
        ok = expect(got == inlineF, "host pole free == inline") && ok;
    }

    // Residual 2900 / prior dedicated 3352 still hold under dedicated 3449.
    ok = expect(ShouldDropFloor100VigilWeapons(100) && !ShouldDropFloor100VigilWeapons(99),
                "residual 2900 / prior 3352 pins under 3449") &&
         ok;

    return ok;
}
