#include "test_item_search_slot_3033.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer CanSearchSlotID 3033 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline GetItem / search range gate for dual-wire cross-check (slice 3033):
//   slotID <= size
auto inlineCanSearchSlotID(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::CanSearchSlotID
// (GetItem / search inclusive range gate; slice 3033).
auto runItemSearchSlot3033SelfTests() -> bool
{
    using itemcontainerhelpers::CanInsertAtSlot;
    using itemcontainerhelpers::CanRemoveSlot;
    using itemcontainerhelpers::CanSearchSlotID;

    bool ok = true;

    const struct
    {
        std::uint8_t slotID;
        std::uint8_t size;
        bool         want;
        const char*  label;
    } cases[] = {
        // Classic dual poles.
        { 0, 0, true, "slot 0 size 0 accepted" },
        { 6, 5, false, "past size rejected" },

        // Range edges: 0, size, size+1.
        { 0, 5, true, "slot 0 under size" },
        { 5, 5, true, "at size boundary accepted" },
        { 6, 5, false, "size+1 rejected" },
        { 4, 5, true, "one under size accepted" },

        // Residual 2831 pins.
        { 0, 0, true, "residual search slot 0 size 0" },
        { 0, 5, true, "residual search slot 0 included" },
        { 5, 5, true, "residual search at size boundary" },
        { 6, 5, false, "residual search past size" },
        { 120, 120, true, "residual max container boundary" },
        { 121, 120, false, "residual past max container" },

        // size=0 edges.
        { 0, 0, true, "size 0 slot 0 accepted" },
        { 1, 0, false, "size 0 slot 1 rejected" },

        // size=255 (max uint8) edges.
        { 0, 255, true, "slot 0 size max" },
        { 255, 255, true, "both max uint8" },
        { 254, 255, true, "one under max size" },

        // Max container bounds (MAX_CONTAINER_SIZE = 120).
        { 120, 120, true, "max container boundary" },
        { 121, 120, false, "past max container" },
        { 255, 120, false, "uint8 high past size" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanSearchSlotID(c.slotID, c.size);
        const bool inlineF = inlineCanSearchSlotID(c.slotID, c.size);
        const bool wantPin = c.slotID <= c.size;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanSearchSlotID dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanSearchSlotID == pin formula slotID <= size") && ok;
    }

    // Pin composition: slotID <= size only.
    ok = expect(CanSearchSlotID(0, 0), "slot 0 size 0 must accept") && ok;
    ok = expect(CanSearchSlotID(5, 5), "slot == size must accept") && ok;
    ok = expect(!CanSearchSlotID(6, 5), "slot > size must reject") && ok;

    // Dense compose over small size domain (0..8 slots × slot candidates).
    for (std::uint8_t size = 0; size <= 8; ++size)
    {
        for (std::uint8_t slotID = 0; slotID <= size + 2; ++slotID)
        {
            const bool got  = CanSearchSlotID(slotID, size);
            const bool want = slotID <= size;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanSearchSlotID(slotID, size), "compose free == inline") && ok;
            // Parity with CanInsertAtSlot / CanRemoveSlot (same predicate).
            ok = expect(got == CanInsertAtSlot(slotID, size), "compose search == insert") && ok;
            ok = expect(got == CanRemoveSlot(slotID, size), "compose search == remove") && ok;
        }
    }

    // Host-style inject poles: SlotID / m_size as uint8 after SetSize.
    // (Live CItemContainer GetItem list lookup is residual 2831 /
    // item_container tests.)
    for (const auto& pair : {
             std::pair<std::uint8_t, std::uint8_t>{ 0, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 3, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 4, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 120, 120 },
             std::pair<std::uint8_t, std::uint8_t>{ 121, 120 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 255 },
             std::pair<std::uint8_t, std::uint8_t>{ 255, 255 },
             std::pair<std::uint8_t, std::uint8_t>{ 1, 0 },
         })
    {
        const std::uint8_t slotID = pair.first;
        const std::uint8_t size   = pair.second;
        const bool         inject = slotID <= size; // mirrors SlotID <= m_size host inject
        ok                        = expect(CanSearchSlotID(slotID, size) == inject, "host inject dual-wire identity") && ok;
        ok                        = expect(CanSearchSlotID(slotID, size) == inlineCanSearchSlotID(slotID, size),
                    "host inject free == inline") &&
             ok;
        ok = expect(CanSearchSlotID(slotID, size) == CanInsertAtSlot(slotID, size), "host inject search == insert") && ok;
        ok = expect(CanSearchSlotID(slotID, size) == CanRemoveSlot(slotID, size), "host inject search == remove") && ok;
    }

    // Production GetItem path semantics:
    // in range (slotID <= size) → proceed to return m_ItemList entry
    // past size → nullptr
    ok = expect(CanSearchSlotID(0, 3), "GetItem slot 0 → accept path") && ok;
    ok = expect(CanSearchSlotID(3, 3), "GetItem boundary → accept path") && ok;
    ok = expect(!CanSearchSlotID(4, 3), "GetItem past size → nullptr") && ok;
    ok = expect(CanSearchSlotID(120, 120), "GetItem max boundary → accept") && ok;
    ok = expect(!CanSearchSlotID(121, 120), "GetItem past max → nullptr") && ok;
    ok = expect(CanSearchSlotID(0, 0), "GetItem size 0 slot 0 → accept") && ok;
    ok = expect(!CanSearchSlotID(1, 0), "GetItem size 0 slot 1 → nullptr") && ok;
    ok = expect(CanSearchSlotID(255, 255), "GetItem both max uint8 → accept") && ok;

    return ok;
}
