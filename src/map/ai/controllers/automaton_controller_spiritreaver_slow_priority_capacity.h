#pragma once

namespace automatoncontrollerspiritreaverslow
{
inline auto CanPrioritize(uint8 earthManeuvers) -> bool
{
    return earthManeuvers >= 2;
}
} // namespace automatoncontrollerspiritreaverslow
