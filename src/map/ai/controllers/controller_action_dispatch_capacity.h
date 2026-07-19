#pragma once

namespace controlleractiondispatch
{
// Dispatch runs an owner action only when the controller has an owner.
template <typename Action>
constexpr auto Dispatch(const bool hasOwner, Action&& action) -> bool
{
    return hasOwner && action();
}
} // namespace controlleractiondispatch
