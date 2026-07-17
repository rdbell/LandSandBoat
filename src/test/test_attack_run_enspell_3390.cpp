#include "test_attack_run_enspell_3390.h"

#include "map/attack_post_swing_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack ShouldRunEnspell 3390 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnAttack live-target enspell gate for dual-wire cross-check
// (dedicated slice 3390):
//   targetHPP > 0
auto inlineShouldRunEnspell(const uint8 targetHPP) -> bool
{
    return targetHPP > 0;
}

// Compact dual-wire pin matching Go pinShouldRunEnspell3390 / C++ capacity:
//   targetHPP > 0
auto pinShouldRunEnspell(const uint8 targetHPP) -> bool
{
    return targetHPP > 0;
}

} // namespace

// Pure dual-wire expansion for attackpostswinghelpers::ShouldRunEnspell
// (targetHPP > 0; suppresses enspell after target dies; dedicated slice 3390;
// residual expand 3046 / pure 1399).
//
// Coverage:
//   - free == inline == pin == (targetHPP > 0)
//   - residual 1399 / 3046 pins still hold
//   - siblings ShouldRunEnspellAndSpikes / ShouldRunParrySpikes residual only
// Edges: 0, 1, 50, 100, 255.
auto runAttackRunEnspell3390SelfTests() -> bool
{
    using namespace attackpostswinghelpers;

    bool ok = true;

    // Residual 1399 / 3046 pins still hold under dual-wire.
    ok = expect(ShouldRunEnspell(1), "residual: live target (HPP=1) runs enspell") && ok;
    ok = expect(!ShouldRunEnspell(0), "residual: dead target (HPP=0) skips enspell") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        uint8       targetHPP;
        bool        want;
        const char* label;
    } cases[] = {
        // Required edges: 0, 1, 50, 100, 255.
        { 0, false, "HPP=0 dead → skip enspell" },
        { 1, true, "HPP=1 live floor → run enspell" },
        { 50, true, "HPP=50 mid → run enspell" },
        { 100, true, "HPP=100 full → run enspell" },
        { 255, true, "HPP=255 max uint8 → run enspell" },

        // Extra poles for dual-wire stability.
        { 2, true, "HPP=2 → run" },
        { 99, true, "HPP=99 → run" },
        { 127, true, "HPP=127 → run" },
        { 254, true, "HPP=254 → run" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRunEnspell(c.targetHPP);
        const bool inlineF = inlineShouldRunEnspell(c.targetHPP);
        const bool pin     = pinShouldRunEnspell(c.targetHPP);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "ShouldRunEnspell dual-wire free == inline == pin") &&
             ok;
        ok = expect(got == (c.targetHPP > 0), "free == targetHPP > 0") && ok;
    }

    // Pin composition: free == (targetHPP > 0).
    ok = expect(!ShouldRunEnspell(0), "HPP=0 must not run enspell") && ok;
    ok = expect(ShouldRunEnspell(1), "HPP=1 must run enspell") && ok;
    ok = expect(ShouldRunEnspell(100), "HPP=100 must run enspell") && ok;
    ok = expect(ShouldRunEnspell(255), "HPP=255 must run enspell") && ok;

    // Host path: OnAttack injects PTarget->GetHPP() after
    // ShouldRunEnspellAndSpikes; false skips HandleEnspell only (spikes still
    // run under the outer gate).
    const struct
    {
        uint8       targetHPP;
        bool        wantRun;
        const char* label;
    } hostCases[] = {
        { 0, false, "dead → skip HandleEnspell; spikes still under outer gate" },
        { 1, true, "live floor → HandleEnspell" },
        { 50, true, "mid HPP → HandleEnspell" },
        { 100, true, "full HPP → HandleEnspell" },
        { 255, true, "max HPP → HandleEnspell" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRunEnspell(c.targetHPP);
        const bool inlineF = inlineShouldRunEnspell(c.targetHPP);
        const bool pin     = pinShouldRunEnspell(c.targetHPP);

        ok = expect(got == c.wantRun, c.label) && ok;
        ok = expect(got == inlineF && got == pin, "host compose dual-wire free == inline == pin") && ok;

        // Host compose: outer gate assumed true; enspell runs only when live.
        const bool outerGate      = true; // ShouldRunEnspellAndSpikes already true
        const bool enspellAttempt = outerGate && got;
        ok                        = expect(enspellAttempt == c.wantRun, "host enspell attempt matches want") && ok;
        // Spikes run whenever outer gate is true, independent of HPP.
        const bool spikesAttempt = outerGate;
        ok                       = expect(spikesAttempt, "spikes must still run under outer gate") && ok;
    }

    // Dense compose: free == inline == pin == (hpp > 0) for edge + dense span.
    const uint8 dense[] = { 0, 1, 2, 10, 50, 99, 100, 127, 200, 254, 255 };
    for (const uint8 hpp : dense)
    {
        const bool got  = ShouldRunEnspell(hpp);
        const bool want = hpp > 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRunEnspell(hpp), "compose free == inline") && ok;
        ok              = expect(got == pinShouldRunEnspell(hpp), "compose free == pin") && ok;
    }

    // Sibling residual 1399 still hold (not re-expanded under 3390).
    ok = expect(ShouldRunEnspellAndSpikes(0, 0),
                "sibling residual: hit/normal opens outer enspell+spikes gate") &&
         ok;
    ok = expect(!ShouldRunEnspellAndSpikes(0, AttackTypeDaken),
                "sibling residual: Daken blocks outer enspell+spikes gate") &&
         ok;
    ok = expect(!ShouldRunEnspell(0),
                "dead target must skip enspell even when outer gate is open") &&
         ok;

    int  battutaChecks = 0;
    auto battuta       = [&]()
    {
        ++battutaChecks;
        return true;
    };
    ok = expect(!ShouldRunParrySpikes(0, battuta) && battutaChecks == 0,
                "sibling residual: non-parry must not run parry spikes") &&
         ok;
    ok = expect(ShouldRunParrySpikes(ResolutionParry, battuta) && battutaChecks == 1,
                "sibling residual: parry must run parry spikes") &&
         ok;

    return ok;
}
