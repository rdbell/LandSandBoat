#pragma once

#include <functional>
#include <utility>

namespace charamanhosthelpers
{

template <typename OptionalContainer, typename Factory>
inline auto Get(OptionalContainer& container, Factory&& factory) -> decltype(*container)&
{
    if (!container)
    {
        container = std::invoke(std::forward<Factory>(factory));
    }
    return *container;
}

} // namespace charamanhosthelpers
