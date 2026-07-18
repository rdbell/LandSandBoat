#pragma once

#include <chrono>

namespace playercontrollerengage
{
enum class Error { None, TooFar, WaitLonger };
struct Decision { bool dispatch; Error error; };

constexpr auto Evaluate(
    const bool targetValid, const float distance, const std::chrono::steady_clock::time_point lastAttack,
    const std::chrono::steady_clock::duration weaponDelay, const std::chrono::steady_clock::time_point now) -> Decision
{
    if (!targetValid)
    {
        return { false, Error::None };
    }
    if (distance >= 30)
    {
        return { false, Error::TooFar };
    }
    if (now <= lastAttack + weaponDelay)
    {
        return { false, Error::WaitLonger };
    }
    return { true, Error::None };
}
} // namespace playercontrollerengage
