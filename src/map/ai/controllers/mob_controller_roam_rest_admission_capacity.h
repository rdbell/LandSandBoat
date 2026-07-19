#pragma once

#include <chrono>

#include "mob_controller_roam_rest_gate_capacity.h"

namespace mobcontrollerroamrestadmission
{
// CanRest reports whether the current roam tick may perform a recovery rest.
constexpr auto CanRest(
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point lastHeal,
    const bool noRest,
    const bool entityCanRest) -> bool
{
    return mobcontrollerroamrestgate::CanRest(
        tick >= lastHeal + std::chrono::seconds(10), noRest, entityCanRest);
}
} // namespace mobcontrollerroamrestadmission
