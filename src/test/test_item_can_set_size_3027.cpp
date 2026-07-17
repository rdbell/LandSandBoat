#include "test_item_can_set_size_3027.h"

#include "map/item_container_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "itemcontainer CanSetSize 3027 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SetSize / AddSize acceptance for dual-wire cross-check (slice 3027):
//   newSize <= maxSize && newSize >= itemCount
auto inlineCanSetSize(const std::uint8_t newSize, const std::uint8_t maxSize, const std::uint8_t itemCount) -> bool
{
    return newSize <= maxSize && newSize >= itemCount;
}

} // namespace

// Pure dual-wire expansion for itemcontainerhelpers::CanSetSize
// (SetSize / AddSize acceptance; slice 3027).
auto runItemCanSetSize3027SelfTests() -> bool
{
    using itemcontainerhelpers::CanSetSize;

    bool ok = true;

    // Residual 2802 pins still hold under dual-wire.
    ok = expect(CanSetSize(10, 120, 0), "residual set size empty") && ok;
    ok = expect(CanSetSize(10, 120, 10), "residual set size equal count") && ok;
    ok = expect(CanSetSize(120, 120, 50), "residual set size at max") && ok;
    ok = expect(!CanSetSize(121, 120, 0), "residual set size above max") && ok;
    ok = expect(!CanSetSize(5, 120, 6), "residual set size below count") && ok;
    ok = expect(CanSetSize(0, 120, 0), "residual set size zero empty") && ok;
    ok = expect(!CanSetSize(0, 120, 1), "residual set size zero with items") && ok;
    ok = expect(!CanSetSize(255, 120, 0), "residual set size wrapped 255") && ok;

    const struct
    {
        std::uint8_t newSize;
        std::uint8_t maxSize;
        std::uint8_t itemCount;
        bool         want;
        const char*  label;
    } cases[] = {
        // Residual 2802 poles.
        { 10, 120, 0, true, "residual empty" },
        { 10, 120, 10, true, "residual equal count" },
        { 120, 120, 50, true, "residual at max" },
        { 121, 120, 0, false, "residual above max" },
        { 5, 120, 6, false, "residual below count" },
        { 0, 120, 0, true, "residual zero empty" },
        { 0, 120, 1, false, "residual zero with items" },
        { 255, 120, 0, false, "residual wrapped 255" },

        // Edge: newSize == itemCount (accept when also <= max).
        { 0, 120, 0, true, "newSize==itemCount zero" },
        { 1, 120, 1, true, "newSize==itemCount one" },
        { 10, 120, 10, true, "newSize==itemCount mid" },
        { 120, 120, 120, true, "newSize==itemCount==max" },
        { 50, 50, 50, true, "newSize==itemCount==maxSize mid" },

        // Edge: newSize == maxSize (accept when also >= itemCount).
        { 120, 120, 0, true, "newSize==max empty" },
        { 120, 120, 50, true, "newSize==max partial" },
        { 120, 120, 120, true, "newSize==max full" },
        { 80, 80, 0, true, "newSize==maxSize 80 empty" },
        { 255, 255, 0, true, "newSize==maxSize max uint8" },
        { 255, 255, 255, true, "newSize==maxSize==itemCount max" },

        // Edge: newSize > maxSize (reject regardless of count).
        { 121, 120, 0, false, "newSize>max empty" },
        { 121, 120, 50, false, "newSize>max partial" },
        { 121, 120, 120, false, "newSize>max at count" },
        { 255, 120, 0, false, "newSize max uint8 > max" },
        { 2, 1, 0, false, "newSize>max small" },
        { 1, 0, 0, false, "newSize 1 > max 0" },

        // Edge: newSize < itemCount (reject regardless of max).
        { 5, 120, 6, false, "newSize<itemCount by one" },
        { 0, 120, 1, false, "newSize 0 < itemCount 1" },
        { 119, 120, 120, false, "newSize one under full count" },
        { 50, 255, 51, false, "newSize<itemCount wide max" },
        { 0, 0, 1, false, "newSize 0 max 0 < count 1" },

        // Zero poles.
        { 0, 0, 0, true, "all zeros" },
        { 0, 120, 0, true, "newSize zero empty" },
        { 0, 255, 0, true, "newSize zero max uint8" },
        { 0, 120, 1, false, "zero with items" },
        { 0, 0, 1, false, "zero max zero with items" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanSetSize(c.newSize, c.maxSize, c.itemCount);
        const bool inlineF = inlineCanSetSize(c.newSize, c.maxSize, c.itemCount);
        const bool wantPin = c.newSize <= c.maxSize && c.newSize >= c.itemCount;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanSetSize dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanSetSize == pin formula newSize <= max && newSize >= count") && ok;
    }

    // Pin composition: newSize <= maxSize && newSize >= itemCount only.
    ok = expect(CanSetSize(10, 120, 10), "newSize == itemCount must accept") && ok;
    ok = expect(CanSetSize(120, 120, 0), "newSize == maxSize must accept") && ok;
    ok = expect(!CanSetSize(121, 120, 0), "newSize > maxSize must reject") && ok;
    ok = expect(!CanSetSize(5, 120, 6), "newSize < itemCount must reject") && ok;
    ok = expect(CanSetSize(0, 0, 0), "all zeros must accept") && ok;

    // Dense compose over small domains free == inline == pin.
    for (std::uint8_t maxSize = 0; maxSize <= 8; ++maxSize)
    {
        for (std::uint8_t itemCount = 0; itemCount <= 8; ++itemCount)
        {
            for (std::uint8_t newSize = 0; newSize <= 10; ++newSize)
            {
                const bool got  = CanSetSize(newSize, maxSize, itemCount);
                const bool want = newSize <= maxSize && newSize >= itemCount;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineCanSetSize(newSize, maxSize, itemCount),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // Edge poles at MAX_CONTAINER_SIZE (120) and uint8 extremes.
    const std::uint8_t maxEdges[]   = { 0, 1, 80, 120, 121, 254, 255 };
    const std::uint8_t countEdges[] = { 0, 1, 50, 80, 120, 121, 255 };
    const std::uint8_t newEdges[]   = { 0, 1, 5, 50, 80, 119, 120, 121, 254, 255 };
    for (const std::uint8_t maxSize : maxEdges)
    {
        for (const std::uint8_t itemCount : countEdges)
        {
            for (const std::uint8_t newSize : newEdges)
            {
                const bool got  = CanSetSize(newSize, maxSize, itemCount);
                const bool want = newSize <= maxSize && newSize >= itemCount;
                ok              = expect(got == want, "edge free == pin formula") && ok;
                ok              = expect(got == inlineCanSetSize(newSize, maxSize, itemCount),
                            "edge free == inline") &&
                     ok;
            }
        }
    }

    // Host-style inject poles: size / MAX_CONTAINER_SIZE / m_count as
    // SetSize / AddSize inject. (Live CItemContainer SetSize ownership is
    // residual 2802 / item_container tests.)
    for (const auto& triple : {
             // new, max, count
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 0, 120, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 0, 120, 1 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 10, 120, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 10, 120, 10 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 10, 120, 11 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 120, 120, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 120, 120, 120 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 121, 120, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 255, 120, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 255, 255, 255 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 1, 0, 0 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 5, 5, 5 },
             std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>{ 5, 5, 6 },
         })
    {
        const std::uint8_t newSize   = std::get<0>(triple);
        const std::uint8_t maxSize   = std::get<1>(triple);
        const std::uint8_t itemCount = std::get<2>(triple);
        // mirrors size <= MAX_CONTAINER_SIZE && size >= m_count host inject
        const bool inject = newSize <= maxSize && newSize >= itemCount;
        ok                = expect(CanSetSize(newSize, maxSize, itemCount) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(CanSetSize(newSize, maxSize, itemCount) ==
                        inlineCanSetSize(newSize, maxSize, itemCount),
                    "host inject free == inline") &&
             ok;
    }

    // Production SetSize / AddSize path semantics:
    //   newSize <= MAX_CONTAINER_SIZE && newSize >= m_count → assign m_size
    //   else → return -1, leave m_size
    ok = expect(CanSetSize(10, 120, 0), "SetSize empty mid → accept") && ok;
    ok = expect(CanSetSize(10, 120, 10), "SetSize equal count → accept") && ok;
    ok = expect(CanSetSize(120, 120, 0), "SetSize at max → accept") && ok;
    ok = expect(!CanSetSize(121, 120, 0), "SetSize above max → reject") && ok;
    ok = expect(!CanSetSize(5, 120, 6), "SetSize below count → reject") && ok;
    ok = expect(!CanSetSize(255, 120, 0), "AddSize wrap 255 → reject") && ok;
    ok = expect(CanSetSize(0, 120, 0), "SetSize zero empty → accept") && ok;
    ok = expect(!CanSetSize(0, 120, 1), "SetSize zero with items → reject") && ok;

    return ok;
}
