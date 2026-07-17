#include "test_treasure_force_check_full_2981.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldForceCheckOnFullPoolInsert 2981 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline addItem full-pool force-check formula for dual-wire cross-check (slice 2981):
//   slotAfterFreeScan == PoolSize
auto inlineShouldForceCheckOnFullPoolInsert(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == treasurepoolhelpers::PoolSize;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldForceCheckOnFullPoolInsert
// (SlotID == PoolSize after free-slot scan; slice 2981).
auto runTreasureForceCheckFull2981SelfTests() -> bool
{
    using treasurepoolhelpers::PoolSize;
    using treasurepoolhelpers::ShouldForceCheckOnFullPoolInsert;

    bool ok = true;

    const struct
    {
        uint8       slot;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / required edges.
        { 0, false, "first free slot does not force-check" },
        { 9, false, "last valid free slot does not force-check" },
        { 10, true, "PoolSize after free-slot scan force-checks" },
        { 11, false, "one over PoolSize does not force-check" },
        { 255, false, "FreeSlotUnset sentinel does not force-check" },

        // Mid free-slot indices.
        { 1, false, "slot 1 free does not force-check" },
        { 3, false, "residual mid free slot" },
        { 5, false, "mid free slot does not force-check" },

        // Residual 1367 pins.
        { 10, true, "residual force full true" },
        { 3, false, "residual force full false" },

        // Boundary neighbors around PoolSize.
        { 9, false, "one under PoolSize boundary" },
        { 10, true, "exact PoolSize boundary" },
        { 11, false, "one over PoolSize boundary" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldForceCheckOnFullPoolInsert(c.slot);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(c.slot);
        const bool wantPin = c.slot == PoolSize;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldForceCheckOnFullPoolInsert dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldForceCheckOnFullPoolInsert == pin formula == PoolSize") && ok;
    }

    // Pin composition: equality to PoolSize only.
    ok = expect(!ShouldForceCheckOnFullPoolInsert(0), "slot 0 must not force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(9), "slot 9 must not force-check") && ok;
    ok = expect(ShouldForceCheckOnFullPoolInsert(10), "slot 10 (PoolSize) must force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(11), "slot 11 must not force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(255), "slot 255 must not force-check") && ok;
    ok = expect(PoolSize == 10, "PoolSize == 10") && ok;

    // Dense compose over slot domain around PoolSize (0..20) + edges.
    for (uint16 slot = 0; slot <= 20; ++slot)
    {
        const uint8 s    = static_cast<uint8>(slot);
        const bool  got  = ShouldForceCheckOnFullPoolInsert(s);
        const bool  want = s == PoolSize;
        ok               = expect(got == want, "compose free == pin formula") && ok;
        ok               = expect(got == inlineShouldForceCheckOnFullPoolInsert(s), "compose free == inline") && ok;
        ok               = expect(got == (s == PoolSize), "compose free == pin == PoolSize") && ok;
    }
    for (const uint8 s : { uint8{ 0 }, uint8{ 9 }, uint8{ 10 }, uint8{ 11 }, uint8{ 255 } })
    {
        const bool got  = ShouldForceCheckOnFullPoolInsert(s);
        const bool want = s == PoolSize;
        ok              = expect(got == want, "edge free == pin") && ok;
        ok              = expect(got == inlineShouldForceCheckOnFullPoolInsert(s), "edge free == inline") && ok;
    }

    // Explicit dual-wire identity: free == inline == (slot == PoolSize).
    for (const uint8 s : { uint8{ 0 }, uint8{ 1 }, uint8{ 3 }, uint8{ 9 }, uint8{ 10 }, uint8{ 11 }, uint8{ 255 } })
    {
        const bool freeF  = ShouldForceCheckOnFullPoolInsert(s);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(s);
        const bool pin     = s == PoolSize;
        ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
    }

    // Host-style inject poles: SlotID left by free-slot scan into free function.
    // free found (0..9) → no force-check; free-scan exhausted (10) → force-check.
    for (uint8 slot = 0; slot < PoolSize; ++slot)
    {
        ok = expect(!ShouldForceCheckOnFullPoolInsert(slot), "host inject free-found must not force-check") && ok;
    }
    ok = expect(ShouldForceCheckOnFullPoolInsert(PoolSize), "host inject SlotID==PoolSize must force-check") && ok;

    // Production addItem path semantics:
    // SlotID == PoolSize → force checkTreasureItem on FreeSlotID before insert
    // SlotID in [0, PoolSize) → skip force-check
    ok = expect(ShouldForceCheckOnFullPoolInsert(10), "addItem full free-scan → force-check path") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(0), "addItem free slot 0 → no force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(9), "addItem free slot 9 → no force-check") && ok;

    return ok;
}
