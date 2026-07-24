#pragma once

#include <chrono>

namespace mobcontrollerdeaggrorespawn
{

inline constexpr auto delay() -> std::chrono::seconds
{
    return std::chrono::seconds{ 60 };
}

} // namespace mobcontrollerdeaggrorespawn
