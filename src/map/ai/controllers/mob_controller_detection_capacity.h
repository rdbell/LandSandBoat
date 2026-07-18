#pragma once

#include <chrono>

namespace mobcontrollerdetection
{
struct Decision
{
    bool tapDeaggro;
    bool shouldDeaggro;
};

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
