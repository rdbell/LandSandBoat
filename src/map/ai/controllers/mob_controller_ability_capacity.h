#pragma once

namespace mobcontrollerability
{
// CanUse reports whether a mob controller may dispatch an ability action.
constexpr auto CanUse(const bool hasRecast, const bool canChangeState) -> bool
{
    return !hasRecast && canChangeState;
}
} // namespace mobcontrollerability
