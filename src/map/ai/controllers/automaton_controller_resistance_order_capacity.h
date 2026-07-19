#pragma once

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace automatoncontrollerresistanceorder
{
template <typename Element>
inline void Sort(std::vector<std::pair<Element, int16_t>>& resistances)
{
    std::stable_sort(resistances.begin(), resistances.end(), [](const auto& first, const auto& second)
    {
        return first.second < second.second;
    });
}
} // namespace automatoncontrollerresistanceorder
