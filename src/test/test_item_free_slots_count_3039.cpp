#include "test_item_free_slots_count_3039.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer FreeSlotsCount 3039 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline GetFreeSlotsCount formula for dual-wire cross-check (slice 3039):
//   size - count  (uint8 wrap underflow when count > size)
auto inlineFreeSlotsCount(const std::uint8_t size, const std::uint8_t count) -> std::uint8_t
{
    return size - count;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::FreeSlotsCount
// (GetFreeSlotsCount size-minus-count; slice 3039).
auto runItemFreeSlotsCount3039SelfTests() -> bool
{
    using itemcontainerhelpers::FreeSlotsCount;
    using itemcontainerhelpers::MoveItemToDisposition;
    using itemcontainerhelpers::PlanMoveItemTo;

    bool ok = true;

    // Residual 2826 pins still hold under dual-wire.
    ok = expect(FreeSlotsCount(0, 0) == 0, "residual: empty zero") && ok;
    ok = expect(FreeSlotsCount(10, 0) == 10, "residual: full free") && ok;
    ok = expect(FreeSlotsCount(10, 3) == 7, "residual: partial") && ok;
    ok = expect(FreeSlotsCount(10, 10) == 0, "residual: full occupied") && ok;
    ok = expect(FreeSlotsCount(80, 80) == 0, "residual: max inventory full") && ok;
    ok = expect(FreeSlotsCount(120, 1) == 119, "residual: max size nearly empty") && ok;
    ok = expect(FreeSlotsCount(0, 1) == 255, "residual: wrap count one over zero") && ok;
    ok = expect(FreeSlotsCount(5, 6) == 255, "residual: wrap count one over size") && ok;
    ok = expect(FreeSlotsCount(0, 255) == 1, "residual: wrap max count") && ok;
    ok = expect(FreeSlotsCount(1, 255) == 2, "residual: wrap large count over one") && ok;
    ok = expect(FreeSlotsCount(255, 0) == 255, "residual: size max free") && ok;
    ok = expect(FreeSlotsCount(255, 255) == 0, "residual: size max full") && ok;

    const struct
    {
        std::uint8_t size;
        std::uint8_t count;
        std::uint8_t want;
        const char*  label;
    } cases[] = {
        // Residual 2826 poles.
        { 0, 0, 0, "residual empty zero" },
        { 10, 0, 10, "residual full free" },
        { 10, 3, 7, "residual partial" },
        { 10, 10, 0, "residual full occupied" },
        { 80, 80, 0, "residual max inventory full" },
        { 120, 1, 119, "residual max size nearly empty" },
        { 0, 1, 255, "residual wrap count one over zero" },
        { 5, 6, 255, "residual wrap count one over size" },
        { 0, 255, 1, "residual wrap max count" },
        { 1, 255, 2, "residual wrap large count over one" },
        { 255, 0, 255, "residual size max free" },
        { 255, 255, 0, "residual size max full" },

        // Edge: zeros.
        { 0, 0, 0, "zeros size==count" },
        { 0, 1, 255, "zeros size wrap" },
        { 0, 255, 1, "zeros size max count" },
        { 1, 0, 1, "count zero size one" },
        { 10, 0, 10, "count zero mid size" },
        { 120, 0, 120, "count zero max container" },
        { 255, 0, 255, "count zero max uint8" },

        // Edge: size == count (full; free = 0).
        { 0, 0, 0, "full zeros" },
        { 1, 1, 0, "full one" },
        { 10, 10, 0, "full mid" },
        { 80, 80, 0, "full inventory" },
        { 120, 120, 0, "full max container" },
        { 255, 255, 0, "full max uint8" },

        // Edge: max uint8 poles.
        { 255, 0, 255, "max size empty" },
        { 255, 1, 254, "max size one occupied" },
        { 255, 254, 1, "max size nearly full" },
        { 255, 255, 0, "max size full" },
        { 254, 255, 255, "wrap size one under max count" },
        { 128, 255, 129, "wrap mid size max count" },

        // Edge: wrap underflow (count > size).
        { 0, 1, 255, "wrap underflow zero/one" },
        { 0, 2, 254, "wrap underflow zero/two" },
        { 1, 2, 255, "wrap underflow one/two" },
        { 5, 6, 255, "wrap underflow by one" },
        { 5, 10, 251, "wrap underflow by five" },
        { 10, 255, 11, "wrap underflow size 10 max count" },
        { 100, 200, 156, "wrap underflow mid" },
        { 200, 255, 201, "wrap underflow large" },

        // Typical production sizes.
        { 80, 0, 80, "inventory empty" },
        { 80, 40, 40, "inventory half" },
        { 80, 79, 1, "inventory nearly full" },
        { 80, 80, 0, "inventory full" },
        { 120, 0, 120, "mogsafe empty" },
        { 120, 60, 60, "mogsafe half" },
        { 120, 120, 0, "mogsafe full" },
    };

    for (const auto& c : cases)
    {
        const auto got     = FreeSlotsCount(c.size, c.count);
        const auto inlineF = inlineFreeSlotsCount(c.size, c.count);
        const auto wantPin = static_cast<std::uint8_t>(c.size - c.count);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "FreeSlotsCount dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "FreeSlotsCount == pin formula size-count") && ok;
    }

    // Pin composition: only unsigned size - count.
    ok = expect(FreeSlotsCount(10, 3) == 7, "partial free must be size-count") && ok;
    ok = expect(FreeSlotsCount(0, 1) == 255, "underflow must wrap to 255") && ok;
    ok = expect(FreeSlotsCount(255, 0) == 255, "max free must be 255") && ok;
    ok = expect(FreeSlotsCount(0, 0) == 0, "zeros must be 0") && ok;

    // Dense compose: free == inline == pin on residual + edge grid.
    for (const std::uint8_t size : { std::uint8_t{ 0 }, std::uint8_t{ 1 }, std::uint8_t{ 5 }, std::uint8_t{ 10 },
                                     std::uint8_t{ 80 }, std::uint8_t{ 120 }, std::uint8_t{ 254 }, std::uint8_t{ 255 } })
    {
        for (const std::uint8_t count : { std::uint8_t{ 0 }, std::uint8_t{ 1 }, std::uint8_t{ 5 }, std::uint8_t{ 6 },
                                          std::uint8_t{ 10 }, std::uint8_t{ 80 }, std::uint8_t{ 120 }, std::uint8_t{ 254 },
                                          std::uint8_t{ 255 } })
        {
            const auto got  = FreeSlotsCount(size, count);
            const auto want = static_cast<std::uint8_t>(size - count);
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineFreeSlotsCount(size, count), "compose free == inline") && ok;
        }
    }

    // Host-style inject poles: FreeSlotsCount(m_size, m_count).
    // Production path: return FreeSlotsCount(m_size, m_count).
    for (const auto& pair : {
             std::pair<std::uint8_t, std::uint8_t>{ 0, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 1 },
             std::pair<std::uint8_t, std::uint8_t>{ 10, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 10, 10 },
             std::pair<std::uint8_t, std::uint8_t>{ 255, 0 },
             std::pair<std::uint8_t, std::uint8_t>{ 255, 255 },
             std::pair<std::uint8_t, std::uint8_t>{ 5, 6 },
             std::pair<std::uint8_t, std::uint8_t>{ 0, 255 },
         })
    {
        const auto size  = pair.first;
        const auto count = pair.second;
        const auto want  = static_cast<std::uint8_t>(size - count);
        ok               = expect(FreeSlotsCount(size, count) == want, "host inject dual-wire identity") && ok;
        ok               = expect(FreeSlotsCount(size, count) == inlineFreeSlotsCount(size, count),
                    "host inject free == inline") &&
             ok;
    }

    // Production GetFreeSlotsCount path semantics:
    // return FreeSlotsCount(m_size, m_count)  // uint8 wrap when count > size
    ok = expect(FreeSlotsCount(10, 3) == 7, "GetFreeSlotsCount partial → free path") && ok;
    ok = expect(FreeSlotsCount(10, 10) == 0, "GetFreeSlotsCount full → zero free") && ok;
    ok = expect(FreeSlotsCount(0, 1) == 255, "GetFreeSlotsCount wrap → underflow free") && ok;

    // Sibling move planner still injects free-slot scalar (does not re-impl).
    ok = expect(PlanMoveItemTo(false, 0, 10, false, FreeSlotsCount(10, 10)) == MoveItemToDisposition::RejectNoFreeSlots,
                "PlanMoveItemTo auto-dst uses FreeSlotsCount==0 reject") &&
         ok;
    ok = expect(PlanMoveItemTo(false, 0, 10, false, FreeSlotsCount(10, 0)) == MoveItemToDisposition::Allow,
                "PlanMoveItemTo auto-dst uses FreeSlotsCount>0 allow") &&
         ok;

    return ok;
}
