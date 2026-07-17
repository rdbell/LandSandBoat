#include "test_einherjar_cycle_wave_2864.h"

#include "map/einherjar_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "einherjar cycle wave 2864 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onMobDespawn cycleWave gate for dual-wire checks:
// #chamberData.mobs <= 0
auto inlineShouldCycleWave(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

} // namespace

// Pure dual-wire expansion for einherjarhelpers::ShouldCycleWave
// (Lua onMobDespawn cycleWave gate after active-list remove).
auto runEinherjarCycleWave2864SelfTests() -> bool
{
    using einherjarhelpers::ShouldCycleWave;

    bool ok = true;

    const struct
    {
        int32       remaining;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "empty live list after last despawn" },
        { -1, true, "defensive negative remaining" },
        { -100, true, "large negative remaining" },
        { 1, false, "one live mob remaining" },
        { 2, false, "two live mobs remaining" },
        { 5, false, "mid-wave remaining" },
        { 36, false, "max-party-scale remaining" },
        { 100, false, "large remaining" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCycleWave(c.remaining);
        const bool inlineF = inlineShouldCycleWave(c.remaining);
        const bool pure    = c.remaining <= 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == remaining <= 0") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Residual 1069 pins.
    ok = expect(ShouldCycleWave(0), "residual empty cycles") && ok;
    ok = expect(ShouldCycleWave(-1), "residual negative cycles") && ok;
    ok = expect(!ShouldCycleWave(1), "residual remaining does not cycle") && ok;

    // Dense compose range identity.
    for (int32 remaining = -5; remaining <= 20; ++remaining)
    {
        const bool got  = ShouldCycleWave(remaining);
        const bool want = remaining <= 0;
        ok = expect(got == want, "compose range free == remaining<=0") && ok;
        ok = expect(got == inlineShouldCycleWave(remaining), "compose range free == inline") && ok;
    }

    return ok;
}
