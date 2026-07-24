#pragma once

// Pure CAIActionQueue::isEmpty inject (slice 6331).
// Dual-wire of Go actionqueue.IsBothQueuesEmpty
// (internal/actionqueue/is_empty.go).

namespace actionqueuehelpers
{

// IsBothQueuesEmpty reports whether both the state-gated action queue and
// the timer queue are empty.
// Mirrors: return actionQueue.empty() && timerQueue.empty();
// Formula (slice 6331): actionEmpty && timerEmpty
inline auto IsBothQueuesEmpty(const bool actionEmpty, const bool timerEmpty) -> bool
{
    return actionEmpty && timerEmpty;
}

} // namespace actionqueuehelpers
