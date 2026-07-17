#include "test_einherjar_should_cycle_wave_3517.h"

#include "map/einherjar_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "einherjar ShouldCycleWave 3517 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onMobDespawn cycleWave gate for dual-wire cross-check
// (dedicated slice 3517):
//   #chamberData.mobs <= 0
auto inlineShouldCycleWave(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// Compact dual-wire pin matching Go pinShouldCycleWave3517 / C++ capacity.
// Direct return only — same form as production; no De Morgan rewrite:
//   remainingMobs <= 0
auto pinShouldCycleWave(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// Prior dedicated 3329 pin (independence cross-check):
//   remainingMobs <= 0
auto pinShouldCycleWave3329(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// Prior dedicated 3329 inline (independence cross-check):
//   #chamberData.mobs <= 0
auto inlineShouldCycleWave3329(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// Prior dedicated 3181 pin (independence cross-check):
//   remainingMobs <= 0
auto pinShouldCycleWave3181(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

// Prior dedicated 3181 inline (independence cross-check):
//   #chamberData.mobs <= 0
auto inlineShouldCycleWave3181(const int32 remainingMobs) -> bool
{
    return remainingMobs <= 0;
}

} // namespace

// Pure dual-wire expansion for einherjarhelpers::ShouldCycleWave
// (Lua onMobDespawn cycleWave gate after active-list remove; dedicated slice
// 3517 expand residual 2864 / pure 1069; prior dedicated 3181 / 3329 retained).
//
// Coverage:
//   - free == inline == pin == (remainingMobs <= 0)
//   - residual 1069 / 2864 pins still hold
//   - prior dedicated 3181 / 3329 poles still hold
//   - poles: remainingMobs -1 / 0 / 1
//   - dense cases matching 3329 / 3181 style
//   - prior 3329 independence: free == prior inline == prior pin
auto runEinherjarShouldCycleWave3517SelfTests() -> bool
{
    using einherjarhelpers::ShouldCycleWave;

    bool ok = true;

    // Residual 1069 / 2864 / prior 3181 / 3329 pins still hold under dual-wire.
    ok = expect(ShouldCycleWave(0), "residual empty cycles") && ok;
    ok = expect(ShouldCycleWave(-1), "residual negative cycles") && ok;
    ok = expect(!ShouldCycleWave(1), "residual remaining does not cycle") && ok;

    // --- Composition table: free == inline == pin ---
    // Required poles: remainingMobs -1 / 0 / 1.
    const struct
    {
        int32       remaining;
        bool        want;
        const char* label;
    } cases[] = {
        // Required poles: -1 / 0 / 1.
        { 0, true, "pole remaining=0 cycles" },
        { -1, true, "pole remaining=-1 cycles" },
        { 1, false, "pole remaining=1 does not cycle" },

        // Residual 2864 / prior 3181 / 3329 happy / multi remaining.
        { -100, true, "large negative remaining cycles" },
        { 2, false, "two live mobs remaining" },
        { 5, false, "mid-wave remaining" },
        { 36, false, "max-party-scale remaining" },
        { 100, false, "large remaining" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCycleWave(c.remaining);
        const bool inlineF = inlineShouldCycleWave(c.remaining);
        const bool pin     = pinShouldCycleWave(c.remaining);
        const bool wantPin = c.remaining <= 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldCycleWave dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "ShouldCycleWave == pin formula") && ok;
    }

    // Pin composition at exact remainingMobs<=0 boundary poles.
    ok = expect(ShouldCycleWave(0), "remaining==0 must cycle") && ok;
    ok = expect(ShouldCycleWave(-1), "remaining==-1 must cycle") && ok;
    ok = expect(!ShouldCycleWave(1), "remaining==1 must not cycle") && ok;

    // Dense compose: remaining poles free == inline == pin (3329 / 3181 style).
    for (int32 remaining = -5; remaining <= 20; ++remaining)
    {
        const bool got     = ShouldCycleWave(remaining);
        const bool inlineF = inlineShouldCycleWave(remaining);
        const bool pin     = pinShouldCycleWave(remaining);
        const bool want    = remaining <= 0;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // --- Production onMobDespawn path semantics ---
    // Host injects remainingMobs = #chamberData.mobs after remove;
    // if ShouldCycleWave → xi.einherjar.cycleWave(chamberData)
    ok = expect(ShouldCycleWave(0), "host inject last despawn cycles") && ok;
    ok = expect(ShouldCycleWave(-1), "host inject negative remaining cycles") && ok;
    ok = expect(!ShouldCycleWave(1), "host inject one live does not cycle") && ok;
    ok = expect(!ShouldCycleWave(2), "host inject multi live does not cycle") && ok;
    ok = expect(!ShouldCycleWave(36), "host inject max-party-scale does not cycle") && ok;
    ok = expect(ShouldCycleWave(-100), "host inject large negative cycles") && ok;

    // Explicit dual-wire: free == inline == pin for host poles.
    const int32 hostPoles[] = { -100, -1, 0, 1, 2, 36 };
    for (const int32 remaining : hostPoles)
    {
        const bool got     = ShouldCycleWave(remaining);
        const bool inlineF = inlineShouldCycleWave(remaining);
        const bool pin     = pinShouldCycleWave(remaining);
        const bool want    = remaining <= 0;
        ok                 = expect(got == want, "host inject free == pin") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Prior dedicated 3329 / 3181 independence: free == prior inline == prior pin.
    for (int32 remaining = -5; remaining <= 20; ++remaining)
    {
        const bool got            = ShouldCycleWave(remaining);
        const bool prior3329Inline = inlineShouldCycleWave3329(remaining);
        const bool prior3329Pin    = pinShouldCycleWave3329(remaining);
        const bool prior3181Inline = inlineShouldCycleWave3181(remaining);
        const bool prior3181Pin    = pinShouldCycleWave3181(remaining);
        ok = expect(got == prior3329Inline && got == prior3329Pin, "prior 3329 independence") && ok;
        ok = expect(got == prior3181Inline && got == prior3181Pin, "prior 3181 independence") && ok;
    }

    return ok;
}
