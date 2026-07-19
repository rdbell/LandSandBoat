#pragma once

#include <chrono>

namespace mobcontrollerwait
{
struct State
{
    std::chrono::steady_clock::time_point tick;
    std::chrono::steady_clock::time_point waitUntil;
};

constexpr auto Apply(
    std::chrono::steady_clock::time_point tick,
    const std::chrono::steady_clock::time_point waitUntil,
    const std::chrono::steady_clock::duration delay) -> State
{
    if (tick > waitUntil)
    {
        tick += delay;
        return { tick, tick };
    }
    return { tick, waitUntil + delay };
}
} // namespace mobcontrollerwait
