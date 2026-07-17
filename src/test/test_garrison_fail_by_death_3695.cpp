#include "test_garrison_fail_by_death_3695.h"

#include "map/garrison_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garrison fail-by-death 3695 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua BATTLE fail-by-death formula for dual-wire checks
// (residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
// prior dedicated expand 3330 / prior dedicated expand 3406 /
// prior dedicated expand 3461 / prior dedicated expand 3514 /
// prior dedicated expand 3560 / prior dedicated expand 3605 /
// prior dedicated expand 3650 / dedicated 3695):
//   allNPCsDead || allPlayersDead
// Direct return of the expression (not if/else). Positive OR only
// (no !(a && b) De Morgan rewrite; QF1001).
auto inlineShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching Go pinShouldFailByDeath3695:
//   allNPCsDead || allPlayersDead
// Direct return OR (positive form only; no De Morgan).
auto pinShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated 3073.
auto pinShouldFailByDeath3073(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3236.
auto pinShouldFailByDeath3236(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3330.
auto pinShouldFailByDeath3330(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3406.
auto pinShouldFailByDeath3406(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3461.
auto pinShouldFailByDeath3461(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3514.
auto pinShouldFailByDeath3514(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3560.
auto pinShouldFailByDeath3560(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3605.
auto pinShouldFailByDeath3605(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching prior dedicated expand residual 3650.
auto pinShouldFailByDeath3650(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

auto inlineAllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

} // namespace

// Pure dual-wire expansion for garrisonhelpers::ShouldFailByDeath
// (Lua xi.garrison BATTLE state fail-by-death gate; dedicated slice 3695;
// expand residual 2870 / pure residual suite; prior dedicated 3073;
// prior dedicated expand residual 3236; prior dedicated expand residual 3330;
// prior dedicated expand residual 3406; prior dedicated expand residual 3461;
// prior dedicated expand residual 3514; prior dedicated expand residual 3560;
// prior dedicated expand residual 3605; prior dedicated expand residual 3650).
//
// Coverage:
//   - free == inline == pin == pin3650 (direct return positive OR; no De Morgan)
//   - 4-row truth table + residual poles + dense 2² free == inline == pin == pin3650
//   - residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
//     prior dedicated expand 3330 / prior dedicated expand 3406 /
//     prior dedicated expand 3461 / prior dedicated expand 3514 /
//     prior dedicated expand 3560 / prior dedicated expand 3605 /
//     prior dedicated expand 3650 pins still hold
//   - AllNPCsDead residual compose still holds
auto runGarrisonFailByDeath3695SelfTests() -> bool
{
    using garrisonhelpers::AllNPCsDead;
    using garrisonhelpers::ShouldFailByDeath;

    bool ok = true;

    // Residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
    // prior dedicated expand 3330 / prior dedicated expand 3406 /
    // prior dedicated expand 3461 / prior dedicated expand 3514 /
    // prior dedicated expand 3560 / prior dedicated expand 3605 /
    // prior dedicated expand 3650 pins.
    ok = expect(!ShouldFailByDeath(false, false), "residual: neither dead") && ok;
    ok = expect(ShouldFailByDeath(true, false), "residual: npcs dead only") && ok;
    ok = expect(ShouldFailByDeath(false, true), "residual: players dead only") && ok;
    ok = expect(ShouldFailByDeath(true, true), "residual: both dead") && ok;

    // 4-row truth table (+ residual re-pins): free == inline == pin == pin3650.
    const struct
    {
        bool        allNPCsDead;
        bool        allPlayersDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic reject pole — both sides alive.
        { false, false, false, "neither dead continues BATTLE" },

        // Residual 2870 admit poles (2-input OR).
        { true, false, true, "npcs dead only fails" },
        { false, true, true, "players dead only fails" },
        { true, true, true, "both dead fails" },

        // Residual 2870 re-pins.
        { false, false, false, "residual 2870 neither dead" },
        { true, false, true, "residual 2870 npcs dead only" },
        { false, true, true, "residual 2870 players dead only" },
        { true, true, true, "residual 2870 both dead" },

        // Prior dedicated 3073 re-pins.
        { false, false, false, "prior dedicated 3073 neither dead" },
        { true, false, true, "prior dedicated 3073 npcs dead only" },
        { false, true, true, "prior dedicated 3073 players dead only" },
        { true, true, true, "prior dedicated 3073 both dead" },

        // Prior dedicated expand residual 3236 re-pins.
        { false, false, false, "prior dedicated 3236 neither dead" },
        { true, false, true, "prior dedicated 3236 npcs dead only" },
        { false, true, true, "prior dedicated 3236 players dead only" },
        { true, true, true, "prior dedicated 3236 both dead" },

        // Prior dedicated expand residual 3330 re-pins.
        { false, false, false, "prior dedicated 3330 neither dead" },
        { true, false, true, "prior dedicated 3330 npcs dead only" },
        { false, true, true, "prior dedicated 3330 players dead only" },
        { true, true, true, "prior dedicated 3330 both dead" },

        // Prior dedicated expand residual 3406 re-pins.
        { false, false, false, "prior dedicated 3406 neither dead" },
        { true, false, true, "prior dedicated 3406 npcs dead only" },
        { false, true, true, "prior dedicated 3406 players dead only" },
        { true, true, true, "prior dedicated 3406 both dead" },

        // Prior dedicated expand residual 3461 re-pins.
        { false, false, false, "prior dedicated 3461 neither dead" },
        { true, false, true, "prior dedicated 3461 npcs dead only" },
        { false, true, true, "prior dedicated 3461 players dead only" },
        { true, true, true, "prior dedicated 3461 both dead" },

        // Prior dedicated expand residual 3514 re-pins.
        { false, false, false, "prior dedicated 3514 neither dead" },
        { true, false, true, "prior dedicated 3514 npcs dead only" },
        { false, true, true, "prior dedicated 3514 players dead only" },
        { true, true, true, "prior dedicated 3514 both dead" },

        // Prior dedicated expand residual 3560 re-pins.
        { false, false, false, "prior dedicated 3560 neither dead" },
        { true, false, true, "prior dedicated 3560 npcs dead only" },
        { false, true, true, "prior dedicated 3560 players dead only" },
        { true, true, true, "prior dedicated 3560 both dead" },

        // Prior dedicated expand residual 3605 re-pins.
        { false, false, false, "prior dedicated 3605 neither dead" },
        { true, false, true, "prior dedicated 3605 npcs dead only" },
        { false, true, true, "prior dedicated 3605 players dead only" },
        { true, true, true, "prior dedicated 3605 both dead" },

        // Prior dedicated expand residual 3650 re-pins.
        { false, false, false, "prior dedicated 3650 neither dead" },
        { true, false, true, "prior dedicated 3650 npcs dead only" },
        { false, true, true, "prior dedicated 3650 players dead only" },
        { true, true, true, "prior dedicated 3650 both dead" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool inlineF = inlineShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool pinGot  = pinShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool pin3650 = pinShouldFailByDeath3650(c.allNPCsDead, c.allPlayersDead);
        // Direct return positive OR pin composition (explicit; no De Morgan).
        const bool wantPin = c.allNPCsDead || c.allPlayersDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldFailByDeath dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldFailByDeath dual-wire == pin") && ok;
        ok = expect(got == pin3650, "ShouldFailByDeath dual-wire == pin3650") && ok;
        ok = expect(got == wantPin, "ShouldFailByDeath == pin formula allNPCsDead || allPlayersDead") && ok;
    }

    // Pin composition: fail when either wipe flag is set.
    ok = expect(!ShouldFailByDeath(false, false), "both false must continue") && ok;
    ok = expect(ShouldFailByDeath(true, false), "allNPCsDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(false, true), "allPlayersDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(true, true), "both true must fail") && ok;

    // Free == pin == pin3650 across residual poles (4-row truth table).
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath(false, false) &&
                    ShouldFailByDeath(false, false) == pinShouldFailByDeath3650(false, false),
                "free == pin/pin3650 neither dead") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath(true, false) &&
                    ShouldFailByDeath(true, false) == pinShouldFailByDeath3650(true, false),
                "free == pin/pin3650 npcs dead only") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath(false, true) &&
                    ShouldFailByDeath(false, true) == pinShouldFailByDeath3650(false, true),
                "free == pin/pin3650 players dead only") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath(true, true) &&
                    ShouldFailByDeath(true, true) == pinShouldFailByDeath3650(true, true),
                "free == pin/pin3650 both dead") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin == pin3650.
    for (const bool allNPCsDead : { false, true })
    {
        for (const bool allPlayersDead : { false, true })
        {
            const bool got  = ShouldFailByDeath(allNPCsDead, allPlayersDead);
            const bool want = allNPCsDead || allPlayersDead;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldFailByDeath(allNPCsDead, allPlayersDead),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldFailByDeath(allNPCsDead, allPlayersDead),
                        "compose free == pin") &&
                 ok;
            ok = expect(got == pinShouldFailByDeath3650(allNPCsDead, allPlayersDead),
                        "compose free == pin3650") &&
                 ok;
        }
    }

    // Residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
    // prior dedicated expand 3330 / prior dedicated expand 3406 /
    // prior dedicated expand 3461 / prior dedicated expand 3514 /
    // prior dedicated expand 3560 / prior dedicated expand 3605 /
    // prior dedicated expand 3650 still hold.
    ok = expect(!ShouldFailByDeath(false, false) && ShouldFailByDeath(true, false) &&
                    ShouldFailByDeath(false, true) && ShouldFailByDeath(true, true),
                "residual 2870/3073/3236/3330/3406/3461/3514/3560/3605/3650 wipe gate pins under 3695") &&
         ok;

    // --- Production Lua BATTLE wipe gate ---
    // Host injects:
    //   allNPCsDead    = #zoneData.npcs == zoneData.deadNPCCount
    //   allPlayersDead = not utils.any(players, isAliveFn)
    // when true  → zoneData.state = ENDED (+ fail message)
    // when false → leave BATTLE running
    ok = expect(!ShouldFailByDeath(false, false), "battle neither wiped → continue") && ok;
    ok = expect(ShouldFailByDeath(true, false), "battle npcs wiped → ENDED") && ok;
    ok = expect(ShouldFailByDeath(false, true), "battle players wiped → ENDED") && ok;
    ok = expect(ShouldFailByDeath(true, true), "battle both wiped → ENDED") && ok;

    // Residual independence: prior dedicated 3073 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3073(false, false),
                "prior 3073 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3073(true, false),
                "prior 3073 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3073(false, true),
                "prior 3073 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3073(true, true),
                "prior 3073 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3236 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3236(false, false),
                "prior 3236 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3236(true, false),
                "prior 3236 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3236(false, true),
                "prior 3236 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3236(true, true),
                "prior 3236 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3330 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3330(false, false),
                "prior 3330 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3330(true, false),
                "prior 3330 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3330(false, true),
                "prior 3330 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3330(true, true),
                "prior 3330 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3406 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3406(false, false),
                "prior 3406 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3406(true, false),
                "prior 3406 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3406(false, true),
                "prior 3406 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3406(true, true),
                "prior 3406 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3461 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3461(false, false),
                "prior 3461 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3461(true, false),
                "prior 3461 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3461(false, true),
                "prior 3461 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3461(true, true),
                "prior 3461 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3514 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3514(false, false),
                "prior 3514 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3514(true, false),
                "prior 3514 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3514(false, true),
                "prior 3514 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3514(true, true),
                "prior 3514 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3560 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3560(false, false),
                "prior 3560 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3560(true, false),
                "prior 3560 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3560(false, true),
                "prior 3560 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3560(true, true),
                "prior 3560 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3605 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3605(false, false),
                "prior 3605 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3605(true, false),
                "prior 3605 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3605(false, true),
                "prior 3605 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3605(true, true),
                "prior 3605 pin both dead still holds under 3695") &&
         ok;

    // Residual independence: prior dedicated expand residual 3650 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3650(false, false),
                "prior 3650 pin neither dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3650(true, false),
                "prior 3650 pin npcs dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3650(false, true),
                "prior 3650 pin players dead still holds under 3695") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3650(true, true),
                "prior 3650 pin both dead still holds under 3695") &&
         ok;

    // Residual AllNPCsDead compose (2870 sibling) still holds under 3695.
    ok = expect(ShouldFailByDeath(AllNPCsDead(3, 3), false),
                "AllNPCsDead true must feed fail path when players alive") &&
         ok;
    ok = expect(!ShouldFailByDeath(AllNPCsDead(3, 2), false),
                "partial npcs + players alive must not fail") &&
         ok;
    ok = expect(ShouldFailByDeath(AllNPCsDead(3, 2), true),
                "partial npcs + players dead must fail via players path") &&
         ok;

    // Explicit residual compose dual-wire free == inline == pin == pin3650 for host poles.
    const struct
    {
        int32       npcCount;
        int32       deadNPCCount;
        bool        allPlayersDead;
        bool        wantFail;
        const char* label;
    } composeCases[] = {
        { 0, 0, false, true, "compose empty npcs (all dead)" },
        { 3, 0, false, false, "compose none dead, players alive" },
        { 3, 3, false, true, "compose all npcs dead" },
        { 3, 2, true, true, "compose partial npcs, players wiped" },
        { 3, 2, false, false, "compose partial npcs, players alive" },
        { 5, 5, true, true, "compose both wipe paths" },
        { 1, 1, false, true, "compose single npc dead" },
        { 1, 0, true, true, "compose single npc alive players wiped" },
        { 1, 0, false, false, "compose single npc alive players alive" },
    };

    for (const auto& c : composeCases)
    {
        const bool npcsDead  = AllNPCsDead(c.npcCount, c.deadNPCCount);
        const bool got       = ShouldFailByDeath(npcsDead, c.allPlayersDead);
        const bool inlineGot = inlineShouldFailByDeath(inlineAllNPCsDead(c.npcCount, c.deadNPCCount), c.allPlayersDead);
        const bool pinGot    = pinShouldFailByDeath(npcsDead, c.allPlayersDead);
        const bool pin3650   = pinShouldFailByDeath3650(npcsDead, c.allPlayersDead);

        ok = expect(got == c.wantFail, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == pin3650, "compose dual-wire free == pin3650") && ok;
    }

    return ok;
}
