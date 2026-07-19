#pragma once

namespace trustcontrollerability
{

inline auto CanUse(bool hasRecast, bool canChangeState) -> bool
{
    return !hasRecast && canChangeState;
}

template <typename CanChangeState>
constexpr auto CanStart(const bool hasRecast, CanChangeState&& canChangeState) -> bool
{
    return !hasRecast && canChangeState();
}

} // namespace trustcontrollerability
