#include "test_treasure_lot_inventory_3367.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure CanLotWithInventory 3367 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem free-inventory formula for dual-wire cross-check (dedicated 3367):
//   freeSlots != 0
auto inlineCanLotWithInventory(const uint8 freeSlots) -> bool
{
    return freeSlots != 0;
}

// Compact dual-wire pin matching Go pinCanLotWithInventory3367:
//   freeSlots != 0
auto pinCanLotWithInventory(const uint8 freeSlots) -> bool
{
    return freeSlots != 0;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::CanLotWithInventory
// (freeSlots != 0 inventory lot gate; OmegaXI internal/treasurepool; dedicated
// slice 3367 expand residual 2957).
//
// Coverage:
//   - free == inline == pin == (freeSlots != 0)
//   - residual 1367 / 2957 pins still hold
//   - residual poles + dense free-slot compose + PlanLotItemPreflight inject
auto runTreasureLotInventory3367SelfTests() -> bool
{
    using treasurepoolhelpers::CanLotWithInventory;
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    bool ok = true;

    // Residual 1367 / 2957 pins still hold under dual-wire.
    ok = expect(!CanLotWithInventory(0), "residual freeSlots == 0 rejects") && ok;
    ok = expect(CanLotWithInventory(1), "residual freeSlots == 1 accepts") && ok;
    ok = expect(CanLotWithInventory(255), "residual freeSlots == 255 accepts") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        uint8       freeSlots;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1367 / 2957 poles.
        { 0, false, "residual full inventory rejects" },
        { 1, true, "residual one free slot accepts" },

        // Classic dual poles.
        { 0, false, "full inventory rejects" },
        { 1, true, "one free slot accepts" },

        // Empty-adjacent / multi-slot extremes.
        { 2, true, "two free slots accepts" },
        { 5, true, "mid free slots accepts" },
        { 80, true, "typical bag free slots accepts" },
        { 255, true, "uint8 max free slots accepts" },

        // Residual 2772 preflight-facing pins.
        { 0, false, "residual full inv preflight pin" },
        { 1, true, "residual one free preflight pin" },

        // Residual 2957 poles still hold.
        { 0, false, "prior 2957 full reject" },
        { 1, true, "prior 2957 one free accept" },
        { 2, true, "prior 2957 two free accept" },
        { 80, true, "prior 2957 bag free accept" },
        { 255, true, "prior 2957 uint8 max accept" },

        // Boundary re-pins around empty.
        { 0, false, "zero free boundary reject" },
        { 1, true, "one free boundary accept" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanLotWithInventory(c.freeSlots);
        const bool inlineF = inlineCanLotWithInventory(c.freeSlots);
        const bool pin     = pinCanLotWithInventory(c.freeSlots);
        const bool wantPin = c.freeSlots != 0;

        ok = expect(got == c.want, c.label) && ok;
        // free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula freeSlots != 0") && ok;
    }

    // Explicit residual poles free == inline == pin for classic cells.
    for (const uint8 freeSlots : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool got     = CanLotWithInventory(freeSlots);
        const bool inlineF = inlineCanLotWithInventory(freeSlots);
        const bool pin     = pinCanLotWithInventory(freeSlots);
        const bool want    = freeSlots != 0;
        ok                 = expect(got == want, "pole free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
    }

    // Dense compose over representative free-slot counts — free == inline == pin.
    for (uint16 freeSlots = 0; freeSlots <= 20; ++freeSlots)
    {
        const uint8 fs     = static_cast<uint8>(freeSlots);
        const bool  got    = CanLotWithInventory(fs);
        const bool  inlineF = inlineCanLotWithInventory(fs);
        const bool  pin    = pinCanLotWithInventory(fs);
        const bool  want   = fs != 0;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Host-style inject poles through PlanLotItemPreflight (all earlier gates
    // clear so inventory is the discriminating factor).
    // free == inline == pin and disposition polarity.
    ok = expect(!CanLotWithInventory(0), "inject full inv free rejects") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 0, false, false) == LotItemPreflight::RejectFullInventory,
                "inject full inv → RejectFullInventory") &&
         ok;
    ok = expect(CanLotWithInventory(1), "inject one free free accepts") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "inject one free → Proceed") &&
         ok;
    ok = expect(CanLotWithInventory(5), "inject multi free free accepts") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 5, false, false) == LotItemPreflight::Proceed,
                "inject multi free → Proceed") &&
         ok;

    // Dual-wire: free gate polarity matches preflight disposition for inventory
    // free == inline == pin across representative free-slot poles.
    for (const uint8 freeSlots : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool canLot    = CanLotWithInventory(freeSlots);
        const bool inlineF   = inlineCanLotWithInventory(freeSlots);
        const bool pin       = pinCanLotWithInventory(freeSlots);
        const auto preflight = PlanLotItemPreflight(false, false, false, false, freeSlots, false, false);
        ok                   = expect(canLot == inlineF && canLot == pin, "preflight free == inline == pin") && ok;
        if (canLot)
        {
            ok = expect(preflight == LotItemPreflight::Proceed, "canLot preflight Proceed") && ok;
        }
        else
        {
            ok = expect(preflight == LotItemPreflight::RejectFullInventory, "full inv preflight Reject") && ok;
        }
    }

    // Production lotItem path semantics:
    // freeSlots == 0 → RejectFullInventory
    // freeSlots != 0 → Proceed past inventory gate
    ok = expect(!CanLotWithInventory(0), "lotItem full inv → reject path") && ok;
    ok = expect(CanLotWithInventory(1), "lotItem one free → accept path") && ok;
    ok = expect(CanLotWithInventory(10), "lotItem multi free → accept path") && ok;

    return ok;
}
