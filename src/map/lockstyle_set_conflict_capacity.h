#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

// Pure sequential conflict removal from GP_CLI_COMMAND_LOCKSTYLE::process Set mode.

namespace lockstylesetconflicthelpers
{

constexpr std::size_t StyleSlotCount    = 16;
constexpr std::size_t ScannedStyleSlots = 10;

struct Item
{
    std::uint16_t itemID      = 0;
    bool          found       = false;
    std::uint16_t removeSlots = 0;
};

struct Plan
{
    std::array<std::uint16_t, StyleSlotCount> styleItems{};
};

constexpr auto FindItem(const std::uint16_t itemID, const std::span<const Item> items) -> const Item*
{
    for (const auto& item : items)
    {
        if (item.itemID == itemID && item.found)
        {
            return &item;
        }
    }

    return nullptr;
}

constexpr auto PlanFor(std::array<std::uint16_t, StyleSlotCount> styleItems, const std::span<const Item> items) -> Plan
{
    for (std::size_t index = 0; index < ScannedStyleSlots; ++index)
    {
        const auto* item = FindItem(styleItems[index], items);
        if (item == nullptr)
        {
            continue;
        }

        for (std::size_t target = 0; target < StyleSlotCount; ++target)
        {
            if (item->removeSlots & (std::uint16_t{ 1 } << target))
            {
                styleItems[target] = 0;
            }
        }
    }

    return { styleItems };
}

} // namespace lockstylesetconflicthelpers
