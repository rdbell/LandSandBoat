#pragma once

#include <array>
#include <cstdint>

namespace charmoghancementfurniturehelpers
{

struct Furniture
{
    bool          installed;
    bool          secondFloor;
    std::uint8_t  element;
    std::uint8_t  aura;
    std::uint8_t  order;
    std::uint16_t moghancement;
};

template <typename FurnitureRange>
inline std::uint16_t Select(const FurnitureRange& furniture)
{
    std::array<std::uint16_t, 8> elements{};
    for (const auto& item : furniture)
    {
        if (item.installed && !item.secondFloor)
        {
            elements[item.element - 1] += item.aura;
        }
    }

    std::uint8_t  dominantElement = 0;
    std::uint16_t dominantAura    = 0;
    bool          hasTiedElements = false;
    for (std::uint8_t elementID = 1; elementID < 9; ++elementID)
    {
        const auto aura = elements[elementID - 1];
        if (aura > dominantAura)
        {
            dominantElement = elementID;
            dominantAura    = aura;
            hasTiedElements = false;
        }
        else if (aura == dominantAura)
        {
            hasTiedElements = true;
        }
    }

    std::uint8_t  bestAura          = 0;
    std::uint8_t  bestOrder         = 255;
    std::uint16_t newMoghancementID = 0;
    if (!hasTiedElements && dominantAura > 0)
    {
        for (const auto& item : furniture)
        {
            if (item.installed && !item.secondFloor && item.element == dominantElement &&
                (item.aura > bestAura || (item.aura == bestAura && item.order < bestOrder)))
            {
                bestAura          = item.aura;
                bestOrder         = item.order;
                newMoghancementID = item.moghancement;
            }
        }
    }

    return newMoghancementID;
}

} // namespace charmoghancementfurniturehelpers
