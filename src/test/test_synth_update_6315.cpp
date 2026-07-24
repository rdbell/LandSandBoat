#include "test_synth_update_6315.h"

#include "map/ai/states/synth_update.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synth Update 6315 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CSynthState::SynthReady:
//   remainingNegative && isAlive
auto inlineIsReady(const bool remainingNegative, const bool isAlive) -> bool
{
    return remainingNegative && isAlive;
}

// Compact dual-wire pin matching Go SynthReady.
auto pinIsReady(const bool remainingNegative, const bool isAlive) -> bool
{
    return remainingNegative && isAlive;
}

// Inline shouldCriticalFailExit: isDead
auto inlineCriticalFail(const bool isDead) -> bool
{
    return isDead;
}

} // namespace

// Pure dual-wire suite for CSynthState::Update / SynthReady
// (OmegaXI internal/aistate; slice 6315).
auto runSynthUpdate6315SelfTests() -> bool
{
    using synthupdate::isReady;
    using synthupdate::shouldCriticalFailExit;
    using synthupdate::shouldFinishExit;

    bool ok = true;

    // --- SynthReady / isReady: free == inline == pin ---
    ok = expect(isReady(true, true), "negative remaining + alive → ready") && ok;
    ok = expect(!isReady(true, false), "negative remaining + dead → not ready") && ok;
    ok = expect(!isReady(false, true), "non-negative remaining + alive → not ready") && ok;
    ok = expect(!isReady(false, false), "non-negative + dead → not ready") && ok;

    for (const bool remNeg : { false, true })
    {
        for (const bool alive : { false, true })
        {
            const bool got     = isReady(remNeg, alive);
            const bool inlineF = inlineIsReady(remNeg, alive);
            const bool pinGot  = pinIsReady(remNeg, alive);
            const bool want    = remNeg && alive;
            ok                 = expect(got == want, "isReady free==want") && ok;
            ok                 = expect(got == inlineF, "isReady free==inline") && ok;
            ok                 = expect(got == pinGot, "isReady free==pin") && ok;
            ok                 = expect(shouldFinishExit(remNeg, alive) == got, "shouldFinishExit==isReady") && ok;
        }
    }

    // --- shouldCriticalFailExit identity ---
    ok = expect(shouldCriticalFailExit(true), "dead → critical fail exit") && ok;
    ok = expect(!shouldCriticalFailExit(false), "alive → no critical fail exit") && ok;
    ok = expect(shouldCriticalFailExit(true) == inlineCriticalFail(true), "critical free==inline") && ok;
    ok = expect(shouldCriticalFailExit(false) == inlineCriticalFail(false), "critical free==inline false") && ok;

    // Host-style Update composition poles:
    // dead first → critical fail; else if ready → finish; else countdown.
    ok = expect(shouldCriticalFailExit(true), "Update dead path admits critical fail") && ok;
    ok = expect(!shouldCriticalFailExit(false) && shouldFinishExit(true, true),
                "Update alive ready → finish exit") &&
         ok;
    ok = expect(!shouldCriticalFailExit(false) && !shouldFinishExit(false, true),
                "Update alive not-ready → countdown") &&
         ok;
    ok = expect(!shouldCriticalFailExit(false) && !shouldFinishExit(true, false),
                "Update not-dead but not-alive ready inject → countdown") &&
         ok;

    return ok;
}
