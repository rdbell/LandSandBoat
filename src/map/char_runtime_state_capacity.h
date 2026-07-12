#pragma once

#include <cstdint>
#include <utility>

namespace charruntimestatehelpers
{

template <typename T>
inline auto Get(const T& value) -> T
{
    return value;
}

template <typename T>
inline void Set(T& target, T value)
{
    target = std::move(value);
}

inline bool InMogHouse(const std::uint16_t mogHouseID)
{
    return mogHouseID != 0;
}

} // namespace charruntimestatehelpers
