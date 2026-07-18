#pragma once

namespace playercontrolleractiongate
{
enum class Error { None, Unable, WaitLonger };
struct Decision { bool dispatch; Error error; };

constexpr auto Cast(const bool canAct, const bool hasRecast, const bool untargetable) -> Decision
{
    if (!canAct || hasRecast) return { false, Error::Unable };
    if (untargetable) return { false, Error::None };
    return { true, Error::None };
}
constexpr auto Ranged(const bool canAct, const bool canChangeState, const bool untargetable) -> Decision
{
    if (!canAct || !canChangeState) return { false, Error::WaitLonger };
    if (untargetable) return { false, Error::None };
    return { true, Error::None };
}
constexpr auto Item(const bool canAct, const bool canChangeState, const bool untargetable) -> Decision
{
    if (!canAct || !canChangeState || untargetable) return { false, Error::None };
    return { true, Error::None };
}
} // namespace playercontrolleractiongate
