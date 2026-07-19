#pragma once

namespace automatoncontrollerspiritreaverparalyze
{
inline auto CanPrioritize(uint8 iceManeuvers) -> bool
{
    return iceManeuvers >= 2;
}
} // namespace automatoncontrollerspiritreaverparalyze
