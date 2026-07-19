#pragma once

namespace mobcontrollermobskillownerdispatch
{
enum class Route
{
    None,
    Owner,
};

// Resolve returns the owner-delegation work for a mob skill request.
constexpr auto Resolve(const bool hasOwner) -> Route
{
    return hasOwner ? Route::Owner : Route::None;
}
} // namespace mobcontrollermobskillownerdispatch
