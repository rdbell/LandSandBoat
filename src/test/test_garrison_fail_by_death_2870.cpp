#include "test_garrison_fail_by_death_2870.h"

#include "map/garrison_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "garrison fail-by-death 2870 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua BATTLE fail-by-death formula for dual-wire checks:
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
// (Lua xi.garrison BATTLE state fail-by-death gate).
auto runGarrisonFailByDeath2870SelfTests() -> bool
{
    using garrisonhelpers::AllNPCsDead;
    using garrisonhelpers::ShouldFailByDeath;

    bool ok = true;

    // Truth table for ShouldFailByDeath (2-input OR).
    ok = expect(!ShouldFailByDeath(false, false), "neither dead") && ok;
    ok = expect(ShouldFailByDeath(true, false), "npcs dead only") && ok;
    ok = expect(ShouldFailByDeath(false, true), "players dead only") && ok;
    ok = expect(ShouldFailByDeath(true, true), "both dead") && ok;

    // AllNPCsDead pure equality (#npcs == deadNPCCount).
    ok = expect(AllNPCsDead(0, 0), "empty npc list zero dead") && ok;
    ok = expect(!AllNPCsDead(0, 1), "empty list with dead count 1") && ok;
    ok = expect(!AllNPCsDead(3, 0), "3 npcs none dead") && ok;
    ok = expect(!AllNPCsDead(3, 2), "3 npcs 2 dead") && ok;
    ok = expect(AllNPCsDead(3, 3), "3 npcs all dead") && ok;
    ok = expect(!AllNPCsDead(3, 4), "dead exceeds count") && ok;
    ok = expect(AllNPCsDead(1, 1), "single npc dead") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        bool        allNPCsDead;
        bool        allPlayersDead;
        bool        want;
        const char* label;
    } failCases[] = {
        { false, false, false, "table neither" },
        { true, false, true, "table npcs" },
        { false, true, true, "table players" },
        { true, true, true, "table both" },
    };

    for (const auto& c : failCases)
    {
        const bool got       = ShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);
        const bool inlineGot = inlineShouldFailByDeath(c.allNPCsDead, c.allPlayersDead);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // AllNPCsDead dual-wire + compose into ShouldFailByDeath.
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

    return ok;
}
