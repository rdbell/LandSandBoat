#pragma once

#include <chrono>

namespace mobcontrollerdetection
{
struct Decision
{
    bool tapDeaggro;
    bool shouldDeaggro;
};

// CheckState records the source-ordered detection checks used before the
// deaggregation decision.
struct CheckState
{
    bool canPursue;
    bool canDetect;
    bool immobilized;
};

// ResolveChecks evaluates pursuit, detection, and immobilization in controller
// source order. Later checks run only after earlier checks fail.
template <typename CanPursueTarget, typename CanDetectTarget, typename IsImmobilized>
constexpr auto ResolveChecks(
    CanPursueTarget&& canPursueTarget,
    CanDetectTarget&& canDetectTarget,
    IsImmobilized&& isImmobilized) -> CheckState
{
    const bool canPursue = canPursueTarget();
    const bool canDetect = !canPursue && canDetectTarget();
    const bool immobilized = !canPursue && !canDetect && isImmobilized();
    return { canPursue, canDetect, immobilized };
}

constexpr auto Evaluate(
    const bool canPursue,
    const bool canDetect,
    const bool immobilized,
    const bool canDeaggro,
    const bool isWorm,
    const std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point deaggroTime,
    const std::chrono::steady_clock::duration additionalDelay) -> Decision
{
    const bool tapDeaggro = canPursue || canDetect || immobilized;
    const auto delay = std::chrono::seconds(25) + (isWorm ? std::chrono::steady_clock::duration(0) : additionalDelay);
    return { tapDeaggro, canDeaggro && tick >= deaggroTime + delay };
}
} // namespace mobcontrollerdetection
