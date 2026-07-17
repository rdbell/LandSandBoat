#include "test_item_increment_count_insert_3021.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer ShouldIncrementCountOnInsertAt 3021 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertItem count-bump gate for dual-wire cross-check (slice 3021):
//   slotEmpty && slotID != 0
auto inlineShouldIncrementCountOnInsertAt(const bool slotEmpty, const std::uint8_t slotID) -> bool
{
    return slotEmpty && slotID != 0;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::ShouldIncrementCountOnInsertAt
// (InsertItem count bump; slice 3021).
auto runItemIncrementCountInsert3021SelfTests() -> bool
{
    using itemcontainerhelpers::ShouldDecrementCountOnRemove;
    using itemcontainerhelpers::ShouldIncrementCountOnInsertAt;

    bool ok = true;

    // Residual 2802 pins still hold under dual-wire.
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "residual inc empty nonzero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "residual no inc occupied") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "residual no inc empty zero") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "residual no inc occupied zero") && ok;

    const struct
    {
        bool         slotEmpty;
        std::uint8_t slotID;
        bool         want;
        const char*  label;
    } cases[] = {
        // Classic accept pole — empty nonzero.
        { true, 1, true, "inc empty slot 1" },
        { true, 5, true, "inc empty slot 5" },
        { true, 255, true, "inc empty max uint8" },

        // Residual 2802 poles.
        { true, 1, true, "residual empty nonzero" },
        { false, 1, false, "residual occupied nonzero" },
        { true, 0, false, "residual empty zero" },
        { false, 0, false, "residual occupied zero" },

        // Empty but slot 0 never bumps count.
        { true, 0, false, "empty slot 0 no inc" },

        // Occupied never bumps count regardless of slotID.
        { false, 0, false, "occupied slot 0 no inc" },
        { false, 1, false, "occupied slot 1 no inc" },
        { false, 120, false, "occupied max-container slot no inc" },
        { false, 255, false, "occupied max uint8 no inc" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldIncrementCountOnInsertAt(c.slotEmpty, c.slotID);
        const bool inlineF = inlineShouldIncrementCountOnInsertAt(c.slotEmpty, c.slotID);
        const bool wantPin = c.slotEmpty && c.slotID != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldIncrementCountOnInsertAt dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldIncrementCountOnInsertAt == pin formula empty && slotID != 0") && ok;
    }

    // Pin composition: only empty && slotID != 0.
    ok = expect(ShouldIncrementCountOnInsertAt(true, 1), "empty nonzero must inc") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 1), "occupied must not inc") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(true, 0), "empty zero must not inc") && ok;
    ok = expect(!ShouldIncrementCountOnInsertAt(false, 0), "occupied zero must not inc") && ok;

    // Dense compose: empty × slot edges free == inline == pin.
    const std::uint8_t slotEdges[] = { 0, 1, 2, 3, 5, 120, 121, 254, 255 };
    for (const bool empty : { false, true })
    {
        for (const std::uint8_t slotID : slotEdges)
        {
            const bool got  = ShouldIncrementCountOnInsertAt(empty, slotID);
            const bool want = empty && slotID != 0;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldIncrementCountOnInsertAt(empty, slotID),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Mirror decrement-count gate shape:
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

    // Host-style inject poles: emptiness / SlotID as InsertItem injects.
    // (Live CItemContainer InsertItem ownership move is residual 2802 /
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
        const bool         empty  = pair.first;
        const std::uint8_t slotID = pair.second;
        // mirrors (m_ItemList[SlotID] == nullptr) && SlotID != 0 host inject
        const bool inject = empty && slotID != 0;
        ok                = expect(ShouldIncrementCountOnInsertAt(empty, slotID) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldIncrementCountOnInsertAt(empty, slotID) ==
                        inlineShouldIncrementCountOnInsertAt(empty, slotID),
                    "host inject free == inline") &&
             ok;
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
