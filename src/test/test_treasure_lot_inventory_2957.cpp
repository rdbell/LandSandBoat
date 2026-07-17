#include "test_treasure_lot_inventory_2957.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure CanLotWithInventory 2957 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem free-inventory formula for dual-wire cross-check (slice 2957):
//   freeSlots != 0
auto inlineCanLotWithInventory(const uint8 freeSlots) -> bool
{
    return freeSlots != 0;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::CanLotWithInventory
// (freeSlots != 0 inventory lot gate; slice 2957).
auto runTreasureLotInventory2957SelfTests() -> bool
{
    using treasurepoolhelpers::CanLotWithInventory;
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    bool ok = true;

    const struct
    {
        uint8       freeSlots;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 0, false, "full inventory rejects" },
        { 1, true, "one free slot accepts" },

        // Empty-adjacent / multi-slot extremes.
        { 2, true, "two free slots accepts" },
        { 5, true, "mid free slots accepts" },
        { 80, true, "typical bag free slots accepts" },
        { 255, true, "uint8 max free slots accepts" },

        // Residual 1367 pins.
        { 0, false, "residual full" },
        { 1, true, "residual one free" },

        // Boundary neighbors around empty.
        { 0, false, "zero free boundary" },
        { 1, true, "one free boundary" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanLotWithInventory(c.freeSlots);
        const bool inlineF = inlineCanLotWithInventory(c.freeSlots);
        const bool wantPin = c.freeSlots != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanLotWithInventory dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanLotWithInventory == pin formula freeSlots != 0") && ok;
    }

    // Pin composition: any non-zero freeSlots accepts; zero rejects.
    ok = expect(!CanLotWithInventory(0), "freeSlots == 0 must reject") && ok;
    ok = expect(CanLotWithInventory(1), "freeSlots == 1 must accept") && ok;
    ok = expect(CanLotWithInventory(255), "freeSlots == 255 must accept") && ok;

    // Dense compose over representative free-slot counts.
    for (uint16 freeSlots = 0; freeSlots <= 20; ++freeSlots)
    {
        const uint8 fs  = static_cast<uint8>(freeSlots);
        const bool  got = CanLotWithInventory(fs);
        const bool  want = fs != 0;
        ok               = expect(got == want, "compose free == pin formula") && ok;
        ok               = expect(got == inlineCanLotWithInventory(fs), "compose free == inline") && ok;
    }

    // Host-style inject poles: GetFreeSlotsCount() as uint8 into free function.
    // (Live CTreasurePool lotItem / PlanLotItemPreflight is residual 1367 /
    // 2772 treasure_pool tests.)
    for (const uint8 n : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool inject = n != 0; // mirrors GetFreeSlotsCount() != 0 host inject
        ok                = expect(CanLotWithInventory(n) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(CanLotWithInventory(n) == inlineCanLotWithInventory(n),
                    "host inject free == inline") &&
             ok;
    }

    // Production lotItem path semantics via PlanLotItemPreflight:
    // freeSlots == 0 → RejectFullInventory
    // freeSlots != 0 → continue past inventory gate (Proceed when other gates clear)
    ok = expect(PlanLotItemPreflight(false, false, false, false, 0, false, false) == LotItemPreflight::RejectFullInventory,
                "lotItem full inv → RejectFullInventory") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "lotItem one free → Proceed") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 5, false, false) == LotItemPreflight::Proceed,
                "lotItem multi free → Proceed") &&
         ok;

    // Dual-wire: free gate polarity matches preflight disposition for inventory.
    for (const uint8 freeSlots : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool canLot    = CanLotWithInventory(freeSlots);
        const auto preflight = PlanLotItemPreflight(false, false, false, false, freeSlots, false, false);
        if (canLot)
        {
            ok = expect(preflight == LotItemPreflight::Proceed, "canLot preflight Proceed") && ok;
        }
        else
        {
            ok = expect(preflight == LotItemPreflight::RejectFullInventory, "full inv preflight Reject") && ok;
        }
        ok = expect(canLot == inlineCanLotWithInventory(freeSlots), "preflight free == inline") && ok;
        ok = expect(canLot == (freeSlots != 0), "preflight free == pin") && ok;
    }

    return ok;
}
