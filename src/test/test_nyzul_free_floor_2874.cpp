#include "test_nyzul_free_floor_2874.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul free floor 2874 self-test failed: " << label << '\n';
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
// (Lua pickSetPoint free-floor selection gate).
auto runNyzulFreeFloor2874SelfTests() -> bool
{
    using nyzulhelpers::FreeFloorRollHit;
    using nyzulhelpers::ShouldGrantFreeFloor;

    bool ok = true;

    ok = expect(FreeFloorRollHit == 1, "FreeFloorRollHit == 1") && ok;

    const struct
    {
        int32       roll;
        int32       freeFloorVar;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, 0, true, "hit + freeFloor unset grants" },
        { 1, 1, false, "hit but freeFloor already set" },
        { 2, 0, false, "miss roll freeFloor unset" },
        { 30, 0, false, "max roll freeFloor unset" },
        { 15, 0, false, "mid miss freeFloor unset" },
        { 1, 2, false, "hit freeFloor any non-zero" },
        { 1, -1, false, "hit freeFloor defensive negative" },
        { 0, 0, false, "roll 0 freeFloor unset" },
        { -1, 0, false, "negative roll freeFloor unset" },
        { 1, 100, false, "hit freeFloor large non-zero" },
        { 30, 1, false, "miss and freeFloor set" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldGrantFreeFloor(c.roll, c.freeFloorVar);
        const bool inlineF = inlineShouldGrantFreeFloor(c.roll, c.freeFloorVar);
        const bool pure    = c.freeFloorVar == 0 && c.roll == FreeFloorRollHit;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == freeFloorVar==0 && roll==hit") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1088 pins.
    ok = expect(ShouldGrantFreeFloor(1, 0), "residual hit grants") && ok;
    ok = expect(!ShouldGrantFreeFloor(1, 1), "residual hit blocked by freeFloor") && ok;
    ok = expect(!ShouldGrantFreeFloor(2, 0), "residual miss") && ok;

    // Dense compose range identity: rolls 0..30 × freeFloorVar 0..2.
    for (int32 roll = 0; roll <= 30; ++roll)
    {
        for (int32 freeFloorVar = 0; freeFloorVar <= 2; ++freeFloorVar)
        {
            const bool got  = ShouldGrantFreeFloor(roll, freeFloorVar);
            const bool want = freeFloorVar == 0 && roll == FreeFloorRollHit;
            ok = expect(got == want, "compose range free == formula") && ok;
            ok = expect(got == inlineShouldGrantFreeFloor(roll, freeFloorVar),
                        "compose range free == inline") &&
                 ok;
        }
    }

    return ok;
}
