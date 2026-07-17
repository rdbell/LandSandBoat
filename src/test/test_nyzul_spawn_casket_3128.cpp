#include "test_nyzul_spawn_casket_3128.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul spawn casket 3128 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua spawnChest casket roll gate for dual-wire checks:
// ENABLE_NYZUL_CASKETS and math.random(1, 100) <= 6
//   → enableCaskets && roll >= 1 && roll <= 6
auto inlineShouldSpawnCasket(const int32 roll1to100, const bool enableCaskets) -> bool
{
    return enableCaskets && roll1to100 >= 1 && roll1to100 <= 6;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldSpawnCasket
// (Lua spawnChest regular-mob casket roll; slice 3128).
// Required poles:
//   - enableCaskets off → false
//   - roll boundaries (1..CasketDropChancePercent success; out-of-band miss)
//   - free == inline
auto runNyzulSpawnCasket3128SelfTests() -> bool
{
    using nyzulhelpers::CasketDropChancePercent;
    using nyzulhelpers::ShouldSpawnCasket;

    bool ok = true;

    ok = expect(CasketDropChancePercent == 6, "CasketDropChancePercent == 6") && ok;

    // Residual 1088 / prior dual-wire 2905 pins still hold under 3128.
    ok = expect(ShouldSpawnCasket(6, true), "residual threshold success") && ok;
    ok = expect(ShouldSpawnCasket(1, true), "residual min success") && ok;
    ok = expect(!ShouldSpawnCasket(7, true), "residual above-threshold miss") && ok;
    ok = expect(!ShouldSpawnCasket(1, false), "residual caskets disabled") && ok;

    // enableCaskets off → false (all rolls).
    for (const int32 roll : { -1, 0, 1, 6, 7, 100 })
    {
        ok = expect(!ShouldSpawnCasket(roll, false), "enable off → false") && ok;
        ok = expect(!inlineShouldSpawnCasket(roll, false), "inline enable off → false") && ok;
    }

    // Roll boundaries with enable on — success band 1..CasketDropChancePercent.
    for (int32 roll = 1; roll <= CasketDropChancePercent; ++roll)
    {
        ok = expect(ShouldSpawnCasket(roll, true), "success band → true") && ok;
        ok = expect(inlineShouldSpawnCasket(roll, true), "inline success band → true") && ok;
    }

    // Miss / out-of-range poles.
    for (const int32 roll : { -1, 0, CasketDropChancePercent + 1, 7, 50, 100, 101 })
    {
        ok = expect(!ShouldSpawnCasket(roll, true), "miss/out-of-range → false") && ok;
        ok = expect(!inlineShouldSpawnCasket(roll, true), "inline miss/out-of-range → false") && ok;
    }

    // --- ShouldSpawnCasket table ---
    const struct
    {
        int32       roll;
        bool        enable;
        bool        want;
        const char* label;
    } cases[] = {
        // Required dual-wire poles.
        { 1, true, true, "min success roll → true" },
        { 6, true, true, "threshold success roll → true" },
        { 3, true, true, "mid success roll → true" },
        { CasketDropChancePercent, true, true, "CasketDropChancePercent ceiling → true" },
        { 7, true, false, "just above threshold → false" },
        { 100, true, false, "max miss roll → false" },
        { 50, true, false, "mid miss roll → false" },
        { 0, true, false, "roll 0 out of range → false" },
        { -1, true, false, "negative roll → false" },
        { 101, true, false, "above max range → false" },
        { 1, false, false, "min roll caskets disabled → false" },
        { 6, false, false, "threshold roll caskets disabled → false" },
        { 7, false, false, "miss roll caskets disabled → false" },
        { 0, false, false, "out of range and disabled → false" },

        // Residual 1088 / 2905 re-pins.
        { 2, true, true, "residual success roll 2" },
        { 5, true, true, "residual success roll 5" },
        { 6, true, true, "residual threshold succeeds" },
        { 7, true, false, "residual above-threshold misses" },
        { 1, false, false, "residual disabled blocks" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSpawnCasket(c.roll, c.enable);
        const bool inlineF = inlineShouldSpawnCasket(c.roll, c.enable);
        const bool pure    = c.enable && c.roll >= 1 && c.roll <= CasketDropChancePercent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == enable&&roll in 1..percent") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Dense compose range identity: rolls -2..101 × enable true/false.
    for (const bool enable : { true, false })
    {
        for (int32 roll = -2; roll <= 101; ++roll)
        {
            const bool got  = ShouldSpawnCasket(roll, enable);
            const bool want = enable && roll >= 1 && roll <= CasketDropChancePercent;
            ok = expect(got == want, "compose range free == formula") && ok;
            ok = expect(got == inlineShouldSpawnCasket(roll, enable),
                        "compose range free == inline") &&
                 ok;
        }
    }

    return ok;
}
