#include "test_item_decrement_count_remove_2989.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer ShouldDecrementCountOnRemove 2989 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RemoveItem count-drop gate for dual-wire cross-check (slice 2989):
//   slotOccupied && slotID != 0
auto inlineShouldDecrementCountOnRemove(const bool slotOccupied, const std::uint8_t slotID) -> bool
{
    return slotOccupied && slotID != 0;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::ShouldDecrementCountOnRemove
// (RemoveItem count drop; slice 2989).
auto runItemDecrementCountRemove2989SelfTests() -> bool
{
    using itemcontainerhelpers::ShouldDecrementCountOnRemove;
    using itemcontainerhelpers::ShouldIncrementCountOnInsertAt;

    bool ok = true;

    // Residual 2802 pins still hold under dual-wire.
    ok = expect(ShouldDecrementCountOnRemove(true, 1), "residual dec occupied nonzero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 1), "residual no dec empty") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(true, 0), "residual no dec occupied zero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 0), "residual no dec empty zero") && ok;

    const struct
    {
        bool         slotOccupied;
        std::uint8_t slotID;
        bool         want;
        const char*  label;
    } cases[] = {
        // Classic accept pole — occupied nonzero.
        { true, 1, true, "dec occupied slot 1" },
        { true, 5, true, "dec occupied slot 5" },
        { true, 255, true, "dec occupied max uint8" },

        // Residual 2802 poles.
        { true, 1, true, "residual occupied nonzero" },
        { false, 1, false, "residual empty nonzero" },
        { true, 0, false, "residual occupied zero" },
        { false, 0, false, "residual empty zero" },

        // Occupied but slot 0 never drops count.
        { true, 0, false, "occupied slot 0 no dec" },

        // Empty never drops count regardless of slotID.
        { false, 0, false, "empty slot 0 no dec" },
        { false, 1, false, "empty slot 1 no dec" },
        { false, 120, false, "empty max-container slot no dec" },
        { false, 255, false, "empty max uint8 no dec" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDecrementCountOnRemove(c.slotOccupied, c.slotID);
        const bool inlineF = inlineShouldDecrementCountOnRemove(c.slotOccupied, c.slotID);
        const bool wantPin = c.slotOccupied && c.slotID != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDecrementCountOnRemove dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDecrementCountOnRemove == pin formula occupied && slotID != 0") && ok;
    }

    // Pin composition: only occupied && slotID != 0.
    ok = expect(ShouldDecrementCountOnRemove(true, 1), "occupied nonzero must dec") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 1), "empty must not dec") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(true, 0), "occupied zero must not dec") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 0), "empty zero must not dec") && ok;

    // Dense compose: occupied × slot edges free == inline == pin.
    const std::uint8_t slotEdges[] = { 0, 1, 2, 3, 5, 120, 121, 254, 255 };
    for (const bool occupied : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got  = ShouldDecrementCountOnRemove(occupied, slotID);
            const bool want = occupied && slotID != 0;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldDecrementCountOnRemove(occupied, slotID),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Mirror residual insert-count gate shape:
    // ShouldIncrementCountOnInsertAt(slotEmpty, slotID) = slotEmpty && slotID != 0
    // ShouldDecrementCountOnRemove(slotOccupied, slotID) = slotOccupied && slotID != 0
    for (const std::uint8_t slotID : slotEdges)
    {
        const bool inc = ShouldIncrementCountOnInsertAt(true, slotID);
        const bool dec = ShouldDecrementCountOnRemove(true, slotID);
        ok             = expect(inc == (slotID != 0), "inc nonzero-slot shape") && ok;
        ok             = expect(dec == (slotID != 0), "dec nonzero-slot shape") && ok;
        ok             = expect(!ShouldIncrementCountOnInsertAt(false, slotID), "inc false-pole reject") && ok;
        ok             = expect(!ShouldDecrementCountOnRemove(false, slotID), "dec false-pole reject") && ok;
    }

    // Host-style inject poles: occupancy / SlotID as RemoveItem injects.
    // (Live CItemContainer RemoveItem ownership move is residual 2802 /
    // item_container tests.)
    for (const auto& pair : {
             std::pair<bool, std::uint8_t>{ true, 0 },
             std::pair<bool, std::uint8_t>{ true, 1 },
             std::pair<bool, std::uint8_t>{ true, 3 },
             std::pair<bool, std::uint8_t>{ true, 120 },
             std::pair<bool, std::uint8_t>{ true, 255 },
             std::pair<bool, std::uint8_t>{ false, 0 },
             std::pair<bool, std::uint8_t>{ false, 1 },
             std::pair<bool, std::uint8_t>{ false, 3 },
             std::pair<bool, std::uint8_t>{ false, 120 },
             std::pair<bool, std::uint8_t>{ false, 255 },
         })
    {
        const bool         occupied = pair.first;
        const std::uint8_t slotID   = pair.second;
        // mirrors (m_ItemList[SlotID] != nullptr) && SlotID != 0 host inject
        const bool inject = occupied && slotID != 0;
        ok                = expect(ShouldDecrementCountOnRemove(occupied, slotID) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldDecrementCountOnRemove(occupied, slotID) ==
                        inlineShouldDecrementCountOnRemove(occupied, slotID),
                    "host inject free == inline") &&
             ok;
    }

    // Production RemoveItem path semantics (after CanRemoveSlot admits):
    //   occupied && slotID != 0 → m_count--
    //   else → leave m_count
    ok = expect(ShouldDecrementCountOnRemove(true, 1), "RemoveItem occupied nonzero → count drop") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(true, 0), "RemoveItem occupied zero → no count drop") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 1), "RemoveItem empty nonzero → no count drop") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 0), "RemoveItem empty zero → no count drop") && ok;
    ok = expect(ShouldDecrementCountOnRemove(true, 255), "RemoveItem occupied max → count drop") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 255), "RemoveItem empty max → no count drop") && ok;

    return ok;
}
