#pragma once

namespace mobcontrollerspellcastroute
{
enum class Route
{
    FindTarget,
    Direct,
};

// Select mirrors CMobController::TryCastSpell's final dispatch routing.
constexpr auto Select(const bool hasTargetOverride, const bool hasCastTarget) -> Route
{
    return hasTargetOverride && hasCastTarget ? Route::Direct : Route::FindTarget;
}
} // namespace mobcontrollerspellcastroute
