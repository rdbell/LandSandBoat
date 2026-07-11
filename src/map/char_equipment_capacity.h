#pragma once

#include "common/cbasetypes.h"

#include <array>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace charequipmenthelpers
{

template <std::size_t Count>
constexpr auto IsValidSlot(const uint8 slot) -> bool
{
    return slot < Count;
}

template <typename Item, std::size_t Count>
inline auto Get(const std::array<Item*, Count>& slots, const uint8 slot) -> Item*
{
    return IsValidSlot<Count>(slot) ? slots[slot] : nullptr;
}

template <typename Item, std::size_t Count, typename LocationOf>
inline auto Location(const std::array<Item*, Count>& slots, const uint8 slot, LocationOf&& locationOf)
    -> std::optional<std::invoke_result_t<LocationOf, Item*>>
{
    auto* item = Get(slots, slot);
    if (item == nullptr)
    {
        return std::nullopt;
    }
    return std::invoke(std::forward<LocationOf>(locationOf), item);
}

template <typename Item, std::size_t Count, typename MarkFree>
inline void Clear(std::array<Item*, Count>& slots, const uint8 slot, MarkFree&& markFree)
{
    if (!IsValidSlot<Count>(slot))
    {
        return;
    }

    if (auto* item = slots[slot]; item != nullptr)
    {
        if (!std::invoke(std::forward<MarkFree>(markFree), item))
        {
            return;
        }
        slots[slot] = nullptr;
    }
}

template <typename Item, std::size_t Count, typename MarkEquipped, typename MarkFree>
inline auto Bind(
    std::array<Item*, Count>& slots,
    const uint8              slot,
    Item*                    item,
    MarkEquipped&&           markEquipped,
    MarkFree&&               markFree) -> bool
{
    if (!IsValidSlot<Count>(slot) || item == nullptr)
    {
        return false;
    }
    if (!std::invoke(std::forward<MarkEquipped>(markEquipped), item))
    {
        return false;
    }

    Clear(slots, slot, std::forward<MarkFree>(markFree));
    slots[slot] = item;
    return true;
}

} // namespace charequipmenthelpers
