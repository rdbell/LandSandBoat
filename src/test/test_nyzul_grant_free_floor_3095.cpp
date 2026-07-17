#include "test_nyzul_grant_free_floor_3095.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul grant free floor 3095 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua pickSetPoint free-floor gate for dual-wire checks:
// math.random(1, 30) == 1 and freeFloor == 0
auto inlineShouldGrantFreeFloor(const int32 roll1to30, const int32 freeFloorVar) -> bool
{
    return freeFloorVar == 0 && roll1to30 == 1;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldGrantFreeFloor
// (Lua pickSetPoint free-floor selection gate; slice 3095).
// Required poles:
//   - freeFloorVar == 0 && roll == FreeFloorRollHit → true
//   - freeFloorVar != 0 → false (even on hit)
//   - roll != FreeFloorRollHit → false (even with freeFloor unset)
//   - free == inline
auto runNyzulGrantFreeFloor3095SelfTests() -> bool
{
    using nyzulhelpers::FreeFloorRollHit;
    using nyzulhelpers::ShouldGrantFreeFloor;

    bool ok = true;

    ok = expect(FreeFloorRollHit == 1, "FreeFloorRollHit == 1") && ok;

    // Residual 1088 / prior dual-wire 2874 pins still hold under 3095.
    ok = expect(ShouldGrantFreeFloor(1, 0), "hit + freeFloor 0 should grant") && ok;
    ok = expect(!ShouldGrantFreeFloor(1, 1), "hit + freeFloor set should not grant") && ok;
    ok = expect(!ShouldGrantFreeFloor(2, 0), "miss roll should not grant") && ok;

    // freeFloorVar == 0 && roll == FreeFloorRollHit → true.
    ok = expect(ShouldGrantFreeFloor(FreeFloorRollHit, 0), "hit pin + freeFloor unset grants") && ok;
    ok = expect(inlineShouldGrantFreeFloor(FreeFloorRollHit, 0), "inline hit + freeFloor unset grants") && ok;

    // freeFloorVar != 0 → false even on hit.
    const int32 nonZeroFreeFloors[] = { 1, 2, -1, 100 };
    for (const int32 freeFloorVar : nonZeroFreeFloors)
    {
        ok = expect(!ShouldGrantFreeFloor(FreeFloorRollHit, freeFloorVar),
                    "hit + freeFloor non-zero → false") &&
             ok;
        ok = expect(!inlineShouldGrantFreeFloor(FreeFloorRollHit, freeFloorVar),
                    "inline hit + freeFloor non-zero → false") &&
             ok;
    }

    // roll != FreeFloorRollHit → false even with freeFloor unset.
    const int32 missRolls[] = { 0, 2, 15, 30, -1, 100 };
    for (const int32 roll : missRolls)
    {
        if (roll == FreeFloorRollHit)
        {
            continue;
        }
        ok = expect(!ShouldGrantFreeFloor(roll, 0), "miss roll freeFloor unset → false") && ok;
    }

    // --- ShouldGrantFreeFloor table ---
    const struct
    {
        int32       roll;
        int32       freeFloorVar;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { 1, 0, true, "hit + freeFloor unset → true" },
        { FreeFloorRollHit, 0, true, "FreeFloorRollHit pin + freeFloor 0 → true" },
        { 1, 1, false, "hit + freeFloor set → false" },
        { 1, 2, false, "hit + freeFloor any non-zero → false" },
        { 1, -1, false, "hit + freeFloor defensive negative → false" },
        { 1, 100, false, "hit + freeFloor large non-zero → false" },
        { 2, 0, false, "miss roll freeFloor unset → false" },
        { 30, 0, false, "max roll freeFloor unset → false" },
        { 15, 0, false, "mid miss freeFloor unset → false" },
        { 0, 0, false, "roll 0 freeFloor unset → false" },
        { -1, 0, false, "negative roll freeFloor unset → false" },
        { 30, 1, false, "miss and freeFloor set → false" },

        // Residual 1088 / 2874 re-pins.
        { 1, 0, true, "residual hit grants" },
        { 1, 1, false, "residual freeFloor set blocks" },
        { 2, 0, false, "residual miss blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldGrantFreeFloor(c.roll, c.freeFloorVar);
        const bool inlineF = inlineShouldGrantFreeFloor(c.roll, c.freeFloorVar);
        const bool compose = c.freeFloorVar == 0 && c.roll == FreeFloorRollHit;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == compose, "dual-wire free == freeFloorVar==0 && roll==hit") && ok;
    }

    // Host compose: dense free == inline == freeFloorVar==0 && roll==hit.
    for (int32 roll = 0; roll <= 30; ++roll)
    {
        for (int32 freeFloorVar = 0; freeFloorVar <= 2; ++freeFloorVar)
        {
            const bool got     = ShouldGrantFreeFloor(roll, freeFloorVar);
            const bool inlineF = inlineShouldGrantFreeFloor(roll, freeFloorVar);
            const bool want    = freeFloorVar == 0 && roll == FreeFloorRollHit;

            ok = expect(got == want, "compose free == freeFloorVar==0 && roll==hit") && ok;
            ok = expect(got == inlineF, "compose dual-wire free == inline") && ok;
        }
    }

    return ok;
}
