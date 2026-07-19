#pragma once

namespace petcontrollerstatechangeroam
{
inline auto ShouldHold(bool canChangeState) -> bool
{
    return !canChangeState;
}
} // namespace petcontrollerstatechangeroam
