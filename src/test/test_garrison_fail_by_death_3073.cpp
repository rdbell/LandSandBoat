#include "test_garrison_fail_by_death_3073.h"

#include "map/garrison_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garrison fail-by-death 3073 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua BATTLE fail-by-death formula for dual-wire checks (slice 3073):
//   allNPCsDead or allPlayersDead
// allNPCsDead = (#npcs == deadNPCCount)
auto inlineShouldFailByDeath(const bool allNPCsDead, const bool allPlayersDead) -> bool
{
    return allNPCsDead || allPlayersDead;
}

auto inlineAllNPCsDead(const int32 npcCount, const int32 deadNPCCount) -> bool
{
    return npcCount == deadNPCCount;
}

} // namespace

// Pure dual-wire expansion for garrisonhelpers::ShouldFailByDeath
// (Lua xi.garrison BATTLE state fail-by-death gate; slice 3073).
auto runGarrisonFailByDeath3073SelfTests() -> bool
{
    using garrisonhelpers::AllNPCsDead;
    using garrisonhelpers::ShouldFailByDeath;

    bool ok = true;

    // Residual 2870 pins still hold under dual-wire.
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
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool inlineF = inlineShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool wantPin = c.allNPCsDead || c.allPlayersDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldFailByDeath dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldFailByDeath == pin formula allNPCsDead || allPlayersDead") && ok;
    }

    // Pin composition: fail when either wipe flag is set.
    ok = expect(!ShouldFailByDeath(false, false), "both false must continue") && ok;
    ok = expect(ShouldFailByDeath(true, false), "allNPCsDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(false, true), "allPlayersDead true must fail") && ok;
    ok = expect(ShouldFailByDeath(true, true), "both true must fail") && ok;

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
        }
    }

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

    // Explicit dual-wire: free function is OR of injects.
    for (const bool allNPCsDead : { false, true })
    {
        for (const bool allPlayersDead : { false, true })
        {
            ok = expect(ShouldFailByDeath(allNPCsDead, allPlayersDead) ==
                            (allNPCsDead || allPlayersDead),
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldFailByDeath(allNPCsDead, allPlayersDead) ==
                            inlineShouldFailByDeath(allNPCsDead, allPlayersDead),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Residual AllNPCsDead compose (2870 sibling): host pre-resolves NPC wipe.
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
        { 1, 0, true, true, "compose single npc alive, players wiped" },
        { 1, 0, false, false, "compose single npc alive, players alive" },
    };

    for (const auto& c : composeCases)
    {
        const bool npcsDead  = AllNPCsDead(c.npcCount, c.deadNPCCount);
        const bool got       = ShouldFailByDeath(npcsDead, c.allPlayersDead);
        const bool inlineGot = inlineShouldFailByDeath(inlineAllNPCsDead(c.npcCount, c.deadNPCCount), c.allPlayersDead);

        ok = expect(got == c.wantFail, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(npcsDead == inlineAllNPCsDead(c.npcCount, c.deadNPCCount), "AllNPCsDead dual-wire") && ok;
    }

    // Sibling residual independence: AllNPCsDead is pure equality (2870);
    // ShouldFailByDeath ORs that flag with players-dead.
    ok = expect(AllNPCsDead(3, 3) && ShouldFailByDeath(true, false),
                "AllNPCsDead true must feed fail path when players alive") &&
         ok;
    ok = expect(!AllNPCsDead(3, 2) && !ShouldFailByDeath(false, false),
                "partial npcs + players alive must not fail") &&
         ok;
    ok = expect(!AllNPCsDead(3, 2) && ShouldFailByDeath(false, true),
                "partial npcs + players dead must fail via players path") &&
         ok;

    return ok;
}
