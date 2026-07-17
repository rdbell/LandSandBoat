#include "test_garrison_fail_by_death_3406.h"

#include "map/garrison_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garrison fail-by-death 3406 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua BATTLE fail-by-death formula for dual-wire checks
// (residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
// prior dedicated expand 3330 / dedicated 3406):
//   allNPCsDead || allPlayersDead
// Direct return of the expression (not if/else). Positive OR only
// (no !(a && b) De Morgan rewrite; QF1001).
auto inlineShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

// Compact dual-wire pin matching Go pinShouldFailByDeath3406:
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

auto inlineAllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

} // namespace

// Pure dual-wire expansion for garrisonhelpers::ShouldFailByDeath
// (Lua xi.garrison BATTLE state fail-by-death gate; dedicated slice 3406;
// expand residual 2870 / pure residual suite; prior dedicated 3073;
// prior dedicated expand residual 3236; prior dedicated expand residual 3330).
//
// Coverage:
//   - free == inline == pin (direct return positive OR; no De Morgan)
//   - residual poles + dense 2² free == inline == pin
//   - residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
//     prior dedicated expand 3330 pins still hold
//   - AllNPCsDead residual compose still holds
auto runGarrisonFailByDeath3406SelfTests() -> bool
{
    using garrisonhelpers::AllNPCsDead;
    using garrisonhelpers::ShouldFailByDeath;

    bool ok = true;

    // Residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
    // prior dedicated expand 3330 pins.
    ok = expect(!ShouldFailByDeath(false, false), "residual: neither dead") && ok;
    ok = expect(ShouldFailByDeath(true, false), "residual: npcs dead only") && ok;
    ok = expect(ShouldFailByDeath(false, true), "residual: players dead only") && ok;
    ok = expect(ShouldFailByDeath(true, true), "residual: both dead") && ok;

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
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool inlineF = inlineShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool pinGot  = pinShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        // Direct return positive OR pin composition (explicit; no De Morgan).
        const bool wantPin = c.allNPCsDead || c.allPlayersDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldFailByDeath dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldFailByDeath dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldFailByDeath == pin formula allNPCsDead || allPlayersDead") && ok;
    }

    // Pin composition: fail when either wipe flag is set.
    ok = expect(!ShouldFailByDeath(false, false), "both false must continue") && ok;
    ok = expect(ShouldFailByDeath(true, false), "allNPCsDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(false, true), "allPlayersDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(true, true), "both true must fail") && ok;

    // Free == pin across residual poles.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath(false, false),
                "free == pin neither dead") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath(true, false),
                "free == pin npcs dead only") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath(false, true),
                "free == pin players dead only") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath(true, true),
                "free == pin both dead") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
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
        }
    }

    // Residual 2870 / prior dedicated 3073 / prior dedicated expand 3236 /
    // prior dedicated expand 3330 still hold.
    ok = expect(!ShouldFailByDeath(false, false) && ShouldFailByDeath(true, false) &&
                    ShouldFailByDeath(false, true) && ShouldFailByDeath(true, true),
                "residual 2870/3073/3236/3330 wipe gate pins under 3406") &&
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
                "prior 3073 pin neither dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3073(true, false),
                "prior 3073 pin npcs dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3073(false, true),
                "prior 3073 pin players dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3073(true, true),
                "prior 3073 pin both dead still holds under 3406") &&
         ok;

    // Residual independence: prior dedicated expand residual 3236 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3236(false, false),
                "prior 3236 pin neither dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3236(true, false),
                "prior 3236 pin npcs dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3236(false, true),
                "prior 3236 pin players dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3236(true, true),
                "prior 3236 pin both dead still holds under 3406") &&
         ok;

    // Residual independence: prior dedicated expand residual 3330 pin still matches free.
    ok = expect(ShouldFailByDeath(false, false) == pinShouldFailByDeath3330(false, false),
                "prior 3330 pin neither dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, false) == pinShouldFailByDeath3330(true, false),
                "prior 3330 pin npcs dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(false, true) == pinShouldFailByDeath3330(false, true),
                "prior 3330 pin players dead still holds under 3406") &&
         ok;
    ok = expect(ShouldFailByDeath(true, true) == pinShouldFailByDeath3330(true, true),
                "prior 3330 pin both dead still holds under 3406") &&
         ok;

    // Residual AllNPCsDead compose (2870 sibling) still holds under 3406.
    ok = expect(ShouldFailByDeath(AllNPCsDead(3, 3), false),
                "AllNPCsDead true must feed fail path when players alive") &&
         ok;
    ok = expect(!ShouldFailByDeath(AllNPCsDead(3, 2), false),
                "partial npcs + players alive must not fail") &&
         ok;
    ok = expect(ShouldFailByDeath(AllNPCsDead(3, 2), true),
                "partial npcs + players dead must fail via players path") &&
         ok;

    // Explicit residual compose dual-wire free == inline for a few host poles.
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

        ok = expect(got == c.wantFail, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
    }

    return ok;
}
