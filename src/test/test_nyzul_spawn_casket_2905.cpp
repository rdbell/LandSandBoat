#include "test_nyzul_spawn_casket_2905.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul spawn casket 2905 self-test failed: " << label << '\n';
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
// (Lua spawnChest regular-mob casket roll).
auto runNyzulSpawnCasket2905SelfTests() -> bool
{
    using nyzulhelpers::CasketDropChancePercent;
    using nyzulhelpers::ShouldSpawnCasket;

    bool ok = true;

    ok = expect(CasketDropChancePercent == 6, "CasketDropChancePercent == 6") && ok;

    const struct
    {
        int32       roll;
        bool        enable;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, true, true, "min success roll" },
        { 6, true, true, "threshold success roll" },
        { 3, true, true, "mid success roll" },
        { 7, true, false, "just above threshold" },
        { 100, true, false, "max miss roll" },
        { 50, true, false, "mid miss roll" },
        { 0, true, false, "roll 0 out of range" },
        { -1, true, false, "negative roll" },
        { 2, true, true, "success roll 2" },
        { 5, true, true, "success roll 5" },
        { 101, true, false, "above max range" },
        { 1, false, false, "min roll caskets disabled" },
        { 6, false, false, "threshold roll caskets disabled" },
        { 7, false, false, "miss roll caskets disabled" },
        { 0, false, false, "out of range and disabled" },
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

    // Residual 1088 pins.
    ok = expect(ShouldSpawnCasket(6, true), "residual threshold success") && ok;
    ok = expect(ShouldSpawnCasket(1, true), "residual min success") && ok;
    ok = expect(!ShouldSpawnCasket(7, true), "residual above-threshold miss") && ok;
    ok = expect(!ShouldSpawnCasket(1, false), "residual caskets disabled") && ok;

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
