#pragma once

#include <chrono>
#include <cstdint>

namespace automatoncontrolleractiongate
{
template <typename TimeValue, typename Cooldown>
inline auto CanTakeAction(TimeValue now, TimeValue lastAction, Cooldown cooldown, int16_t decisionDelay) -> bool
{
    return now > lastAction + cooldown - std::chrono::milliseconds(decisionDelay * 10);
}
} // namespace automatoncontrolleractiongate
