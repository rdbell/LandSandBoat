#pragma once

namespace mobcontrollerroamrestgate
{
inline auto CanRest(bool intervalElapsed, bool noRestModifier, bool canRest) -> bool
{
    return intervalElapsed && !noRestModifier && canRest;
}
} // namespace mobcontrollerroamrestgate
