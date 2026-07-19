#pragma once

namespace mobcontrollerroamrestgate
{
constexpr auto CanRest(bool intervalElapsed, bool noRestModifier, bool canRest) -> bool
{
    return intervalElapsed && !noRestModifier && canRest;
}
} // namespace mobcontrollerroamrestgate
