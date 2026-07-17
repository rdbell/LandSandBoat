#include "test_item_increment_count_insert_3432.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer ShouldIncrementCountOnInsertAt 3432 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertItem count-bump gate for dual-wire cross-check (dedicated 3432):
//   slotEmpty && slotID != 0
auto inlineShouldIncrementCountOnInsertAt(const bool slotEmpty, const std::uint8_t slotID) -> bool
{
    return slotEmpty && slotID != 0;
}

// Compact dual-wire pin matching Go pinShouldIncrementCountOnInsertAt3432 / C++ capacity:
//   slotEmpty && slotID != 0
auto pinShouldIncrementCountOnInsertAt(const bool slotEmpty, const std::uint8_t slotID) -> bool
{
    return slotEmpty && slotID != 0;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::ShouldIncrementCountOnInsertAt
// (InsertItem count bump; OmegaXI internal/itemcontainer;
// dedicated slice 3432 expand residual 3021; prior dedicated 3374 retained).
//
// Coverage:
//   - free == inline == pin == (slotEmpty && slotID != 0)
//   - residual 2802 / 3021 pins still hold
//   - prior 3374 dedicated poles still hold
//   - poles: empty nonzero, occupied, slot 0, max uint8
auto runItemIncrementCountInsert3432SelfTests() -> bool
{
    using itemcontainerhelpers::ShouldDecrementCountOnRemove;
    using itemcontainerhelpers::ShouldIncrementCountOnInsertAt;

    bool ok = true;

    // Residual 2802 / 3021 pins still hold under dual-wire.
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "residual inc empty nonzero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "residual no inc occupied") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "residual no inc empty zero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "residual no inc occupied zero") && ok;

    // Prior dedicated 3374 poles still hold.
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "prior 3374 inc empty nonzero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "prior 3374 no inc occupied") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "prior 3374 no inc empty zero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "prior 3374 no inc occupied zero") && ok;
    ok = expect(ShouldIncrementCountOnInsertAt(true, 255), "prior 3374 inc empty max") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 255), "prior 3374 no inc occupied max") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool         slotEmpty;
        std::uint8_t slotID;
        bool         want;
        const char*  label;
    } cases[] = {
        // Residual 2802 / 3021 poles.
        { true, 1, true, "residual empty nonzero" },
        { false, 1, false, "residual occupied nonzero" },
        { true, 0, false, "residual empty zero" },
        { false, 0, false, "residual occupied zero" },
        { true, 5, true, "residual empty slot 5" },
        { true, 255, true, "residual empty max uint8" },
        { false, 120, false, "residual occupied max-container slot" },
        { false, 255, false, "residual occupied max uint8" },

        // Prior dedicated 3374 poles still hold.
        { true, 1, true, "prior 3374 empty nonzero" },
        { false, 1, false, "prior 3374 occupied nonzero" },
        { true, 0, false, "prior 3374 empty zero" },
        { false, 0, false, "prior 3374 occupied zero" },
        { true, 120, true, "prior 3374 empty max-container slot" },
        { true, 255, true, "prior 3374 empty max uint8" },
        { false, 120, false, "prior 3374 occupied max-container slot" },
        { false, 255, false, "prior 3374 occupied max uint8" },

        // Classic accept pole — empty nonzero.
        { true, 1, true, "inc empty slot 1" },
        { true, 5, true, "inc empty slot 5" },
        { true, 120, true, "inc empty max-container slot" },
        { true, 255, true, "inc empty max uint8" },

        // Empty but slot 0 never bumps count.
        { true, 0, false, "empty slot 0 no inc" },

        // Occupied never bumps count regardless of slotID.
        { false, 0, false, "occupied slot 0 no inc" },
        { false, 1, false, "occupied slot 1 no inc" },
        { false, 3, false, "occupied slot 3 no inc" },
        { false, 120, false, "occupied max-container slot no inc" },
        { false, 255, false, "occupied max uint8 no inc" },

        // Pole: empty × nonzero slots accept.
        { true, 2, true, "pole empty slot 2" },
        { true, 3, true, "pole empty slot 3" },
        { true, 121, true, "pole empty past max container id" },
        { true, 254, true, "pole empty near max uint8" },

        // Pole: empty × zero reject.
        { true, 0, false, "pole empty zero" },

        // Pole: occupied × any reject.
        { false, 2, false, "pole occupied slot 2" },
        { false, 254, false, "pole occupied near max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldIncrementCountOnInsertAt(c.slotEmpty, c.slotID);
        const bool inlineF = inlineShouldIncrementCountOnInsertAt(c.slotEmpty, c.slotID);
        const bool pin     = pinShouldIncrementCountOnInsertAt(c.slotEmpty, c.slotID);
        const bool wantPin = c.slotEmpty && c.slotID != 0;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula empty && slotID != 0") && ok;
    }

    // Explicit poles free == inline == pin for empty/occupied × slot edges.
    const std::uint8_t slotEdges[] = { 0, 1, 2, 3, 5, 120, 121, 254, 255 };
    for (const bool empty : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got     = ShouldIncrementCountOnInsertAt(empty, slotID);
            const bool inlineF = inlineShouldIncrementCountOnInsertAt(empty, slotID);
            const bool pin     = pinShouldIncrementCountOnInsertAt(empty, slotID);
            const bool want    = empty && slotID != 0;
            ok                 = expect(got == want, "pole free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
        }
    }

    // Dense compose: empty × slot edges free == inline == pin.
    for (const bool empty : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got     = ShouldIncrementCountOnInsertAt(empty, slotID);
            const bool inlineF = inlineShouldIncrementCountOnInsertAt(empty, slotID);
            const bool pin     = pinShouldIncrementCountOnInsertAt(empty, slotID);
            const bool want    = empty && slotID != 0;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host-style inject poles: free == inline == pin for InsertItem count bump.
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
        const bool         empty  = pair.first;
        const std::uint8_t slotID = pair.second;
        const bool         got    = ShouldIncrementCountOnInsertAt(empty, slotID);
        const bool         inlineF = inlineShouldIncrementCountOnInsertAt(empty, slotID);
        const bool         pin     = pinShouldIncrementCountOnInsertAt(empty, slotID);
        // mirrors (m_ItemList[SlotID] == nullptr) && SlotID != 0 host inject
        const bool inject = empty && slotID != 0;
        ok                = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Mirror residual decrement-count gate shape (3351 left residual):
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
        // free == inline == pin still holds for the inc pole.
        const bool got     = ShouldIncrementCountOnInsertAt(true, slotID);
        const bool inlineF = inlineShouldIncrementCountOnInsertAt(true, slotID);
        const bool pin     = pinShouldIncrementCountOnInsertAt(true, slotID);
        ok                 = expect(got == inlineF && got == pin, "inc/dec shape free == inline == pin") && ok;
    }

    // Production InsertItem path semantics (after CanInsertAtSlot admits):
    //   empty && slotID != 0 → m_count++
    //   else → leave m_count
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "InsertItem empty nonzero → count bump") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "InsertItem empty zero → no count bump") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "InsertItem occupied nonzero → no count bump") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "InsertItem occupied zero → no count bump") && ok;
    ok = expect(ShouldIncrementCountOnInsertAt(true, 255), "InsertItem empty max → count bump") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 255), "InsertItem occupied max → no count bump") && ok;

    return ok;
}
