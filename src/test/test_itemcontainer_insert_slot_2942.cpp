#include "test_itemcontainer_insert_slot_2942.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer CanInsertAtSlot 2942 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertItem(PItem, SlotID) range gate for dual-wire cross-check (slice 2942):
//   slotID <= size
auto inlineCanInsertAtSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::CanInsertAtSlot
// (InsertItem(PItem, SlotID) range gate; slice 2942).
auto runItemcontainerInsertSlot2942SelfTests() -> bool
{
    using itemcontainerhelpers::CanInsertAtSlot;

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

        // Boundary neighbors around size.
        { 5, 5, true, "at size boundary accepted" },
        { 4, 5, true, "one under size accepted" },
        { 6, 5, false, "one over size rejected" },

        // Residual 2802 pins.
        { 0, 0, true, "residual insert slot 0 size 0" },
        { 5, 5, true, "residual insert at size boundary" },
        { 6, 5, false, "residual insert past size" },

        // Slot zero always addressable when size allows.
        { 0, 3, true, "slot 0 included" },
        { 0, 120, true, "slot 0 at max container" },

        // Max container bounds (MAX_CONTAINER_SIZE = 120).
        { 120, 120, true, "max container boundary" },
        { 121, 120, false, "past max container" },
        { 255, 120, false, "uint8 high past size" },
        { 255, 255, true, "both max uint8" },
        { 0, 255, true, "slot 0 size max" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanInsertAtSlot(c.slotID, c.size);
        const bool inlineF = inlineCanInsertAtSlot(c.slotID, c.size);
        const bool wantPin = c.slotID <= c.size;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanInsertAtSlot dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanInsertAtSlot == pin formula slotID <= size") && ok;
    }

    // Pin composition: slotID <= size only.
    ok = expect(CanInsertAtSlot(0, 0), "slot 0 size 0 must accept") && ok;
    ok = expect(CanInsertAtSlot(5, 5), "slot == size must accept") && ok;
    ok = expect(!CanInsertAtSlot(6, 5), "slot > size must reject") && ok;

    // Dense compose over small size domain (0..8 slots × slot candidates).
    for (std::uint8_t size = 0; size <= 8; ++size)
    {
        for (std::uint8_t slotID = 0; slotID <= size + 2; ++slotID)
        {
            const bool got  = CanInsertAtSlot(slotID, size);
            const bool want = slotID <= size;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanInsertAtSlot(slotID, size), "compose free == inline") && ok;
        }
    }

    // Host-style inject poles: SlotID / m_size as uint8 after SetSize.
    // (Live CItemContainer InsertItem ownership move is residual 2802 /
    // item_container tests.)
    for (const auto& pair : {
             std::pair<std::uint8_t, std::uint8_t>{ 0, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 3, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 4, 3 },
             std::pair<std::uint8_t, std::uint8_t>{ 120, 120 },
             std::pair<std::uint8_t, std::uint8_t>{ 121, 120 },
         })
    {
        const std::uint8_t slotID = pair.first;
        const std::uint8_t size   = pair.second;
        const bool         inject = slotID <= size; // mirrors SlotID <= m_size host inject
        ok                        = expect(CanInsertAtSlot(slotID, size) == inject, "host inject dual-wire identity") && ok;
        ok                        = expect(CanInsertAtSlot(slotID, size) == inlineCanInsertAtSlot(slotID, size),
                    "host inject free == inline") &&
             ok;
    }

    // Production InsertItem(PItem, SlotID) path semantics:
    // in range (slotID <= size) → proceed to set location / move unique_ptr
    // past size → ERROR_SLOTID
    ok = expect(CanInsertAtSlot(0, 3), "InsertItem slot 0 → accept path") && ok;
    ok = expect(CanInsertAtSlot(3, 3), "InsertItem boundary → accept path") && ok;
    ok = expect(!CanInsertAtSlot(4, 3), "InsertItem past size → ERROR_SLOTID") && ok;
    ok = expect(CanInsertAtSlot(120, 120), "InsertItem max boundary → accept") && ok;
    ok = expect(!CanInsertAtSlot(121, 120), "InsertItem past max → ERROR_SLOTID") && ok;

    return ok;
}
