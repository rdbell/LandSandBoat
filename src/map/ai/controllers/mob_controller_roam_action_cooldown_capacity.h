#pragma once

#include <chrono>

namespace mobcontrollerroamactioncooldown
{
// IsReady reports whether an idle mob may take its next roam action.
constexpr auto IsReady(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point lastAction,
    const std::chrono::seconds roamCooldown) -> bool
{
    return tick >= lastAction + roamCooldown;
}
} // namespace mobcontrollerroamactioncooldown
