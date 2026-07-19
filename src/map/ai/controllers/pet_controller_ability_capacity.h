#pragma once

namespace petcontrollerability
{
inline auto ShouldDelegate(bool canChangeState) -> bool
{
    return canChangeState;
}
} // namespace petcontrollerability
