#include "test_nyzul_floor100_vigil_2900.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul floor100 vigil 2900 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua vigilWeaponDrop floor-100 gate for dual-wire checks:
// instance:getLocalVar('Nyzul_Current_Floor') == 100
auto inlineShouldDropFloor100VigilWeapons(const int32 currentFloor) -> bool
{
    return currentFloor == 100;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldDropFloor100VigilWeapons
// (Lua vigilWeaponDrop floor-100 guaranteed-drop gate).
auto runNyzulFloor100Vigil2900SelfTests() -> bool
{
    using nyzulhelpers::Floor100;
    using nyzulhelpers::ShouldDropFloor100VigilWeapons;

    bool ok = true;

    ok = expect(Floor100 == 100, "Floor100 == 100") && ok;

    const struct
    {
        int32       floor;
        bool        want;
        const char* label;
    } cases[] = {
        { 100, true, "floor 100 grants" },
        { 99, false, "floor 99 misses" },
        { 101, false, "floor 101 misses" },
        { 1, false, "floor 1 misses" },
        { 0, false, "floor 0 misses" },
        { -1, false, "negative floor misses" },
        { 50, false, "mid floor misses" },
        { 20, false, "boss floor 20 misses" },
        { 80, false, "boss floor 80 misses" },
        { 200, false, "above max misses" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDropFloor100VigilWeapons(c.floor);
        const bool inlineF = inlineShouldDropFloor100VigilWeapons(c.floor);
        const bool pure    = c.floor == Floor100;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == floor==Floor100") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1088 pins.
    ok = expect(ShouldDropFloor100VigilWeapons(100), "residual floor 100 grants") && ok;
    ok = expect(!ShouldDropFloor100VigilWeapons(99), "residual floor 99 misses") && ok;

    // Dense compose range identity: floors -2..105.
    for (int32 floor = -2; floor <= 105; ++floor)
    {
        const bool got  = ShouldDropFloor100VigilWeapons(floor);
        const bool want = floor == Floor100;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineShouldDropFloor100VigilWeapons(floor),
                    "compose range free == inline") &&
             ok;
    }

    return ok;
}
