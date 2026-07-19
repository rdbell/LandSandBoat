#pragma once

#include "common/timer.h"

namespace automatoncontrollertpskillchainresonancegate
{
inline auto CanUseResonance(bool hasStatus, const timer::time_point& startTime, const timer::time_point& now) -> bool
{
    return hasStatus && startTime + std::chrono::seconds(3) < now;
}
} // namespace automatoncontrollertpskillchainresonancegate
