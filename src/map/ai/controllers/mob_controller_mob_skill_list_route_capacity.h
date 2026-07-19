#pragma once

namespace mobcontrollermobskilllistroute
{
enum class Route
{
    Reject,
    Explicit,
    Modifier,
};

// Resolve selects the source for a mob skill list request.
constexpr auto Resolve(const bool hasTarget, const int requestedListID) -> Route
{
    if (!hasTarget)
    {
        return Route::Reject;
    }
    return requestedListID != 0 ? Route::Explicit : Route::Modifier;
}
} // namespace mobcontrollermobskilllistroute
