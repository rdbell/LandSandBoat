#include "test_nyzul_gear_objective_2891.h"

#include "map/nyzul_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nyzul gear objective 2891 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua pickSetPoint gear-objective gate for dual-wire checks:
// math.random(1, 30) <= 5  →  roll >= 1 && roll <= 5
auto inlineShouldRollGearObjective(const int32 roll1to30) -> bool
{
    return roll1to30 >= 1 && roll1to30 <= 5;
}

} // namespace

// Pure dual-wire expansion for nyzulhelpers::ShouldRollGearObjective
// (Lua pickSetPoint gear-objective chance gate).
auto runNyzulGearObjective2891SelfTests() -> bool
{
    using nyzulhelpers::GearObjectiveRollThreshold;
    using nyzulhelpers::ShouldRollGearObjective;

    bool ok = true;

    ok = expect(GearObjectiveRollThreshold == 5, "GearObjectiveRollThreshold == 5") && ok;

    const struct
    {
        int32       roll;
        bool        want;
        const char* label;
    } cases[] = {
        { 1, true, "min success roll" },
        { 5, true, "threshold success roll" },
        { 3, true, "mid success roll" },
        { 6, false, "just above threshold" },
        { 30, false, "max miss roll" },
        { 15, false, "mid miss roll" },
        { 0, false, "roll 0 out of range" },
        { -1, false, "negative roll" },
        { 2, true, "success roll 2" },
        { 4, true, "success roll 4" },
        { 31, false, "above max range" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRollGearObjective(c.roll);
        const bool inlineF = inlineShouldRollGearObjective(c.roll);
        const bool pure    = c.roll >= 1 && c.roll <= GearObjectiveRollThreshold;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == roll>=1 && roll<=threshold") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1088 pins.
    ok = expect(ShouldRollGearObjective(5), "residual threshold success") && ok;
    ok = expect(ShouldRollGearObjective(1), "residual min success") && ok;
    ok = expect(!ShouldRollGearObjective(6), "residual above-threshold miss") && ok;

    // Dense compose range identity: rolls -2..31.
    for (int32 roll = -2; roll <= 31; ++roll)
    {
        const bool got  = ShouldRollGearObjective(roll);
        const bool want = roll >= 1 && roll <= GearObjectiveRollThreshold;
        ok = expect(got == want, "compose range free == formula") && ok;
        ok = expect(got == inlineShouldRollGearObjective(roll),
                    "compose range free == inline") &&
             ok;
    }

    return ok;
}
