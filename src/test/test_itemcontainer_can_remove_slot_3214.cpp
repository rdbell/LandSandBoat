#include "test_itemcontainer_can_remove_slot_3214.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer CanRemoveSlot 3214 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RemoveItem range gate for dual-wire cross-check (dedicated 3214):
//   slotID <= size
auto inlineCanRemoveSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

// Compact dual-wire pin matching Go pinCanRemoveSlot3214 / C++ capacity:
//   slotID <= size
auto pinCanRemoveSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::CanRemoveSlot
// (RemoveItem range gate; OmegaXI internal/itemcontainer;
// dedicated slice 3214 expand residual 2976).
//
// Coverage:
//   - free == inline == pin == (slotID <= size)
//   - residual 2802 / 2976 pins still hold
//   - poles: slot 0, slot==size, slot==size+1
auto runItemcontainerCanRemoveSlot3214SelfTests() -> bool
{
    using itemcontainerhelpers::CanRemoveSlot;

    bool ok = true;

    // Residual 2802 / 2976 pins still hold under dual-wire.
    ok = expect(CanRemoveSlot(0, 0), "residual remove slot 0 size 0") && ok;
    ok = expect(CanRemoveSlot(5, 5), "residual remove at size boundary") && ok;
    ok = expect(!CanRemoveSlot(6, 5), "residual remove past size") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        std::uint8_t slotID;
        std::uint8_t size;
        bool         want;
        const char*  label;
    } cases[] = {
        // Residual 2802 / 2976 poles.
        { 0, 0, true, "residual slot 0 size 0" },
        { 5, 5, true, "residual at size boundary" },
        { 6, 5, false, "residual past size" },
        { 0, 3, true, "residual slot 0 included" },
        { 3, 3, true, "residual remove at size boundary" },
        { 4, 3, false, "residual remove past size" },
        { 0, 120, true, "residual slot 0 at max container" },
        { 120, 120, true, "residual max container boundary" },
        { 121, 120, false, "residual past max container" },
        { 255, 120, false, "residual uint8 high past size" },
        { 255, 255, true, "residual both max uint8" },
        { 0, 255, true, "residual slot 0 size max" },
        { 1, 0, false, "residual size 0 slot 1 rejected" },

        // Classic dual poles.
        { 0, 0, true, "slot 0 size 0 accepted" },
        { 6, 5, false, "past size rejected" },

        // Pole: slot 0 always addressable when size allows.
        { 0, 0, true, "pole slot0 size0" },
        { 0, 1, true, "pole slot0 size1" },
        { 0, 3, true, "pole slot0 size3" },
        { 0, 5, true, "pole slot0 size5" },
        { 0, 120, true, "pole slot0 max container" },
        { 0, 255, true, "pole slot0 max uint8" },

        // Pole: slotID == size (inclusive boundary accept).
        { 0, 0, true, "pole slot==size zero" },
        { 1, 1, true, "pole slot==size one" },
        { 3, 3, true, "pole slot==size three" },
        { 5, 5, true, "pole slot==size five" },
        { 120, 120, true, "pole slot==size max container" },
        { 255, 255, true, "pole slot==size max uint8" },

        // Pole: slotID == size+1 (one past size reject; size < 255).
        { 1, 0, false, "pole slot==size+1 zero" },
        { 2, 1, false, "pole slot==size+1 one" },
        { 4, 3, false, "pole slot==size+1 three" },
        { 6, 5, false, "pole slot==size+1 five" },
        { 121, 120, false, "pole slot==size+1 max container" },

        // Boundary neighbors around size.
        { 4, 5, true, "one under size accepted" },
        { 5, 5, true, "at size boundary accepted" },
        { 6, 5, false, "one over size rejected" },

        // Max container bounds (MAX_CONTAINER_SIZE = 120).
        { 119, 120, true, "one under max container" },
        { 120, 120, true, "max container boundary" },
        { 121, 120, false, "past max container" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRemoveSlot(c.slotID, c.size);
        const bool inlineF = inlineCanRemoveSlot(c.slotID, c.size);
        const bool pin     = pinCanRemoveSlot(c.slotID, c.size);
        const bool wantPin = c.slotID <= c.size;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula slotID <= size") && ok;
    }

    // Explicit poles free == inline == pin for 0 / size / size+1.
    const std::uint8_t poleSizes[] = { 0, 1, 3, 5, 8, 120, 254 };
    for (const std::uint8_t size : poleSizes)
    {
        // slot 0
        {
            const bool got     = CanRemoveSlot(0, size);
            const bool inlineF = inlineCanRemoveSlot(0, size);
            const bool pin     = pinCanRemoveSlot(0, size);
            ok                 = expect(got && got == inlineF && got == pin, "pole0 free == inline == pin") && ok;
        }
        // slot == size
        {
            const bool got     = CanRemoveSlot(size, size);
            const bool inlineF = inlineCanRemoveSlot(size, size);
            const bool pin     = pinCanRemoveSlot(size, size);
            ok                 = expect(got && got == inlineF && got == pin, "pole size free == inline == pin") && ok;
        }
        // slot == size+1 (safe when size < 255)
        if (size < 255)
        {
            const std::uint8_t slot    = static_cast<std::uint8_t>(size + 1);
            const bool         got     = CanRemoveSlot(slot, size);
            const bool         inlineF = inlineCanRemoveSlot(slot, size);
            const bool         pin     = pinCanRemoveSlot(slot, size);
            ok = expect(!got && got == inlineF && got == pin, "pole size+1 free == inline == pin") && ok;
        }
    }

    // Dense compose over small size domain free == inline == pin.
    for (std::uint8_t size = 0; size <= 8; ++size)
    {
        for (std::uint8_t slotID = 0; slotID <= size + 2; ++slotID)
        {
            const bool got     = CanRemoveSlot(slotID, size);
            const bool inlineF = inlineCanRemoveSlot(slotID, size);
            const bool pin     = pinCanRemoveSlot(slotID, size);
            const bool want    = slotID <= size;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Edge poles at MAX_CONTAINER_SIZE (120) and uint8 extremes.
    const std::uint8_t sizeEdges[] = { 0, 1, 3, 5, 80, 119, 120, 121, 254, 255 };
    const std::uint8_t slotEdges[] = { 0, 1, 3, 5, 80, 119, 120, 121, 254, 255 };
    for (const std::uint8_t size : sizeEdges)
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got     = CanRemoveSlot(slotID, size);
            const bool inlineF = inlineCanRemoveSlot(slotID, size);
            const bool pin     = pinCanRemoveSlot(slotID, size);
            const bool want    = slotID <= size;
            ok                 = expect(got == want, "edge free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "edge free == inline == pin") && ok;
        }
    }

    // Host-style inject poles: free == inline == pin for RemoveItem range.
    for (const auto& pair : {
             std::pair<std::uint8_t, std::uint8_t>{ 0, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 3, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 4, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 5, 5 },
             std::pair<std::uint8_t, std::uint8_t>{ 6, 5 },
             std::pair<std::uint8_t, std::uint8_t>{ 120, 120 },
             std::pair<std::uint8_t, std::uint8_t>{ 121, 120 },
             std::pair<std::uint8_t, std::uint8_t>{ 255, 255 },
             std::pair<std::uint8_t, std::uint8_t>{ 1, 0 },
         })
    {
        const std::uint8_t slotID  = pair.first;
        const std::uint8_t size    = pair.second;
        const bool         got     = CanRemoveSlot(slotID, size);
        const bool         inlineF = inlineCanRemoveSlot(slotID, size);
        const bool         pin     = pinCanRemoveSlot(slotID, size);
        const bool         inject  = slotID <= size;
        ok                         = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                         = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production RemoveItem path semantics:
    //   in range (slotID <= size) → proceed to release / move unique_ptr
    //   past size → nullptr
    ok = expect(CanRemoveSlot(0, 3), "RemoveItem slot 0 → accept path") && ok;
    ok = expect(CanRemoveSlot(3, 3), "RemoveItem boundary → accept path") && ok;
    ok = expect(!CanRemoveSlot(4, 3), "RemoveItem past size → reject") && ok;
    ok = expect(CanRemoveSlot(120, 120), "RemoveItem max boundary → accept") && ok;
    ok = expect(!CanRemoveSlot(121, 120), "RemoveItem past max → reject") && ok;

    return ok;
}
