#include "test_treasure_force_check_full_3700.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldForceCheckOnFullPoolInsert 3700 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline addItem full-pool force-check formula for dual-wire cross-check (dedicated 3700):
//   slotAfterFreeScan == PoolSize
auto inlineShouldForceCheckOnFullPoolInsert(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == treasurepoolhelpers::PoolSize;
}

// Compact dual-wire pin matching Go pinShouldForceCheckOnFullPoolInsert3700:
//   slotAfterFreeScan == PoolSize
auto pinShouldForceCheckOnFullPoolInsert(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == treasurepoolhelpers::PoolSize;
}

// Compact dual-wire pin matching Go pinShouldForceCheckOnFullPoolInsert3655
// (prior dedicated retained under 3700):
//   slotAfterFreeScan == PoolSize
auto pinShouldForceCheckOnFullPoolInsert3655(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == treasurepoolhelpers::PoolSize;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldForceCheckOnFullPoolInsert
// (SlotID == PoolSize after free-slot scan; OmegaXI internal/treasurepool; dedicated
// slice 3700 expand residual 2981; prior dedicated 3655 / 3610 / 3565 / 3533 / 3477 / 3423 / 3379 retained).
//
// Coverage:
//   - free == inline == pin == pin3655 == (slotAfterFreeScan == PoolSize)
//   - residual 1367 / 2981 pins still hold
//   - prior dedicated 3655 / 3610 / 3565 / 3533 / 3477 / 3423 / 3379 poles still hold
//   - residual poles + dense slot compose + host-style SlotID inject
auto runTreasureForceCheckFull3700SelfTests() -> bool
{
    using treasurepoolhelpers::PoolSize;
    using treasurepoolhelpers::ShouldForceCheckOnFullPoolInsert;

    bool ok = true;

    // Residual 1367 / 2981 pins still hold under dual-wire.
    ok = expect(!ShouldForceCheckOnFullPoolInsert(0), "residual slot 0 does not force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(9), "residual slot 9 does not force-check") && ok;
    ok = expect(ShouldForceCheckOnFullPoolInsert(10), "residual slot 10 (PoolSize) force-checks") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(11), "residual slot 11 does not force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(255), "residual slot 255 does not force-check") && ok;
    ok = expect(PoolSize == 10, "residual PoolSize == 10") && ok;

    // --- Composition table: free == inline == pin == pin3655 ---
    const struct
    {
        uint8       slot;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1367 / 2981 poles.
        { 0, false, "residual first free slot does not force-check" },
        { 9, false, "residual last valid free slot does not force-check" },
        { 10, true, "residual PoolSize after free-slot scan force-checks" },
        { 11, false, "residual one over PoolSize does not force-check" },
        { 255, false, "residual FreeSlotUnset sentinel does not force-check" },

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

        // Residual 2981 poles still hold.
        { 0, false, "prior 2981 first free no force" },
        { 9, false, "prior 2981 last free no force" },
        { 10, true, "prior 2981 PoolSize force" },
        { 11, false, "prior 2981 one over no force" },
        { 255, false, "prior 2981 sentinel no force" },

        // Prior dedicated 3379 poles still hold.
        { 0, false, "prior 3379 first free no force" },
        { 9, false, "prior 3379 last free no force" },
        { 10, true, "prior 3379 PoolSize force" },
        { 11, false, "prior 3379 one over no force" },
        { 255, false, "prior 3379 sentinel no force" },

        // Prior dedicated 3423 poles still hold.
        { 0, false, "prior 3423 first free no force" },
        { 9, false, "prior 3423 last free no force" },
        { 10, true, "prior 3423 PoolSize force" },
        { 11, false, "prior 3423 one over no force" },
        { 255, false, "prior 3423 sentinel no force" },

        // Prior dedicated 3477 poles still hold.
        { 0, false, "prior 3477 first free no force" },
        { 9, false, "prior 3477 last free no force" },
        { 10, true, "prior 3477 PoolSize force" },
        { 11, false, "prior 3477 one over no force" },
        { 255, false, "prior 3477 sentinel no force" },

        // Prior dedicated 3533 poles still hold.
        { 0, false, "prior 3533 first free no force" },
        { 9, false, "prior 3533 last free no force" },
        { 10, true, "prior 3533 PoolSize force" },
        { 11, false, "prior 3533 one over no force" },
        { 255, false, "prior 3533 sentinel no force" },

        // Prior dedicated 3565 poles still hold.
        { 0, false, "prior 3565 first free no force" },
        { 9, false, "prior 3565 last free no force" },
        { 10, true, "prior 3565 PoolSize force" },
        { 11, false, "prior 3565 one over no force" },
        { 255, false, "prior 3565 sentinel no force" },

        // Prior dedicated 3610 poles still hold.
        { 0, false, "prior 3610 first free no force" },
        { 9, false, "prior 3610 last free no force" },
        { 10, true, "prior 3610 PoolSize force" },
        { 11, false, "prior 3610 one over no force" },
        { 255, false, "prior 3610 sentinel no force" },

        // Prior dedicated 3655 poles still hold.
        { 0, false, "prior 3655 first free no force" },
        { 9, false, "prior 3655 last free no force" },
        { 10, true, "prior 3655 PoolSize force" },
        { 11, false, "prior 3655 one over no force" },
        { 255, false, "prior 3655 sentinel no force" },

        // Boundary re-pins around PoolSize.
        { 9, false, "one under PoolSize boundary" },
        { 10, true, "exact PoolSize boundary" },
        { 11, false, "one over PoolSize boundary" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldForceCheckOnFullPoolInsert(c.slot);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(c.slot);
        const bool pin     = pinShouldForceCheckOnFullPoolInsert(c.slot);
        const bool pin3655 = pinShouldForceCheckOnFullPoolInsert3655(c.slot);
        const bool wantPin = c.slot == PoolSize;

        ok = expect(got == c.want, c.label) && ok;
        // free == inline == pin == pin3655.
        ok = expect(got == inlineF && got == pin && got == pin3655, "dual-wire free == inline == pin == pin3655") && ok;
        ok = expect(got == wantPin, "free == pin formula slot == PoolSize") && ok;
    }

    // Explicit residual poles free == inline == pin == pin3655 for classic cells.
    for (const uint8 s : { uint8{ 0 }, uint8{ 1 }, uint8{ 3 }, uint8{ 9 }, uint8{ 10 }, uint8{ 11 }, uint8{ 255 } })
    {
        const bool got     = ShouldForceCheckOnFullPoolInsert(s);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(s);
        const bool pin     = pinShouldForceCheckOnFullPoolInsert(s);
        const bool pin3655 = pinShouldForceCheckOnFullPoolInsert3655(s);
        const bool want    = s == PoolSize;
        ok                 = expect(got == want, "pole free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3655, "pole free == inline == pin == pin3655") && ok;
    }

    // Dense compose over slot domain around PoolSize (0..20) — free == inline == pin == pin3655.
    for (uint16 slot = 0; slot <= 20; ++slot)
    {
        const uint8 s       = static_cast<uint8>(slot);
        const bool  got     = ShouldForceCheckOnFullPoolInsert(s);
        const bool  inlineF = inlineShouldForceCheckOnFullPoolInsert(s);
        const bool  pin     = pinShouldForceCheckOnFullPoolInsert(s);
        const bool  pin3655 = pinShouldForceCheckOnFullPoolInsert3655(s);
        const bool  want    = s == PoolSize;
        ok                  = expect(got == want, "compose free == pin formula") && ok;
        ok                  = expect(got == inlineF && got == pin && got == pin3655, "compose free == inline == pin == pin3655") && ok;
    }

    // Host-style inject poles: SlotID left by free-slot scan into free function.
    // free found (0..9) → no force-check; free-scan exhausted (10) → force-check.
    // free == inline == pin == pin3655 residual pins.
    for (uint8 slot = 0; slot < PoolSize; ++slot)
    {
        const bool got     = ShouldForceCheckOnFullPoolInsert(slot);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(slot);
        const bool pin     = pinShouldForceCheckOnFullPoolInsert(slot);
        const bool pin3655 = pinShouldForceCheckOnFullPoolInsert3655(slot);
        ok                 = expect(!got, "host inject free-found must not force-check") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3655, "host inject free-found free == inline == pin == pin3655") && ok;
    }
    {
        const bool got     = ShouldForceCheckOnFullPoolInsert(PoolSize);
        const bool inlineF = inlineShouldForceCheckOnFullPoolInsert(PoolSize);
        const bool pin     = pinShouldForceCheckOnFullPoolInsert(PoolSize);
        const bool pin3655 = pinShouldForceCheckOnFullPoolInsert3655(PoolSize);
        ok                 = expect(got, "host inject SlotID==PoolSize must force-check") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3655, "host inject PoolSize free == inline == pin == pin3655") && ok;
    }

    // Production addItem path semantics:
    // SlotID == PoolSize → force checkTreasureItem on FreeSlotID before insert
    // SlotID in [0, PoolSize) → skip force-check
    ok = expect(ShouldForceCheckOnFullPoolInsert(10), "addItem full free-scan → force-check path") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(0), "addItem free slot 0 → no force-check") && ok;
    ok = expect(!ShouldForceCheckOnFullPoolInsert(9), "addItem free slot 9 → no force-check") && ok;

    return ok;
}
