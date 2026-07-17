#include "test_item_decrement_count_remove_3351.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer ShouldDecrementCountOnRemove 3351 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline RemoveItem count-drop gate for dual-wire cross-check (dedicated 3351):
//   slotOccupied && slotID != 0
auto inlineShouldDecrementCountOnRemove(const bool slotOccupied, const std::uint8_t slotID) -> bool
{
    return slotOccupied && slotID != 0;
}

// Compact dual-wire pin matching Go pinShouldDecrementCountOnRemove3351 / C++ capacity:
//   slotOccupied && slotID != 0
auto pinShouldDecrementCountOnRemove(const bool slotOccupied, const std::uint8_t slotID) -> bool
{
    return slotOccupied && slotID != 0;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::ShouldDecrementCountOnRemove
// (RemoveItem count drop; OmegaXI internal/itemcontainer;
// dedicated slice 3351 expand residual 2989).
//
// Coverage:
//   - free == inline == pin == (slotOccupied && slotID != 0)
//   - residual 2802 / 2989 pins still hold
//   - poles: occupied nonzero, empty, slot 0, max uint8
auto runItemDecrementCountRemove3351SelfTests() -> bool
{
    using itemcontainerhelpers::ShouldDecrementCountOnRemove;
    using itemcontainerhelpers::ShouldIncrementCountOnInsertAt;

    bool ok = true;

    // Residual 2802 / 2989 pins still hold under dual-wire.
    ok = expect(ShouldDecrementCountOnRemove(true, 1), "residual dec occupied nonzero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 1), "residual no dec empty") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(true, 0), "residual no dec occupied zero") && ok;
    ok = expect(!ShouldDecrementCountOnRemove(false, 0), "residual no dec empty zero") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool         slotOccupied;
        std::uint8_t slotID;
        bool         want;
        const char*  label;
    } cases[] = {
        // Residual 2802 / 2989 poles.
        { true, 1, true, "residual occupied nonzero" },
        { false, 1, false, "residual empty nonzero" },
        { true, 0, false, "residual occupied zero" },
        { false, 0, false, "residual empty zero" },
        { true, 5, true, "residual occupied slot 5" },
        { true, 255, true, "residual occupied max uint8" },
        { false, 120, false, "residual empty max-container slot" },
        { false, 255, false, "residual empty max uint8" },

        // Classic accept pole — occupied nonzero.
        { true, 1, true, "dec occupied slot 1" },
        { true, 5, true, "dec occupied slot 5" },
        { true, 120, true, "dec occupied max-container slot" },
        { true, 255, true, "dec occupied max uint8" },

        // Occupied but slot 0 never drops count.
        { true, 0, false, "occupied slot 0 no dec" },

        // Empty never drops count regardless of slotID.
        { false, 0, false, "empty slot 0 no dec" },
        { false, 1, false, "empty slot 1 no dec" },
        { false, 3, false, "empty slot 3 no dec" },
        { false, 120, false, "empty max-container slot no dec" },
        { false, 255, false, "empty max uint8 no dec" },

        // Pole: occupied × nonzero slots accept.
        { true, 2, true, "pole occupied slot 2" },
        { true, 3, true, "pole occupied slot 3" },
        { true, 121, true, "pole occupied past max container id" },
        { true, 254, true, "pole occupied near max uint8" },

        // Pole: occupied × zero reject.
        { true, 0, false, "pole occupied zero" },

        // Pole: empty × any reject.
        { false, 2, false, "pole empty slot 2" },
        { false, 254, false, "pole empty near max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDecrementCountOnRemove(c.slotOccupied, c.slotID);
        const bool inlineF = inlineShouldDecrementCountOnRemove(c.slotOccupied, c.slotID);
        const bool pin     = pinShouldDecrementCountOnRemove(c.slotOccupied, c.slotID);
        const bool wantPin = c.slotOccupied && c.slotID != 0;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula occupied && slotID != 0") && ok;
    }

    // Explicit poles free == inline == pin for occupied/empty × slot edges.
    const std::uint8_t slotEdges[] = { 0, 1, 2, 3, 5, 120, 121, 254, 255 };
    for (const bool occupied : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got     = ShouldDecrementCountOnRemove(occupied, slotID);
            const bool inlineF = inlineShouldDecrementCountOnRemove(occupied, slotID);
            const bool pin     = pinShouldDecrementCountOnRemove(occupied, slotID);
            const bool want    = occupied && slotID != 0;
            ok                 = expect(got == want, "pole free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
        }
    }

    // Dense compose: occupied × slot edges free == inline == pin.
    for (const bool occupied : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got     = ShouldDecrementCountOnRemove(occupied, slotID);
            const bool inlineF = inlineShouldDecrementCountOnRemove(occupied, slotID);
            const bool pin     = pinShouldDecrementCountOnRemove(occupied, slotID);
            const bool want    = occupied && slotID != 0;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host-style inject poles: free == inline == pin for RemoveItem count drop.
    for (const auto& pair : {
             std::pair<bool, std::uint8_t>{ true, 0 },
             std::pair<bool, std::uint8_t>{ true, 1 },
             std::pair<bool, std::uint8_t>{ true, 3 },
             std::pair<bool, std::uint8_t>{ true, 5 },
             std::pair<bool, std::uint8_t>{ true, 120 },
             std::pair<bool, std::uint8_t>{ true, 255 },
             std::pair<bool, std::uint8_t>{ false, 0 },
             std::pair<bool, std::uint8_t>{ false, 1 },
             std::pair<bool, std::uint8_t>{ false, 3 },
             std::pair<bool, std::uint8_t>{ false, 5 },
             std::pair<bool, std::uint8_t>{ false, 120 },
             std::pair<bool, std::uint8_t>{ false, 255 },
         })
    {
        const bool         occupied = pair.first;
        const std::uint8_t slotID   = pair.second;
        const bool         got      = ShouldDecrementCountOnRemove(occupied, slotID);
        const bool         inlineF  = inlineShouldDecrementCountOnRemove(occupied, slotID);
        const bool         pin      = pinShouldDecrementCountOnRemove(occupied, slotID);
        // mirrors (m_ItemList[SlotID] != nullptr) && SlotID != 0 host inject
        const bool inject = occupied && slotID != 0;
        ok                = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Mirror residual insert-count gate shape (3021 left residual):
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
        // free == inline == pin still holds for the dec pole.
        const bool got     = ShouldDecrementCountOnRemove(true, slotID);
        const bool inlineF = inlineShouldDecrementCountOnRemove(true, slotID);
        const bool pin     = pinShouldDecrementCountOnRemove(true, slotID);
        ok                 = expect(got == inlineF && got == pin, "inc/dec shape free == inline == pin") && ok;
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
