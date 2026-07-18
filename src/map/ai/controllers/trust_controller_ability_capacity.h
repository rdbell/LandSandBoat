#pragma once

namespace trustcontrollerability
{

inline auto CanUse(bool hasRecast, bool canChangeState) -> bool
{
    return !hasRecast && canChangeState;
}

} // namespace trustcontrollerability
