#pragma once

namespace automatoncontrollerspiritreaverdia
{
inline auto CanPrioritize(bool bioMissing, uint8 lightManeuvers) -> bool
{
    return bioMissing && lightManeuvers >= 2;
}
} // namespace automatoncontrollerspiritreaverdia
