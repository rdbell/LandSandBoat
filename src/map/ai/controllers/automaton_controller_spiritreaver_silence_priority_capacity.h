#pragma once

namespace automatoncontrollerspiritreaversilence
{
inline auto CanPrioritize(uint8 windManeuvers) -> bool
{
    return windManeuvers >= 2;
}
} // namespace automatoncontrollerspiritreaversilence
