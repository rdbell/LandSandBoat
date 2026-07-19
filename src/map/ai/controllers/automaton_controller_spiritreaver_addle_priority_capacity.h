#pragma once

namespace automatoncontrollerspiritreaveraddle
{
inline auto CanPrioritize(uint8 fireManeuvers) -> bool
{
    return fireManeuvers >= 2;
}
} // namespace automatoncontrollerspiritreaveraddle
