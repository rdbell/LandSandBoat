#pragma once

#include <ranges>

namespace charhighestjobhelpers
{

template <typename Levels>
inline auto HighestLevel(const Levels& levels) -> std::ranges::range_value_t<Levels>
{
    std::ranges::range_value_t<Levels> highest{};
    for (const auto level : levels)
    {
        if (level > highest)
        {
            highest = level;
        }
    }
    return highest;
}

} // namespace charhighestjobhelpers
