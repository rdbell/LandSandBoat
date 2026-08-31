#pragma once

#include <chrono>

namespace playercontrollercanact
{
// Spells, job abilities, ranged attacks, and items are blocked for 2.5s after
// a spell finishes. Engagement is intentionally outside this gate.
template <typename TimePoint>
inline auto CanAct(const TimePoint now, const TimePoint lastSpellFinished) -> bool
{
    return now > lastSpellFinished + std::chrono::milliseconds(2500);
}
} // namespace playercontrollercanact
