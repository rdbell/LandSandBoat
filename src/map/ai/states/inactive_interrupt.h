#pragma once

namespace inactiveinterrupt
{
constexpr auto shouldInterrupt(const bool canChangeState) -> bool
{
    return !canChangeState;
}
} // namespace inactiveinterrupt
