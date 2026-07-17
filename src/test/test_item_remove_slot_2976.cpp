#include "test_item_remove_slot_2976.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer CanRemoveSlot 2976 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RemoveItem range gate for dual-wire cross-check (slice 2976):
//   slotID <= size
auto inlineCanRemoveSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::CanRemoveSlot
// (RemoveItem range gate; slice 2976).
auto runItemRemoveSlot2976SelfTests() -> bool
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

        // Residual 2802 pins.
        { 0, 3, true, "residual remove slot 0" },
        { 3, 3, true, "residual remove at size boundary" },
        { 4, 3, false, "residual remove past size" },

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
        const bool got     = CanRemoveSlot(c.slotID, c.size);
        const bool inlineF = inlineCanRemoveSlot(c.slotID, c.size);
        const bool wantPin = c.slotID <= c.size;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRemoveSlot dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanRemoveSlot == pin formula slotID <= size") && ok;
    }

    // Pin composition: slotID <= size only.
    ok = expect(CanRemoveSlot(0, 0), "slot 0 size 0 must accept") && ok;
    ok = expect(CanRemoveSlot(5, 5), "slot == size must accept") && ok;
    ok = expect(!CanRemoveSlot(6, 5), "slot > size must reject") && ok;

    // Dense compose over small size domain (0..8 slots × slot candidates).
    for (std::uint8_t size = 0; size <= 8; ++size)
    {
        for (std::uint8_t slotID = 0; slotID <= size + 2; ++slotID)
        {
            const bool got  = CanRemoveSlot(slotID, size);
            const bool want = slotID <= size;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanRemoveSlot(slotID, size), "compose free == inline") && ok;
            // Parity with CanInsertAtSlot / CanSearchSlotID (same predicate).
            ok = expect(got == CanInsertAtSlot(slotID, size), "compose remove == insert") && ok;
            ok = expect(got == CanSearchSlotID(slotID, size), "compose remove == search") && ok;
        }
    }

    // Host-style inject poles: SlotID / m_size as uint8 after SetSize.
    // (Live CItemContainer RemoveItem ownership move is residual 2802 /
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
        ok                        = expect(CanRemoveSlot(slotID, size) == inject, "host inject dual-wire identity") && ok;
        ok                        = expect(CanRemoveSlot(slotID, size) == inlineCanRemoveSlot(slotID, size),
                    "host inject free == inline") &&
             ok;
        ok = expect(CanRemoveSlot(slotID, size) == CanInsertAtSlot(slotID, size), "host inject remove == insert") && ok;
    }

    // Production RemoveItem path semantics:
    // in range (slotID <= size) → proceed to drop count / move unique_ptr
    // past size → nullptr
    ok = expect(CanRemoveSlot(0, 3), "RemoveItem slot 0 → accept path") && ok;
    ok = expect(CanRemoveSlot(3, 3), "RemoveItem boundary → accept path") && ok;
    ok = expect(!CanRemoveSlot(4, 3), "RemoveItem past size → nullptr") && ok;
    ok = expect(CanRemoveSlot(120, 120), "RemoveItem max boundary → accept") && ok;
    ok = expect(!CanRemoveSlot(121, 120), "RemoveItem past max → nullptr") && ok;
    ok = expect(CanRemoveSlot(0, 0), "RemoveItem size 0 slot 0 → accept") && ok;
    ok = expect(!CanRemoveSlot(1, 0), "RemoveItem size 0 slot 1 → nullptr") && ok;
    ok = expect(CanRemoveSlot(255, 255), "RemoveItem both max uint8 → accept") && ok;

    return ok;
}
